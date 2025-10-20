#include <gtest/gtest.h>
#include <cstring>
#include <memory>
#include <limits>
#include <vector>
#include <stdexcept>

#include "duoplot/property_set.h"
#include "duoplot/plot_properties.h"
#include "duoplot/enumerations.h"
#include "duoplot/item_id.h"
#include "duoplot/encode_decode_functions.h"

using namespace duoplot;
using namespace duoplot::internal;
using namespace duoplot::properties;

// PropertySet tests
class PropertySetTest : public ::testing::Test
{
protected:
    void SetUp() override {}
    void TearDown() override {}
    
    // Helper function to verify buffer contents
    void VerifyBufferStructure(const PropertySet& prop_set, const std::vector<uint8_t>& expected_buffer)
    {
        size_t total_size = prop_set.getTotalSize();
        std::vector<uint8_t> actual_buffer(total_size);
        prop_set.fillBuffer(actual_buffer.data());
        
        ASSERT_EQ(actual_buffer.size(), expected_buffer.size());
        for (size_t i = 0; i < expected_buffer.size(); ++i)
        {
            EXPECT_EQ(actual_buffer[i], expected_buffer[i]) << "Mismatch at index " << i;
        }
    }
};

TEST_F(PropertySetTest, DefaultConstruction)
{
    PropertySet prop_set;
    
    // Default constructor should create empty set with UNKNOWN ID
    EXPECT_EQ(prop_set.getTotalSize(), 1U + sizeof(ItemId) + 0U); // num_props + id + props
    
    // Verify buffer contents
    size_t total_size = prop_set.getTotalSize();
    std::vector<uint8_t> buffer(total_size);
    prop_set.fillBuffer(buffer.data());
    
    EXPECT_EQ(buffer[0], 0U); // No properties
    
    // Check ItemId (should be UNKNOWN)
    ItemId stored_id;
    std::memcpy(&stored_id, buffer.data() + 1, sizeof(ItemId));
    EXPECT_EQ(stored_id, ItemId::UNKNOWN);
}

TEST_F(PropertySetTest, ConstructionWithIdOnly)
{
    PropertySet prop_set(ItemId::ID5);
    
    EXPECT_EQ(prop_set.getTotalSize(), 1U + sizeof(ItemId) + 0U);
    
    // Verify buffer contents
    size_t total_size = prop_set.getTotalSize();
    std::vector<uint8_t> buffer(total_size);
    prop_set.fillBuffer(buffer.data());
    
    EXPECT_EQ(buffer[0], 0U); // No properties
    
    // Check ItemId
    ItemId stored_id;
    std::memcpy(&stored_id, buffer.data() + 1, sizeof(ItemId));
    EXPECT_EQ(stored_id, ItemId::ID5);
}

TEST_F(PropertySetTest, ConstructionWithSingleProperty)
{
    Alpha alpha(0.75f);
    PropertySet prop_set(alpha);
    
    // Get actual size from the object
    size_t actual_size = prop_set.getTotalSize();
    EXPECT_GT(actual_size, 1U + sizeof(ItemId)); // Should be at least count + id
    
    // Verify buffer structure
    std::vector<uint8_t> buffer(actual_size);
    prop_set.fillBuffer(buffer.data());
    
    EXPECT_EQ(buffer[0], 1U); // One property
    
    // Check ItemId (should be UNKNOWN since not specified)
    ItemId stored_id;
    std::memcpy(&stored_id, buffer.data() + 1, sizeof(ItemId));
    EXPECT_EQ(stored_id, ItemId::UNKNOWN);
    
    // Check that property size is recorded (don't assume specific size)
    size_t prop_size_offset = 1U + sizeof(ItemId);
    EXPECT_GT(buffer[prop_size_offset], 0U); // Should have non-zero size
}

