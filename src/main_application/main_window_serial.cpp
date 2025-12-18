#include "gui_elements.h"
#include "main_window.h"

using namespace duoplot::internal;

void MainWindow::updateSerialDeviceAboutGuiState()
{
    if (gui_transfer_state_ == GuiTransferState::Idle)
    {
        if (idle_counter_ == 100)
        {
            idle_counter_ = 0;

            BufferedWriter buffered_writer{serial_array_, kSerialSendBufferSize};

            buffered_writer.advance(sizeof(uint16_t));  // For the payload which is at the start, but is not yet known
            uint16_t num_bytes_to_write = 0U;

            // Send full state
            for (const auto& ge : gui_elements_)
            {
                const std::shared_ptr<GuiElementSettings> gui_element_settings =
                    std::dynamic_pointer_cast<GuiElementSettings>(ge.second->getElementSettings());

                const std::shared_ptr<GuiElementState> gui_element_state = ge.second->getGuiElementState();

                buffered_writer.write(gui_element_settings->id);
                num_bytes_to_write += sizeof(GuiElementId);
                num_bytes_to_write += gui_element_state->serializeToSerialBuffer(buffered_writer);
            }

            std::memcpy(serial_array_, &num_bytes_to_write, sizeof(uint16_t));
            serial_interface_.publishData(buffered_writer.data(), num_bytes_to_write + sizeof(num_bytes_to_write));
        }
        idle_counter_++;
    }
    else if (gui_transfer_state_ == GuiTransferState::SendingGuiData)
    {
        BufferedWriter buffered_writer{serial_array_, kSerialSendBufferSize};

        buffered_writer.advance(sizeof(uint16_t));  // For the payload which is at the start, but is not yet known
        uint16_t num_bytes_to_write = 0U;

        for (const auto& ge : gui_elements_)
        {
            const std::shared_ptr<GuiElementSettings> gui_element_settings =
                std::dynamic_pointer_cast<GuiElementSettings>(ge.second->getElementSettings());

            const std::shared_ptr<GuiElementState> gui_element_state = ge.second->getGuiElementState();

            bool state_has_changed = false;

            if (gui_elements_states_.find(gui_element_settings->id) != gui_elements_states_.end())
            {
                // GuiState already exists in map
                if (!gui_elements_states_[gui_element_settings->id]->isEqual(gui_element_state))
                {
                    state_has_changed = true;
                    gui_elements_states_.at(gui_element_settings->id) = gui_element_state;
                }
            }
            else
            {
                // GuiState does not exist in map
                state_has_changed = true;
                gui_elements_states_.insert({gui_element_settings->id, gui_element_state});
            }

            if (state_has_changed)
            {
                buffered_writer.write(gui_element_settings->id);
                num_bytes_to_write += sizeof(GuiElementId);
                num_bytes_to_write += gui_element_state->serializeToSerialBuffer(buffered_writer);
            }
        }

        if (num_bytes_to_write == 0U)
        {
            control_state_ = 0U;
            return;
        }
        else
        {
            std::memcpy(serial_array_, &num_bytes_to_write, sizeof(uint16_t));
            serial_interface_.publishData(buffered_writer.data(), num_bytes_to_write + 2U);

            time_of_sending_gui_data_ = static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::milliseconds>(
                                                                  std::chrono::system_clock::now().time_since_epoch())
                                                                  .count());
            control_state_ = 2U;
            gui_transfer_state_ = GuiTransferState::WaitingForControlMessage;
        }
    }
}

void MainWindow::pushNewDataToQueue(const TopicId topic_id, const std::shared_ptr<objects::BaseObject>& obj)
{
    if (objects_temporary_storage_.find(topic_id) != objects_temporary_storage_.end())
    {
        objects_temporary_storage_.at(topic_id).push_back(obj);
    }
}

