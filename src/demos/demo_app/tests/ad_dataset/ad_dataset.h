#ifndef DEMOS_TESTS_AD_DATASET_H_
#define DEMOS_TESTS_AD_DATASET_H_

#include <Box2D/Box2D.h>
#include <Box2D/Particle/b2Particle.h>

#include <fstream>
#include <nlohmann/json.hpp>
#include <string>
#include <tuple>

#include "lumos/plotting/duoplot.h"

using namespace lumos;

namespace ad_dataset
{

struct PointCollection
{
    VectorConstView<float> x;
    VectorConstView<float> y;
    VectorConstView<float> z;

    PointCollection() = default;
    PointCollection(const VectorConstView<float>& x_,
                    const VectorConstView<float>& y_,
                    const VectorConstView<float>& z_)
        : x{x_}, y{y_}, z{z_}
    {
    }
};

class DatasetReaderBase
{
private:
public:
    DatasetReaderBase() = default;

    virtual PointCollection getPointCollection(const size_t idx) = 0;
    virtual ImageRGBConstView<uint8> getLeftImage(const size_t idx) = 0;
    virtual ImageRGBConstView<uint8> getRightImage(const size_t idx) = 0;
    virtual ImageRGBConstView<uint8> getFrontImage(const size_t idx) = 0;
    virtual size_t numFrames() const = 0;
};

class DatasetReaderFast : public DatasetReaderBase
{
private:
    uint8_t* file_raw_data_;
    size_t total_num_bytes_;
    size_t num_frames_;
    size_t img_width_;
    size_t img_height_;

    std::vector<PointCollection> point_collections_;

    std::vector<ImageRGBConstView<uint8>> images_front_;
    std::vector<ImageRGBConstView<uint8>> images_left_;
    std::vector<ImageRGBConstView<uint8>> images_right_;

    void readFiles();

public:
    DatasetReaderFast() = delete;
    DatasetReaderFast(const std::string& dataset_file);

    PointCollection getPointCollection(const size_t idx) override;
    ImageRGBConstView<uint8> getLeftImage(const size_t idx) override;
    ImageRGBConstView<uint8> getRightImage(const size_t idx) override;
    ImageRGBConstView<uint8> getFrontImage(const size_t idx) override;
    size_t numFrames() const override;
};

class DatasetReader : public DatasetReaderBase
{
private:
    std::vector<PointCollection> point_collections_;
    std::vector<std::vector<unsigned char>> lidar_raw_data_;

    std::vector<ImageRGB<uint8>> images_front_;
    std::vector<ImageRGB<uint8>> images_left_;
    std::vector<ImageRGB<uint8>> images_right_;

    static constexpr int kMaxReadFrame = 5;

    void readLidarFile(const std::string& bin_path);

    void readCamera(const std::string& folder_path, std::vector<ImageRGB<uint8_t>>& destination);
    void readCameraFile(const std::string& bin_path, std::vector<ImageRGB<uint8_t>>& destination);

public:
    DatasetReader() = delete;
    DatasetReader(const std::string& dataset_root_path);

    PointCollection getPointCollection(const size_t idx) override
    {
        return point_collections_[idx];
    }

    ImageRGBConstView<uint8> getLeftImage(const size_t idx) override
    {
        return images_left_[idx].constView();
    }

    ImageRGBConstView<uint8> getRightImage(const size_t idx) override
    {
        return images_right_[idx].constView();
    }

    ImageRGBConstView<uint8> getFrontImage(const size_t idx) override
    {
        return images_front_[idx].constView();
    }

    size_t numFrames() const override
    {
        return point_collections_.size();
    }

    size_t numLidarFiles() const
    {
        return point_collections_.size();
    }

    size_t numImgFiles() const
    {
        return images_front_.size();
    }

    void saveToBigBlob(const std::string& output_folder_path) const;
};

void testBasic();
void testScroll();

}  // namespace ad_dataset

#endif  // DEMOS_TESTS_AD_DATASET_H_