TEST_F(PropertySetTest, ConstructionWithIdAndSingleProperty)
{
    LineWidth line_width(3U);
    PropertySet prop_set(ItemId::ID10, line_width);
    
    // Get actual size
    size_t actual_size = prop_set.getTotalSize();
    EXPECT_GT(actual_size, 1U + sizeof(ItemId)); // Should be at least count + id
    
    // Verify buffer structure
    std::vector<uint8_t> buffer(actual_size);
    prop_set.fillBuffer(buffer.data());
    
    EXPECT_EQ(buffer[0], 1U); // One property
    
    // Check ItemId
    ItemId stored_id;
    std::memcpy(&stored_id, buffer.data() + 1, sizeof(ItemId));
    EXPECT_EQ(stored_id, ItemId::ID10);
    
    // Check that property size is recorded
    size_t prop_size_offset = 1U + sizeof(ItemId);
    EXPECT_GT(buffer[prop_size_offset], 0U); // Should have non-zero size
}

TEST_F(PropertySetTest, ConstructionWithMultipleProperties)
{
    Alpha alpha(0.5f);
    LineWidth line_width(5U);
    ZOffset z_offset(1.5f);
    
    PropertySet prop_set(alpha, line_width, z_offset);
    
    // Get actual size
    size_t actual_size = prop_set.getTotalSize();
    EXPECT_GT(actual_size, 1U + sizeof(ItemId) + 3U); // Should be at least count + id + 3 size bytes
    
    // Verify buffer structure
    std::vector<uint8_t> buffer(actual_size);
    prop_set.fillBuffer(buffer.data());
    
    EXPECT_EQ(buffer[0], 3U); // Three properties
    
    // Check ItemId (should be UNKNOWN)
    ItemId stored_id;
    std::memcpy(&stored_id, buffer.data() + 1, sizeof(ItemId));
    EXPECT_EQ(stored_id, ItemId::UNKNOWN);
    
    // Check that all property sizes are recorded and non-zero
    size_t offset = 1U + sizeof(ItemId);
    for (int i = 0; i < 3; ++i)
    {
        EXPECT_GT(buffer[offset], 0U) << "Property " << i << " should have non-zero size";
        uint8_t prop_size = buffer[offset];
        offset += 1U + prop_size;
    }
    
    // Should have consumed the entire buffer
    EXPECT_EQ(offset, actual_size);
}

TEST_F(PropertySetTest, ConstructionWithIdAndMultipleProperties)
{
    Alpha alpha(0.8f);
    LineWidth line_width(2U);
    PointSize point_size(8U);
    BufferSize buffer_size(1024U);
    
    PropertySet prop_set(ItemId::ID7, alpha, line_width, point_size, buffer_size);
    
    size_t expected_size = 1U + sizeof(ItemId) + 4U + 
                          sizeof(Alpha) + sizeof(LineWidth) + sizeof(PointSize) + sizeof(BufferSize);
    EXPECT_EQ(prop_set.getTotalSize(), expected_size);
    
    // Verify buffer structure
    size_t total_size = prop_set.getTotalSize();
    std::vector<uint8_t> buffer(total_size);
    prop_set.fillBuffer(buffer.data());
    
    EXPECT_EQ(buffer[0], 4U); // Four properties
    
    // Check ItemId
    ItemId stored_id;
    std::memcpy(&stored_id, buffer.data() + 1, sizeof(ItemId));
    EXPECT_EQ(stored_id, ItemId::ID7);
    
    // Verify all property sizes are recorded
    size_t offset = 1U + sizeof(ItemId);
    EXPECT_EQ(buffer[offset], sizeof(Alpha));
    offset += 1U + sizeof(Alpha);
    
    EXPECT_EQ(buffer[offset], sizeof(LineWidth));
    offset += 1U + sizeof(LineWidth);
    
    EXPECT_EQ(buffer[offset], sizeof(PointSize));
    offset += 1U + sizeof(PointSize);
    
    EXPECT_EQ(buffer[offset], sizeof(BufferSize));
}

