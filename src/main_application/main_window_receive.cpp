#include <unistd.h>

#include <chrono>
#include <csignal>
#include <iostream>
#include <stdexcept>

#include "duoplot/math/math.h"
#include "gui_window.h"
#include "main_window.h"
#include "plot_objects/plot_object_base/plot_object_base.h"

using namespace duoplot::internal;

void MainWindow::setActiveView(const ReceivedData& received_data)
{
    const CommunicationHeader& hdr = received_data.getCommunicationHeader();

    const std::string current_element_name =
        hdr.get(CommunicationHeaderObjectType::ELEMENT_NAME).as<properties::Label>().data;

    if (current_element_name.length() == 0)
    {
        LUMOS_LOG_WARNING() << "Label string had zero length!";
        return;
    }

    current_element_name_ = current_element_name;

    if (plot_panes_.count(current_element_name_) == 0)
    {
        new_window_queued_ = true;
    }
}

void MainWindow::OnReceiveTimer(wxTimerEvent&)
{
    handleSerialData();

    try
    {
        receiveData();
    }
    catch (const std::runtime_error& e)
    {
        std::cerr << "Got runtime_error when receiving: " << e.what() << std::endl;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Got exception when receiving: " << e.what() << std::endl;
    }
}

std::shared_ptr<const ConvertedDataBase> convertPlotObjectData(const CommunicationHeader& hdr,
                                                               const ReceivedData& received_data,
                                                               const PlotObjectAttributes& attributes,
                                                               const UserSuppliedProperties& user_supplied_properties)
{
    const Function fcn = received_data.getFunction();

    std::shared_ptr<const ConvertedDataBase> converted_data;

    switch (fcn)
    {
        case Function::STAIRS:
            converted_data =
                Stairs::convertRawData(hdr, attributes, user_supplied_properties, received_data.payloadData());
            break;

        case Function::PLOT2:
            converted_data =
                Plot2D::convertRawData(hdr, attributes, user_supplied_properties, received_data.payloadData());
            break;

        case Function::PLOT3:
            converted_data =
                Plot3D::convertRawData(hdr, attributes, user_supplied_properties, received_data.payloadData());
            break;

        case Function::SCREEN_SPACE_PRIMITIVE:
            converted_data = ScreenSpacePrimitive::convertRawData(
                hdr, attributes, user_supplied_properties, received_data.payloadData());
            break;

        case Function::FAST_PLOT2:
            converted_data =
                FastPlot2D::convertRawData(hdr, attributes, user_supplied_properties, received_data.payloadData());
            break;

        case Function::FAST_PLOT3:
            converted_data =
                FastPlot3D::convertRawData(hdr, attributes, user_supplied_properties, received_data.payloadData());
            break;

        case Function::LINE_COLLECTION2:
            converted_data = LineCollection2D::convertRawData(
                hdr, attributes, user_supplied_properties, received_data.payloadData());
            break;

        case Function::LINE_COLLECTION3:
            converted_data = LineCollection3D::convertRawData(
                hdr, attributes, user_supplied_properties, received_data.payloadData());
            break;

        case Function::STEM:
            converted_data =
                Stem::convertRawData(hdr, attributes, user_supplied_properties, received_data.payloadData());
            break;

        case Function::SCATTER2:
            converted_data =
                Scatter2D::convertRawData(hdr, attributes, user_supplied_properties, received_data.payloadData());
            break;

        case Function::SCATTER3:
            converted_data =
                Scatter3D::convertRawData(hdr, attributes, user_supplied_properties, received_data.payloadData());
            break;

        case Function::SURF:
            converted_data =
                Surf::convertRawData(hdr, attributes, user_supplied_properties, received_data.payloadData());
            break;

        case Function::IM_SHOW:
            converted_data =
                ImShow::convertRawData(hdr, attributes, user_supplied_properties, received_data.payloadData());
            break;

        case Function::PLOT_COLLECTION2:
            converted_data = PlotCollection2D::convertRawData(
                hdr, attributes, user_supplied_properties, received_data.payloadData());
            break;

        case Function::PLOT_COLLECTION3:
            converted_data = PlotCollection3D::convertRawData(
                hdr, attributes, user_supplied_properties, received_data.payloadData());
            break;

        case Function::DRAW_MESH_SEPARATE_VECTORS:
        case Function::DRAW_MESH:
            converted_data =
                DrawMesh::convertRawData(hdr, attributes, user_supplied_properties, received_data.payloadData());
            break;

        case Function::REAL_TIME_PLOT:
            converted_data =
                ScrollingPlot2D::convertRawData(hdr, attributes, user_supplied_properties, received_data.payloadData());
            break;
        default:
            throw std::runtime_error("Invalid function!");
            break;
    }

    return converted_data;
}

