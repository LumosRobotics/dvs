#include "tests/slam/slam.h"

#include <math.h>
#include <stdlib.h>

#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include "duoplot/duoplot.h"
#include "tests/slam/color_maps.h"

using namespace duoplot;

namespace slam
{

// const std::string dataset_base_path = "/Users/danielpi/Downloads/dataset-corridor1_512_16/";
// const std::string dataset_base_path = "/Users/danielpi/work/dvs/tum_dataset/dataset-corridor1_512_16/";
const std::string dataset_base_path = "/Users/danielpi/work/dvs/tum_dataset/sequence_01/";

void testBasicRunSlam();
void testBasicViewDepthFrame();
void testBasicVisualizePoints();
void testPrintCams();
void testRunSavedDataFromPython();

void testBasic()
{
    testRunSavedDataFromPython();
    // testBasicRunSlam();
    // testBasicViewDepthFrame();
    // testBasicVisualizePoints();
    // testPrintCams();
}

ImageRGB<float> readDepthImage(const std::string& file_path)
{
    std::ifstream input(file_path, std::ios::binary);

    std::vector<unsigned char> buffer(std::istreambuf_iterator<char>(input), {});

    std::uint16_t num_rows;
    std::uint16_t num_cols;

    std::size_t idx = 0U;

    std::memcpy(reinterpret_cast<char*>(&num_rows), buffer.data(), sizeof(std::uint16_t));
    idx += sizeof(std::uint16_t);

    std::memcpy(reinterpret_cast<char*>(&num_cols), buffer.data() + idx, sizeof(std::uint16_t));
    idx += sizeof(std::uint16_t);

    ImageRGB<float> output_img{num_rows, num_cols};

    const size_t num_elements = num_rows * num_cols * sizeof(std::uint8_t);

    ImageRGBConstView<std::uint8_t> input_img{buffer.data() + idx, num_rows, num_cols};

    float min_val = 1.0f, max_val = 0.0f;

    for (size_t r = 0; r < num_rows; r++)
    {
        for (size_t c = 0; c < num_cols; c++)
        {
            const float val = static_cast<float>(input_img(r, c, 0)) / 255.0f;
            if (val < min_val)
            {
                min_val = val;
            }
            if (val > max_val)
            {
                max_val = val;
            }
        }
    }
    const float divisor = max_val - min_val;

    for (size_t r = 0; r < num_rows; r++)
    {
        for (size_t c = 0; c < num_cols; c++)
        {
            const float val = (static_cast<float>(input_img(r, c, 0)) / 255.0f - min_val) / divisor;
            // const RGBTripletf rgb_triplet{calculateColormapJetBright(val)};
            // const RGBTripletf rgb_triplet{calculateColormapJetBright(val)};
            const RGBTripletf rgb_triplet{calculateColormapJet(val)};
            output_img(r, c, 0) = rgb_triplet.red;
            output_img(r, c, 1) = rgb_triplet.green;
            output_img(r, c, 2) = rgb_triplet.blue;
        }
    }

    ImageRGB<float> output_img_cropped{400, 550};

    for (size_t r = 0; r < 400; r++)
    {
        for (size_t c = 0; c < 550; c++)
        {
            output_img_cropped(r, c, 0) = output_img(r + 80, c + 50, 0);
            output_img_cropped(r, c, 1) = output_img(r + 80, c + 50, 1);
            output_img_cropped(r, c, 2) = output_img(r + 80, c + 50, 2);
        }
    }

    return output_img_cropped;
}

ImageRGB<std::uint8_t> readImage(const std::string& file_path)
{
    std::ifstream input(file_path, std::ios::binary);

    std::vector<unsigned char> buffer(std::istreambuf_iterator<char>(input), {});

    std::uint16_t num_rows;
    std::uint16_t num_cols;

    std::size_t idx = 0U;

    std::memcpy(reinterpret_cast<char*>(&num_rows), buffer.data(), sizeof(std::uint16_t));
    idx += sizeof(std::uint16_t);

    std::memcpy(reinterpret_cast<char*>(&num_cols), buffer.data() + idx, sizeof(std::uint16_t));
    idx += sizeof(std::uint16_t);

    ImageRGB<std::uint8_t> output_img{num_rows, num_cols};

    const std::uint8_t* const data_ptr = reinterpret_cast<std::uint8_t*>(buffer.data() + idx);

    const size_t num_elements = num_rows * num_cols * sizeof(std::uint8_t);

    std::memcpy(reinterpret_cast<char*>(output_img.data() + num_elements * 2U),
                buffer.data() + idx,
                num_rows * num_cols * sizeof(std::uint8_t));
    idx += num_elements;

    std::memcpy(reinterpret_cast<char*>(output_img.data() + num_elements),
                buffer.data() + idx,
                num_rows * num_cols * sizeof(std::uint8_t));
    idx += num_elements;

    std::memcpy(
        reinterpret_cast<char*>(output_img.data()), buffer.data() + idx, num_rows * num_cols * sizeof(std::uint8_t));

    return output_img;
}

std::tuple<Vector<float>, Vector<float>, Vector<float>> read3DPoints(const std::string& file_path)
{
    std::ifstream input(file_path, std::ios::binary);

    std::vector<unsigned char> buffer(std::istreambuf_iterator<char>(input), {});

    std::uint16_t num_points;

    std::size_t idx = 0U;

    std::memcpy(reinterpret_cast<char*>(&num_points), buffer.data(), sizeof(std::uint16_t));
    idx += sizeof(std::uint16_t);

    Vector<float> x{num_points}, y{num_points}, z{num_points};

    const double* const data_ptr = reinterpret_cast<double*>(buffer.data() + idx);

    size_t pt_idx = 0U;
    for (size_t k = 0; k < num_points; k++)
    {
        x(k) = data_ptr[pt_idx];
        y(k) = data_ptr[pt_idx + 1];
        z(k) = data_ptr[pt_idx + 2];
        pt_idx += 3U;
    }

    return std::make_tuple(x, y, z);
}

std::vector<std::pair<Vector<float>, Vector<float>>> readKeyPoints(const std::string& file_path)
{
    std::ifstream input(file_path, std::ios::binary);

    std::vector<unsigned char> buffer(std::istreambuf_iterator<char>(input), {});

    std::uint16_t num_frames;

    std::size_t idx = 0U;

    std::memcpy(reinterpret_cast<char*>(&num_frames), buffer.data(), sizeof(std::uint16_t));
    idx += sizeof(std::uint16_t);

    std::vector<std::pair<Vector<float>, Vector<float>>> output_points;

    for (size_t k = 0; k < num_frames; k++)
    {
        std::uint16_t num_points;
        std::memcpy(reinterpret_cast<char*>(&num_points), buffer.data() + idx, sizeof(std::uint16_t));
        idx += sizeof(std::uint16_t);

        Vector<float> x{num_points}, y{num_points};

        const float* const data_ptr = reinterpret_cast<float*>(buffer.data() + idx);
        size_t pt_idx = 0U;

        for (size_t i = 0; i < num_points; i++)
        {
            x(i) = data_ptr[pt_idx];
            y(i) = data_ptr[pt_idx + 1];
            pt_idx += 2U;
        }

        idx += num_points * 2U * sizeof(float);
        output_points.push_back({x, y});
    }

    return output_points;
}

void showOneFrame(const std::string base_path, const size_t frame_num)
{
    const auto zero_padding = [](int num, int digits) -> std::string {
        std::string s = std::to_string(num);
        while (s.length() < digits)
            s = "0" + s;
        return s;
    };

    const std::string f_name = zero_padding(frame_num, 4) + ".bin";

    const ImageRGB<std::uint8_t> rgb_img{readImage(base_path + "img/" + f_name)};
    const ImageRGB<float> depth_img{readDepthImage(base_path + "depth/" + f_name)};

    const std::vector<std::pair<Vector<float>, Vector<float>>> key_points{
        readKeyPoints(base_path + "key_points/" + f_name)};
    auto [x, y, z] = read3DPoints(base_path + "points/" + f_name);

    setActiveView("3d_view");
    clearViewOnUpdate();
    scatter3(x, y, z);

    setActiveView("image_view");
    clearViewOnUpdate();
    imShow(rgb_img);
    scatter(key_points[3].first, key_points[3].second, properties::ZOffset(-0.1f));

    setActiveView("depth_view");
    clearViewOnUpdate();
    imShow(depth_img);
}

void testRunSavedDataFromPython()
{
    const std::string project_file_path = "../../project_files/slam.duoplot";
    const std::string base_path = "/Users/danielpi/work/dvs/slam_output/";

    // /Users/danielpi/work/dvs/slam_output
    // cam/    depth/  img/    points/    key_points/

    const ImageRGB<std::uint8_t> rgb_img{readImage(base_path + "img/0008.bin")};
    const ImageRGB<float> depth_img{readDepthImage(base_path + "depth/0008.bin")};

    const std::vector<std::pair<Vector<float>, Vector<float>>> key_points{
        readKeyPoints(base_path + "key_points/0008.bin")};
    auto [x, y, z] = read3DPoints(base_path + "points/0008.bin");

    openProjectFile(project_file_path);

    setActiveView("3d_view");
    viewAngles(-21.0, -34.0);
    axis({-16.0, -8.0, 0.0}, {24.0, 32.0, 64.0});

    setActiveView("image_view");
    setAxesBoxScaleFactor({1.0, 1.0, 1.0});
    setActiveView("depth_view");
    setAxesBoxScaleFactor({1.0, 1.0, 1.0});

    for (size_t k = 8; k < 34; k++)
    {
        showOneFrame(base_path, k);
    }
}

}  // namespace slam