TEST_F(PropertySetTest, VariedPropertyTypes)
{
    // Test with different types of properties
    Alpha alpha(0.6f);
    EdgeColor edge_color(255, 0, 128);
    Silhouette silhouette(64, 192, 255, 0.3f);
    
    PropertySet prop_set(ItemId::ID3, alpha, edge_color, silhouette);
    
    size_t expected_size = 1U + sizeof(ItemId) + 3U + 
                          sizeof(Alpha) + sizeof(EdgeColor) + sizeof(Silhouette);
    EXPECT_EQ(prop_set.getTotalSize(), expected_size);
    
    // Verify buffer can be filled without errors
    size_t total_size = prop_set.getTotalSize();
    std::vector<uint8_t> buffer(total_size);
    prop_set.fillBuffer(buffer.data());
    
    EXPECT_EQ(buffer[0], 3U); // Three properties
    
    // Check ItemId
    ItemId stored_id;
    std::memcpy(&stored_id, buffer.data() + 1, sizeof(ItemId));
    EXPECT_EQ(stored_id, ItemId::ID3);
}

TEST_F(PropertySetTest, LargePropertySet)
{
    // Test with many properties
    Alpha alpha(0.9f);
    LineWidth line_width(10U);
    ZOffset z_offset(2.5f);
    PointSize point_size(15U);
    BufferSize buffer_size(2048U);
    EdgeColor edge_color(128, 64, 32);
    FaceColor face_color(200, 150, 100);
    
    PropertySet prop_set(ItemId::ID15, alpha, line_width, z_offset, point_size, 
                        buffer_size, edge_color, face_color);
    
    size_t expected_size = 1U + sizeof(ItemId) + 7U + 
                          sizeof(Alpha) + sizeof(LineWidth) + sizeof(ZOffset) + 
                          sizeof(PointSize) + sizeof(BufferSize) + sizeof(EdgeColor) + 
                          sizeof(FaceColor);
    EXPECT_EQ(prop_set.getTotalSize(), expected_size);
    
    // Verify buffer structure
    size_t total_size = prop_set.getTotalSize();
    std::vector<uint8_t> buffer(total_size);
    prop_set.fillBuffer(buffer.data());
    
    EXPECT_EQ(buffer[0], 7U); // Seven properties
    
    // Check ItemId
    ItemId stored_id;
    std::memcpy(&stored_id, buffer.data() + 1, sizeof(ItemId));
    EXPECT_EQ(stored_id, ItemId::ID15);
}

TEST_F(PropertySetTest, BufferContentVerification)
{
    // Test basic buffer structure verification
    Alpha alpha(0.25f);
    LineWidth line_width(7U);
    
    PropertySet prop_set(ItemId::ID2, alpha, line_width);
    
    size_t total_size = prop_set.getTotalSize();
    std::vector<uint8_t> buffer(total_size);
    prop_set.fillBuffer(buffer.data());
    
    // Verify basic structure
    size_t offset = 0;
    
    // Number of properties
    EXPECT_EQ(buffer[offset], 2U);
    offset += 1;
    
    // ItemId
    ItemId stored_id;
    std::memcpy(&stored_id, buffer.data() + offset, sizeof(ItemId));
    EXPECT_EQ(stored_id, ItemId::ID2);
    offset += sizeof(ItemId);
    
    // Verify property sizes are recorded and data follows
    for (int i = 0; i < 2; ++i)
    {
        EXPECT_GT(buffer[offset], 0U) << "Property " << i << " should have non-zero size";
        uint8_t prop_size = buffer[offset];
        offset += 1U + prop_size;
    }
    
    // Should have consumed entire buffer
    EXPECT_EQ(offset, total_size);
}