void MainWindow::handleSerialData()
{
    std::vector<RawDataFrame> raw_data_frames{serial_interface_.extractRawDataFrames()};

    for (const RawDataFrame& raw_data_frame : raw_data_frames)
    {
        if (raw_data_frame.size() == 0U)
        {
            std::cout << "Error: Received data size is 0!" << std::endl;
            continue;
        }

        BufferedReader reader{raw_data_frame};

        uint8_t has_timestamp = 1U;
        reader.readByte(has_timestamp);

        uint64_t timestamp;
        if (has_timestamp == 1U)
        {
            uint32_t timestamp_32;
            reader.read(timestamp_32);
            timestamp = static_cast<uint64_t>(timestamp_32);
        }
        else
        {
            const std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch());
            timestamp = static_cast<uint64_t>(ms.count()) - time_at_start_;
        }

        size_t num_objects;
        reader.readByte(num_objects);

        for (size_t k = 0; k < num_objects; k++)
        {
            TopicId topic_id;
            reader.readUInt16(topic_id);

            ObjectType object_type;
            reader.readByte(object_type);

            std::shared_ptr<objects::BaseObject> obj;

            if (ObjectType::kNumber == object_type)
            {
                NumberDataType number_data_type;
                reader.readByte(number_data_type);

                if (number_data_type == NumberDataType::kFloat)
                {
                    obj = std::make_shared<objects::Float>(reader, topic_id, timestamp);
                }
                else if (number_data_type == NumberDataType::kDouble)
                {
                    obj = std::make_shared<objects::Double>(reader, topic_id, timestamp);
                }
                else if (number_data_type == NumberDataType::kInt8)
                {
                    obj = std::make_shared<objects::Int8>(reader, topic_id, timestamp);
                }
                else if (number_data_type == NumberDataType::kInt16)
                {
                    obj = std::make_shared<objects::Int16>(reader, topic_id, timestamp);
                }
                else if (number_data_type == NumberDataType::kInt32)
                {
                    obj = std::make_shared<objects::Int32>(reader, topic_id, timestamp);
                }
                else if (number_data_type == NumberDataType::kInt64)
                {
                    obj = std::make_shared<objects::Int64>(reader, topic_id, timestamp);
                }
                else if (number_data_type == NumberDataType::kUInt8)
                {
                    obj = std::make_shared<objects::UInt8>(reader, topic_id, timestamp);
                }
                else if (number_data_type == NumberDataType::kUInt16)
                {
                    obj = std::make_shared<objects::UInt16>(reader, topic_id, timestamp);
                }
                else if (number_data_type == NumberDataType::kUInt32)
                {
                    obj = std::make_shared<objects::UInt32>(reader, topic_id, timestamp);
                }
                else if (number_data_type == NumberDataType::kUInt64)
                {
                    obj = std::make_shared<objects::UInt64>(reader, topic_id, timestamp);
                }
            }
            else if (ObjectType::kString == object_type)
            {
                std::string s(reinterpret_cast<const char* const>(reader.data() + reader.idx()));
                reader.advance(s.size() + 1U);

                streams_of_strings_[topic_id].push_back({timestamp, s});
                continue;  // TODO: Added this here without testing it with string streams from device
            }

            if (topic_id == 0xFFFEU)
            {
                const uint8_t value = static_cast<objects::UInt8*>(obj.get())->value();

                // Waiting for control message
                if (value == 0U)
                {
                    control_state_ = 1U;
                    gui_transfer_state_ = GuiTransferState::SendingGuiData;
                }
            }
            else
            {
                pushNewDataToQueue(topic_id, obj);
            }

            if (!reader.hasReadToEnd())
            {
                for (size_t k = 0; k < raw_data_frame.size(); k++)
                {
                    std::cout << "0x" << std::hex << static_cast<int>(raw_data_frame.data()[k]) << std::dec << " ";
                }
                std::cout << std::endl;
                std::cout << "Error: Did not read all data!" << std::endl;
            }
        }

        for (auto& [topic_id, objects] : objects_temporary_storage_)
        {
            if (plot_pane_subscriptions_.find(topic_id) == plot_pane_subscriptions_.end() || objects.empty())
            {
                continue;
            }

            const std::vector<PlotPane*>& plot_panes_to_push_data_to = plot_pane_subscriptions_[topic_id];
            for (PlotPane* const plot_pane : plot_panes_to_push_data_to)
            {
                plot_pane->pushStreamData(topic_id, objects);
            }
            objects.clear();
        }

        for (auto& [topic_id, strings] : streams_of_strings_)
        {
            const std::string t_id = std::to_string(topic_id);
            for (const std::pair<uint64_t, std::string>& s : strings)
            {
                topic_text_output_window_->pushNewText(Color_t::RED, t_id);
                topic_text_output_window_->pushNewText(Color_t::BLACK, ": " + s.second + "\n");
            }

            if (stream_of_strings_subscriptions_.find(topic_id) == stream_of_strings_subscriptions_.end() ||
                strings.empty())
            {
                strings.clear();
                continue;
            }
            else
            {
                const std::vector<ScrollingTextGuiElement*>& text_elements_to_push_data_to =
                    stream_of_strings_subscriptions_[topic_id];
                for (ScrollingTextGuiElement* text_element : text_elements_to_push_data_to)
                {
                    text_element->pushNewText(topic_id, strings);
                }
                strings.clear();
            }
        }

        if (gui_transfer_state_ == GuiTransferState::WaitingForControlMessage)
        {
            num_iterations_without_control_message_++;
        }

        if (num_iterations_without_control_message_ > 100)
        {
            gui_transfer_state_ = GuiTransferState::Idle;
            num_iterations_without_control_message_ = 0;
        }

        updateSerialDeviceAboutGuiState();
    }
}