void MainWindow::addActionToQueue(ReceivedData& received_data)
{
    const Function fcn = received_data.getFunction();

    if (fcn == Function::SET_CURRENT_ELEMENT)
    {
        setActiveView(received_data);
    }
    else if (fcn == Function::FLUSH_MULTIPLE_ELEMENTS)
    {
        mainWindowFlushMultipleElements(received_data);
    }
    else if (isPlotDataFunction(fcn))
    {
        const CommunicationHeader& hdr{received_data.getCommunicationHeader()};
        const PlotObjectAttributes plot_object_attributes{hdr};
        const UserSuppliedProperties user_supplied_properties{hdr};

        std::shared_ptr<const ConvertedDataBase> converted_data =
            convertPlotObjectData(hdr, received_data, plot_object_attributes, user_supplied_properties);

        queued_data_[current_element_name_].push(std::make_unique<InputData>(
            received_data, converted_data, plot_object_attributes, user_supplied_properties));
    }
    else if (fcn == Function::PROPERTIES_EXTENSION)
    {
        const CommunicationHeader& hdr{received_data.getCommunicationHeader()};
        const PlotObjectAttributes plot_object_attributes{hdr};
        const UserSuppliedProperties user_supplied_properties{hdr};
        queued_data_[current_element_name_].push(
            std::make_unique<InputData>(received_data, plot_object_attributes, user_supplied_properties));
    }
    else if (fcn == Function::PROPERTIES_EXTENSION_MULTIPLE)
    {
        const CommunicationHeader& hdr{received_data.getCommunicationHeader()};
        const PlotObjectAttributes plot_object_attributes{hdr};
        const UserSuppliedProperties user_supplied_properties{hdr};
        queued_data_[current_element_name_].push(
            std::make_unique<InputData>(received_data, plot_object_attributes, user_supplied_properties));
    }
    else
    {
        queued_data_[current_element_name_].push(std::make_unique<InputData>(received_data));
    }
}

bool isGuiRelatedFunction(const Function fcn)
{
    return fcn == Function::SET_GUI_ELEMENT_LABEL;
}

void MainWindow::handleGuiManipulation(ReceivedData& received_data)
{
    const Function fcn = received_data.getFunction();

    const CommunicationHeader& hdr{received_data.getCommunicationHeader()};
    const std::string handle_string =
        hdr.get(CommunicationHeaderObjectType::HANDLE_STRING).as<properties::Label>().data;
    const std::string label = hdr.get(CommunicationHeaderObjectType::LABEL).as<properties::Label>().data;

    gui_elements_[handle_string]->setLabel(label);
}

void MainWindow::manageReceivedData(ReceivedData& received_data)
{
    const Function fcn = received_data.getFunction();

    {
        const std::lock_guard<std::mutex> lg(receive_mtx_);

        if (fcn == Function::OPEN_PROJECT_FILE)
        {
            const CommunicationHeader& hdr = received_data.getCommunicationHeader();
            queued_project_file_name_ =
                hdr.get(CommunicationHeaderObjectType::PROJECT_FILE_NAME).as<properties::Label>().data;
            open_project_file_queued_ = true;
        }
        else if (fcn == Function::SCREENSHOT)
        {
            const CommunicationHeader& hdr = received_data.getCommunicationHeader();
            std::string screenshot_base_path =
                hdr.get(CommunicationHeaderObjectType::SCREENSHOT_BASE_PATH).as<properties::Label>().data;
            performScreenshot(screenshot_base_path);
        }
        else if (fcn == Function::QUERY_FOR_SYNC_OF_GUI_DATA)
        {
            updateClientApplicationAboutGuiState();
        }
        else if (isGuiRelatedFunction(fcn))
        {
            handleGuiManipulation(received_data);
        }
        else
        {
            addActionToQueue(received_data);
        }
    }
}