TEST_F(PropertySetTest, EmptyPropertySetSerialization)
{
    PropertySet empty_set;
    
    // Verify total size calculation
    size_t expected_size = 1U + sizeof(ItemId) + 0U; // count + id + no properties
    EXPECT_EQ(empty_set.getTotalSize(), expected_size);
    
    // Verify buffer filling
    std::vector<uint8_t> buffer(expected_size);
    empty_set.fillBuffer(buffer.data());
    
    EXPECT_EQ(buffer[0], 0U); // Zero properties
    
    ItemId stored_id;
    std::memcpy(&stored_id, buffer.data() + 1, sizeof(ItemId));
    EXPECT_EQ(stored_id, ItemId::UNKNOWN);
}

TEST_F(PropertySetTest, SinglePropertySerialization)
{
    ZOffset z_offset(-1.5f);
    PropertySet prop_set(ItemId::ID1, z_offset);
    
    size_t total_size = prop_set.getTotalSize();
    std::vector<uint8_t> buffer(total_size);
    prop_set.fillBuffer(buffer.data());
    
    // Verify complete serialization
    size_t offset = 0;
    
    EXPECT_EQ(buffer[offset], 1U); // One property
    offset += 1;
    
    ItemId stored_id;
    std::memcpy(&stored_id, buffer.data() + offset, sizeof(ItemId));
    EXPECT_EQ(stored_id, ItemId::ID1);
    offset += sizeof(ItemId);
    
    EXPECT_EQ(buffer[offset], sizeof(ZOffset));
    offset += 1;
    
    ZOffset stored_z_offset;
    std::memcpy(&stored_z_offset, buffer.data() + offset, sizeof(ZOffset));
    EXPECT_FLOAT_EQ(stored_z_offset.data, -1.5f);
    EXPECT_EQ(stored_z_offset.getPropertyType(), PropertyType::Z_OFFSET);
}

TEST_F(PropertySetTest, ComplexPropertiesSerialization)
{
    // Test with complex properties that have more data
    Label label("test_property_set");
    Transform transform;
    
    // Initialize transform with some test data
    for (size_t r = 0; r < 3; ++r)
    {
        for (size_t c = 0; c < 3; ++c)
        {
            transform.scale(r, c) = r * 3 + c + 1;
            transform.rotation(r, c) = (r + c) * 0.5;
        }
    }
    transform.translation = Vec3<double>(1.0, 2.0, 3.0);
    
    PropertySet prop_set(ItemId::ID8, label, transform);
    
    size_t expected_size = 1U + sizeof(ItemId) + 2U + sizeof(Label) + sizeof(Transform);
    EXPECT_EQ(prop_set.getTotalSize(), expected_size);
    
    // Verify buffer can be filled
    size_t total_size = prop_set.getTotalSize();
    std::vector<uint8_t> buffer(total_size);
    prop_set.fillBuffer(buffer.data());
    
    EXPECT_EQ(buffer[0], 2U); // Two properties
    
    ItemId stored_id;
    std::memcpy(&stored_id, buffer.data() + 1, sizeof(ItemId));
    EXPECT_EQ(stored_id, ItemId::ID8);
}

TEST_F(PropertySetTest, ColorPropertiesSerialization)
{
    // Test with various color-related properties
    EdgeColor edge_color(EdgeColorT::RED);
    FaceColor face_color(FaceColorT::BLUE);
    Silhouette silhouette(SilhouetteT::GREEN);
    
    PropertySet prop_set(ItemId::ID12, edge_color, face_color, silhouette);
    
    size_t total_size = prop_set.getTotalSize();
    std::vector<uint8_t> buffer(total_size);
    prop_set.fillBuffer(buffer.data());
    
    EXPECT_EQ(buffer[0], 3U); // Three properties
    
    // Verify ItemId
    ItemId stored_id;
    std::memcpy(&stored_id, buffer.data() + 1, sizeof(ItemId));
    EXPECT_EQ(stored_id, ItemId::ID12);
    
    // Verify property sizes are recorded correctly
    size_t offset = 1U + sizeof(ItemId);
    EXPECT_EQ(buffer[offset], sizeof(EdgeColor));
    offset += 1U + sizeof(EdgeColor);
    
    EXPECT_EQ(buffer[offset], sizeof(FaceColor));
    offset += 1U + sizeof(FaceColor);
    
    EXPECT_EQ(buffer[offset], sizeof(Silhouette));
}

