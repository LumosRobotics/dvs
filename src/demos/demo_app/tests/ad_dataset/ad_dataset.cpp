#include "ad_dataset.h"

#include <curses.h>
#include <duoplot/timing.h>

#include <algorithm>
#include <filesystem>
#include <fstream>

namespace ad_dataset
{

DatasetReaderFast::DatasetReaderFast(const std::string& dataset_file)
{
    auto t0 = duoplot::timing::getTimeNow();
    std::ifstream input_file(dataset_file, std::ios::binary);

    input_file.read(reinterpret_cast<char*>(&total_num_bytes_), sizeof(size_t));
    input_file.read(reinterpret_cast<char*>(&num_frames_), sizeof(size_t));

    input_file.read(reinterpret_cast<char*>(&img_width_), sizeof(size_t));
    input_file.read(reinterpret_cast<char*>(&img_height_), sizeof(size_t));

    assert((total_num_bytes_ / (1024U * 1024U)) < 2000U);

    file_raw_data_ = new uint8_t[total_num_bytes_];

    input_file.read(reinterpret_cast<char*>(file_raw_data_), total_num_bytes_);

    readFiles();
    auto t1 = duoplot::timing::getTimeNow();

    std::cout << "Read in time: " << duoplot::timing::timePointsToMsDouble(t0, t1) << std::endl;
}

void DatasetReaderFast::readFiles()
{
    size_t data_idx = 0U;

    const size_t num_bytes_in_image = img_width_ * img_height_ * 3U * sizeof(uint8_t);

    for (size_t k = 0; k < num_frames_; k++)
    {
        images_front_.emplace_back(file_raw_data_ + data_idx, img_height_, img_width_);

        data_idx += num_bytes_in_image;
    }

    for (size_t k = 0; k < num_frames_; k++)
    {
        images_left_.emplace_back(file_raw_data_ + data_idx, img_height_, img_width_);

        data_idx += num_bytes_in_image;
    }

    for (size_t k = 0; k < num_frames_; k++)
    {
        images_right_.emplace_back(file_raw_data_ + data_idx, img_height_, img_width_);

        data_idx += num_bytes_in_image;
    }

    for (size_t k = 0; k < num_frames_; k++)
    {
        size_t num_pts;
        std::memcpy(&num_pts, file_raw_data_ + data_idx, sizeof(size_t));
        data_idx += sizeof(size_t);

        const VectorConstView<float> x{reinterpret_cast<const float* const>(file_raw_data_ + data_idx), num_pts};
        data_idx += num_pts * sizeof(float);

        const VectorConstView<float> y{reinterpret_cast<const float* const>(file_raw_data_ + data_idx), num_pts};
        data_idx += num_pts * sizeof(float);

        const VectorConstView<float> z{reinterpret_cast<const float* const>(file_raw_data_ + data_idx), num_pts};
        data_idx += num_pts * sizeof(float);

        point_collections_.emplace_back(x, y, z);
    }

    assert(data_idx == total_num_bytes_);
}

PointCollection DatasetReaderFast::getPointCollection(const size_t idx)
{
    return point_collections_[idx];
}

ImageRGBConstView<uint8> DatasetReaderFast::getLeftImage(const size_t idx)
{
    return images_left_[idx];
}

ImageRGBConstView<uint8> DatasetReaderFast::getRightImage(const size_t idx)
{
    return images_right_[idx];
}

ImageRGBConstView<uint8> DatasetReaderFast::getFrontImage(const size_t idx)
{
    return images_front_[idx];
}

size_t DatasetReaderFast::numFrames() const
{
    return num_frames_;
}

void DatasetReader::readCameraFile(const std::string& bin_path, std::vector<ImageRGB<uint8_t>>& destination)
{
    std::ifstream input(bin_path, std::ios::binary);

    std::vector<unsigned char> img_raw_data(std::istreambuf_iterator<char>(input), {});

    uint32_t width, height;  // From python: Width = 720, height = 540

    size_t idx = 0U;

    std::memcpy(&width, img_raw_data.data(), sizeof(uint32_t));
    idx += sizeof(uint32_t);

    std::memcpy(&height, img_raw_data.data() + idx, sizeof(uint32_t));
    idx += sizeof(uint32_t);

    ImageGrayConstView<uint8_t> img_red{img_raw_data.data() + idx, height, width};
    idx += sizeof(uint8_t) * width * height;

    ImageGrayConstView<uint8_t> img_green{img_raw_data.data() + idx, height, width};
    idx += sizeof(uint8_t) * width * height;

    ImageGrayConstView<uint8_t> img_blue{img_raw_data.data() + idx, height, width};
    idx += sizeof(uint8_t) * width * height;

    ImageRGB<uint8_t> img{height, width};

    for (size_t r = 0; r < height; r++)
    {
        for (size_t c = 0; c < width; c++)
        {
            img(r, c, 0) = img_red(r, c);
            img(r, c, 1) = img_green(r, c);
            img(r, c, 2) = img_blue(r, c);
        }
    }

    destination.push_back(std::move(img));
}

void DatasetReader::readLidarFile(const std::string& bin_path)
{
    std::ifstream input(bin_path, std::ios::binary);

    lidar_raw_data_.push_back(std::vector<unsigned char>(std::istreambuf_iterator<char>(input), {}));
    // lidar_raw_data_.push_back(std::vector<unsigned char>(std::istreambuf_iterator<char>(input), {}));

    std::vector<unsigned char>& read_lidar_data = lidar_raw_data_.back();

    uint32_t n_points;
    std::memcpy(&n_points, read_lidar_data.data(), sizeof(uint32_t));

    PointCollection pc;

    const float* const x_ptr = reinterpret_cast<const float* const>(read_lidar_data.data() + sizeof(uint32_t));
    const float* const y_ptr =
        reinterpret_cast<const float* const>(read_lidar_data.data() + sizeof(uint32_t) + sizeof(float) * n_points);
    const float* const z_ptr =
        reinterpret_cast<const float* const>(read_lidar_data.data() + sizeof(uint32_t) + 2 * sizeof(float) * n_points);

    pc.x = VectorConstView<float>{x_ptr, n_points};
    pc.y = VectorConstView<float>{y_ptr, n_points};
    pc.z = VectorConstView<float>{z_ptr, n_points};

    point_collections_.push_back(pc);
}

void DatasetReader::readCamera(const std::string& folder_path, std::vector<ImageRGB<uint8_t>>& destination)
{
    std::vector<std::string> img_file_paths;

    for (const auto& entry : std::filesystem::directory_iterator(folder_path))
    {
        img_file_paths.push_back(entry.path());
    }

    std::sort(img_file_paths.begin(), img_file_paths.end());

    size_t idx = 0;
    for (const std::string& img_file_path : img_file_paths)
    {
        readCameraFile(img_file_path, destination);

        if (idx > kMaxReadFrame)
        {
            break;
        }
        idx++;
    }
}

void DatasetReader::saveToBigBlob(const std::string& output_folder_path) const
{
    assert(images_front_.size() == images_left_.size());
    assert(images_front_.size() == images_right_.size());
    assert(images_front_.size() == point_collections_.size());

    size_t total_num_bytes = 0U;

    total_num_bytes += (images_front_[0].numBytes() * images_front_.size() * 3U);

    total_num_bytes += point_collections_.size() * sizeof(size_t);  // For num points in each PC

    for (size_t k = 0; k < point_collections_.size(); k++)
    {
        const PointCollection& pc = point_collections_[k];
        total_num_bytes += pc.x.numBytes();
        total_num_bytes += pc.y.numBytes();
        total_num_bytes += pc.z.numBytes();
    }

    const std::string separator_str = output_folder_path.back() == '/' ? "" : "/";

    const std::string file_name =
        output_folder_path + separator_str + "all_data_" + std::to_string(images_front_.size()) + ".bin";

    std::ofstream bin_stream(file_name, std::ios::out | std::ios::binary);

    // Total num bytes
    bin_stream.write(reinterpret_cast<const char*>(&total_num_bytes), sizeof(size_t));

    // Total num frames
    const size_t num_files = images_front_.size();
    bin_stream.write(reinterpret_cast<const char*>(&num_files), sizeof(size_t));

    // Image width
    const size_t img_width = images_front_[0].numCols();
    bin_stream.write(reinterpret_cast<const char*>(&img_width), sizeof(size_t));

    // Image height
    const size_t img_height = images_front_[0].numRows();
    bin_stream.write(reinterpret_cast<const char*>(&img_height), sizeof(size_t));

    // Front image
    for (size_t k = 0; k < images_front_.size(); k++)
    {
        const ImageRGBConstView<uint8> img_front = images_front_[k].constView();
        bin_stream.write(reinterpret_cast<const char*>(img_front.data()), img_front.numBytes());
    }

    // Left image
    for (size_t k = 0; k < images_front_.size(); k++)
    {
        const ImageRGBConstView<uint8> img_left = images_left_[k].constView();
        bin_stream.write(reinterpret_cast<const char*>(img_left.data()), img_left.numBytes());
    }

    // Right image
    for (size_t k = 0; k < images_front_.size(); k++)
    {
        const ImageRGBConstView<uint8> img_right = images_right_[k].constView();
        bin_stream.write(reinterpret_cast<const char*>(img_right.data()), img_right.numBytes());
    }

    // Point cloud
    for (size_t k = 0; k < images_front_.size(); k++)
    {
        const PointCollection& pc = point_collections_[k];

        const size_t num_pts = pc.x.size();
        bin_stream.write(reinterpret_cast<const char*>(&num_pts), sizeof(size_t));

        bin_stream.write(reinterpret_cast<const char*>(pc.x.data()), pc.x.numBytes());
        bin_stream.write(reinterpret_cast<const char*>(pc.y.data()), pc.y.numBytes());
        bin_stream.write(reinterpret_cast<const char*>(pc.z.data()), pc.z.numBytes());
    }

    bin_stream.close();
}

DatasetReader::DatasetReader(const std::string& dataset_root_path)
{
    const std::string ds_path = dataset_root_path.back() == '/' ? dataset_root_path : dataset_root_path + "/";

    // Lidar
    const std::string lidar_path = ds_path + "lidar/";

    std::vector<std::string> lidar_file_paths;

    for (const auto& entry : std::filesystem::directory_iterator(lidar_path))
    {
        lidar_file_paths.push_back(entry.path());
    }

    std::sort(lidar_file_paths.begin(), lidar_file_paths.end());

    size_t idx = 0U;

    for (const std::string& lidar_file_path : lidar_file_paths)
    {
        readLidarFile(lidar_file_path);

        if (idx > kMaxReadFrame)
        {
            break;
        }
        idx++;
    }

    // Camera
    readCamera(ds_path + "img_front/", images_front_);
    readCamera(ds_path + "img_left/", images_left_);
    readCamera(ds_path + "img_right/", images_right_);

    std::cout << "Finished reading files!" << std::endl;
}

std::vector<std::pair<Vector<float>, Vector<float>>> generateCircles(const size_t n_circles,
                                                                     const float r0,
                                                                     const float d)
{
    std::vector<std::pair<Vector<float>, Vector<float>>> res;

    float r = r0;
    const size_t n_points = 100;

    for (size_t k = 0; k < n_circles; k++)
    {
        Vector<float> xp{n_points + 1}, yp{n_points + 1};

        float t = 0;
        const float dt = 2.0f * M_PI / static_cast<float>(n_points);

        for (size_t i = 0; i < n_points; i++)
        {
            xp(i) = r * std::cos(t);
            yp(i) = r * std::sin(t);
            t += dt;
        }

        r += d;

        xp(n_points) = xp(0);
        yp(n_points) = yp(0);

        res.push_back(std::make_pair(xp, yp));
    }

    return res;
}

void testBasic()
{
    const std::string project_file_path = "../../project_files/ad_dataset.duoplot";

    DatasetReaderBase* dataset_reader;
    const bool use_fast_reader = true;

    if (use_fast_reader)
    {
        dataset_reader =
            new DatasetReaderFast("../../leddar_dataset/20200706_171559_part27_1170_1370/output/all_data_199.bin");
    }
    else
    {
        dataset_reader = new DatasetReader("../../leddar_dataset/20200706_171559_part27_1170_1370/output");
    }

    // DatasetReader dataset_writer{"../../leddar_dataset/20200706_171559_part27_1170_1370/output"};
    // dataset_writer.saveToBigBlob("../../leddar_dataset/20200706_171559_part27_1170_1370/output/");

    openProjectFile(project_file_path);

    const std::vector<std::pair<Vector<float>, Vector<float>>> circle_points = generateCircles(5, 10.0f, 5.0f);

    setActiveView("point_cloud");
    clearView();
    waitForFlush();
    axis({-20.0, -20.0, -20.0}, {20.0, 20.0, 20.0});
    viewAngles(-38.0, 32.0);
    axesSquare();

    setActiveView("center");
    clearView();
    setAxesBoxScaleFactor({1.0, 1.0, 1.0});
    waitForFlush();

    setActiveView("right");
    clearView();
    setAxesBoxScaleFactor({1.0, 1.0, 1.0});
    waitForFlush();

    setActiveView("left");
    clearView();
    setAxesBoxScaleFactor({1.0, 1.0, 1.0});
    waitForFlush();

    bool should_run_free = false;
    int current_frame = 0;

    duoplot::gui::startGuiReceiveThread();

    duoplot::gui::registerGuiCallback(
        "slider0", [&should_run_free, &current_frame](const duoplot::gui::SliderHandle& gui_element_handle) -> void {
            current_frame = gui_element_handle.getValue();
        });

    duoplot::gui::registerGuiCallback(
        "button0", [&should_run_free, &current_frame](const duoplot::gui::ButtonHandle& gui_element_handle) -> void {
            should_run_free = !should_run_free;

            std::cout << "Current frame: " << current_frame << std::endl;
        });

    const auto plot_data = [&dataset_reader, &circle_points](const int k) -> void {
        const ImageRGBConstView<uint8_t> img_front = dataset_reader->getFrontImage(k);
        const ImageRGBConstView<uint8_t> img_left = dataset_reader->getLeftImage(k);
        const ImageRGBConstView<uint8_t> img_right = dataset_reader->getRightImage(k);

        const PointCollection& pc = dataset_reader->getPointCollection(k);

        setActiveView("point_cloud");
        softClearView();
        scatter3(pc.x,
                 pc.y,
                 pc.z,
                 properties::DistanceFrom::xyz({0, 0, 0}, 0.0, 64.0),
                 properties::ColorMap::VIRIDIS,
                 properties::PointSize(5),
                 properties::ScatterStyle::DISC);
        for (const auto& circ_pts : circle_points)
        {
            plot(circ_pts.first, circ_pts.second, properties::Color::WHITE, properties::LineWidth(3));
        }

        setActiveView("center");
        softClearView();
        imShow(img_front);

        setActiveView("right");
        softClearView();
        imShow(img_right);

        setActiveView("left");
        softClearView();
        imShow(img_left);

        flushMultipleElements("point_cloud", "center", "left", "right");
    };

    int previous_frame = current_frame;
    while (true)
    {
        if (should_run_free)
        {
            current_frame++;
            current_frame = current_frame <= 199 ? current_frame : 0;
        }

        if (previous_frame != current_frame)
        {
            plot_data(current_frame);
            // usleep(1000U * 10U);
        }

        previous_frame = current_frame;
    }

    /*for (size_t k = 0; k < dataset_reader->numFrames(); k++)
    {
        const ImageRGBConstView<uint8_t> img_front = dataset_reader->getFrontImage(k);
        const ImageRGBConstView<uint8_t> img_left = dataset_reader->getLeftImage(k);
        const ImageRGBConstView<uint8_t> img_right = dataset_reader->getRightImage(k);

        const PointCollection& pc = dataset_reader->getPointCollection(k);

        setActiveView("point_cloud");
        softClearView();
        scatter3(pc.x,
                 pc.y,
                 pc.z,
                 properties::DistanceFrom::xyz({0, 0, 0}, 0.0, 64.0),
                 properties::ColorMap::VIRIDIS,
                 properties::PointSize(5),
                 properties::ScatterStyle::DISC);
        for (const auto& circ_pts : circle_points)
        {
            plot(circ_pts.first, circ_pts.second, properties::Color::WHITE, properties::LineWidth(3));
        }

        setActiveView("center");
        softClearView();
        imShow(img_front);

        setActiveView("right");
        softClearView();
        imShow(img_right);

        setActiveView("left");
        softClearView();
        imShow(img_left);

        flushMultipleElements("point_cloud", "center", "left", "right");
        // screenshot("/Users/danielpi/work/dvs/demo_screenshots_output/ad_dataset");
        if ((k % 10) == 0)
        {
            std::cout << "Iter: " << k << std::endl;
        }
    }*/
}

void testScroll()
{
    initscr();

    DatasetReader* dataset_reader = new DatasetReader("../../leddar_dataset/20200706_171559_part27_1170_1370/output");

    setActiveView("point_cloud");
    clearView();
    waitForFlush();
    axis({-20.0, -20.0, -20.0}, {20.0, 20.0, 20.0});
    viewAngles(-38.0, 32.0);

    setActiveView("right");
    clearView();
    waitForFlush();
    axis({-20.0, -20.0, -20.0}, {20.0, 20.0, 20.0});
    viewAngles(-38.0, 32.0);

    size_t idx = 0U;

    while (true)
    {
        const PointCollection& pc = dataset_reader->getPointCollection(idx);

        setActiveView("main");
        softClearView();
        scatter3(pc.x,
                 pc.y,
                 pc.z,
                 properties::DistanceFrom::xyz({0, 0, 0}, 0.0, 64.0),
                 properties::ColorMap::VIRIDIS,
                 properties::PointSize(5),
                 properties::ScatterStyle::DISC);

        setActiveView("secondary");
        softClearView();
        scatter3(pc.x,
                 pc.y,
                 pc.z,
                 properties::DistanceFrom::xyz({0, 0, 0}, 0.0, 64.0),
                 properties::ColorMap::VIRIDIS,
                 properties::PointSize(5),
                 properties::ScatterStyle::DISC);

        flushMultipleElements("main", "secondary");

        char q = getch();

        if (q == 'a')
        {
            idx = (idx == 0U) ? 0U : (idx - 1U);
        }
        else if (q == 'd')
        {
            idx = (idx == (dataset_reader->numLidarFiles() - 1U)) ? (dataset_reader->numLidarFiles() - 1U) : (idx + 1U);
        }
        else
        {
            break;
        }
    }
}

}  // namespace ad_dataset