void MainWindow::tcpReceiveThreadFunction()
{
    while (1)
    {
        ReceivedData received_data = data_receiver_.receiveAndGetDataFromTcp();
        if (received_data.rawData() == nullptr)
        {
            continue;
        }

        /*std::atomic<bool> received_data_has_been_moved = false;

        std::thread local_thread([this, &received_data, &received_data_has_been_moved] {
            // LUMOS_LOG_DEBUG() << "In thread!";
            ReceivedData local_received_data{std::move(received_data)};
            received_data_has_been_moved = true;
            manageReceivedData(local_received_data);
        });
        local_thread.detach();

        while (!received_data_has_been_moved)
            ;*/

        manageReceivedData(received_data);
    }
}

void MainWindow::mainWindowFlushMultipleElements(const ReceivedData& received_data)
{
    const internal::CommunicationHeader& hdr = received_data.getCommunicationHeader();

    const uint8_t num_names = hdr.get(CommunicationHeaderObjectType::NUM_NAMES).as<uint8_t>();

    const VectorConstView<uint8_t> name_lengths{received_data.payloadData(), static_cast<size_t>(num_names)};

    std::vector<std::string> names;

    size_t idx = num_names;

    for (size_t k = 0; k < num_names; k++)
    {
        names.push_back("");
        std::string& current_elem = names.back();

        const uint8_t current_element_length = name_lengths(k);

        for (size_t i = 0; i < current_element_length; i++)
        {
            current_elem += received_data.payloadData()[idx];
            idx++;
        }
    }

    // Have to create a 'fake' header and 'fake' received data
    internal::CommunicationHeader faked_hdr{internal::Function::FLUSH_ELEMENT};

    const uint64_t num_bytes_hdr = faked_hdr.numBytes();

    // + 1 bytes for endianness byte
    // + 2 * sizeof(uint64_t) for magic number and number of bytes in transfer
    const uint64_t num_bytes = num_bytes_hdr + 1 + 2 * sizeof(uint64_t);

    FillableUInt8Array fillable_array{num_bytes};

    fillable_array.fillWithStaticType(isBigEndian());
    fillable_array.fillWithStaticType(kMagicNumber);
    fillable_array.fillWithStaticType(num_bytes);

    faked_hdr.fillBufferWithData(fillable_array);

    const UInt8ArrayView array_view{fillable_array.data(), fillable_array.size()};

    for (size_t k = 0; k < names.size(); k++)
    {
        ReceivedData fake_received_data{array_view.size()};
        std::memcpy(fake_received_data.rawData(), array_view.data(), array_view.size());
        fake_received_data.parseHeader();

        queued_data_[names[k]].push(std::make_unique<InputData>(fake_received_data));
    }
}

void MainWindow::receiveData()
{
    {
        const std::lock_guard<std::mutex> lg(receive_mtx_);

        if (open_project_file_queued_)
        {
            open_project_file_queued_ = false;
            openExistingFile(queued_project_file_name_);
        }

        if (new_window_queued_)
        {
            new_window_queued_ = false;
            newWindowWithoutFileModification(current_element_name_);
        }

        for (auto& qa : queued_data_)
        {
            if (!qa.second.empty())
            {
                const std::string element_handle_string = qa.first;

                if (plot_panes_.count(element_handle_string) > 0U)
                {
                    PlotPane* const plot_pane = dynamic_cast<PlotPane*>(plot_panes_[element_handle_string]);
                    plot_pane->pushQueue(qa.second);
                }
            }
        }
    }

    for (auto& ge : plot_panes_)
    {
        PlotPane* const plot_pane = dynamic_cast<PlotPane*>(ge.second);
        plot_pane->update();
    }
}