TEST_F(PropertySetTest, MixedConstructorPatterns)
{
    // Test different constructor patterns work consistently
    
    // Pattern 1: No ID, single property
    Alpha alpha1(0.1f);
    PropertySet set1(alpha1);
    
    // Pattern 2: With ID, single property
    Alpha alpha2(0.2f);
    PropertySet set2(ItemId::ID5, alpha2);
    
    // Pattern 3: No ID, multiple properties
    Alpha alpha3(0.3f);
    LineWidth lw3(3U);
    PropertySet set3(alpha3, lw3);
    
    // Pattern 4: With ID, multiple properties
    Alpha alpha4(0.4f);
    LineWidth lw4(4U);
    PropertySet set4(ItemId::ID10, alpha4, lw4);
    
    // All should serialize without errors
    std::vector<uint8_t> buffer1(set1.getTotalSize());
    std::vector<uint8_t> buffer2(set2.getTotalSize());
    std::vector<uint8_t> buffer3(set3.getTotalSize());
    std::vector<uint8_t> buffer4(set4.getTotalSize());
    
    set1.fillBuffer(buffer1.data());
    set2.fillBuffer(buffer2.data());
    set3.fillBuffer(buffer3.data());
    set4.fillBuffer(buffer4.data());
    
    // Verify property counts
    EXPECT_EQ(buffer1[0], 1U);
    EXPECT_EQ(buffer2[0], 1U);
    EXPECT_EQ(buffer3[0], 2U);
    EXPECT_EQ(buffer4[0], 2U);
    
    // Verify IDs
    ItemId id1, id2, id3, id4;
    std::memcpy(&id1, buffer1.data() + 1, sizeof(ItemId));
    std::memcpy(&id2, buffer2.data() + 1, sizeof(ItemId));
    std::memcpy(&id3, buffer3.data() + 1, sizeof(ItemId));
    std::memcpy(&id4, buffer4.data() + 1, sizeof(ItemId));
    
    EXPECT_EQ(id1, ItemId::UNKNOWN);
    EXPECT_EQ(id2, ItemId::ID5);
    EXPECT_EQ(id3, ItemId::UNKNOWN);
    EXPECT_EQ(id4, ItemId::ID10);
}

TEST_F(PropertySetTest, BoundaryValueProperties)
{
    // Test with boundary values for different property types
    Alpha min_alpha(0.0f);
    Alpha max_alpha(1.0f);
    LineWidth min_lw(0U);
    LineWidth max_lw(255U);
    BufferSize min_bs(0U);
    BufferSize max_bs(65535U);
    
    PropertySet prop_set(ItemId::ID20, min_alpha, max_alpha, min_lw, max_lw, min_bs, max_bs);
    
    size_t total_size = prop_set.getTotalSize();
    std::vector<uint8_t> buffer(total_size);
    prop_set.fillBuffer(buffer.data());
    
    EXPECT_EQ(buffer[0], 6U); // Six properties
    
    ItemId stored_id;
    std::memcpy(&stored_id, buffer.data() + 1, sizeof(ItemId));
    EXPECT_EQ(stored_id, ItemId::ID20);
    
    // Verify all property sizes are recorded
    size_t offset = 1U + sizeof(ItemId);
    for (int i = 0; i < 6; ++i)
    {
        EXPECT_GT(buffer[offset], 0U) << "Property " << i << " should have non-zero size";
        uint8_t prop_size = buffer[offset];
        offset += 1U + prop_size;
    }
    
    EXPECT_EQ(offset, total_size);
}