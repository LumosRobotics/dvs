#include "communication/received_data.h"
#include "lumos/plotting/enumerations.h"
#include "plot_objects/plot_object_base/plot_object_base.h"
#include "plot_objects/plot_objects.h"
#include "user_supplied_properties.h"

using namespace lumos::internal;

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
