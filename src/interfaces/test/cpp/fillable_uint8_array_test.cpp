#include <gtest/gtest.h>
#include <cstring>
#include <memory>
#include <limits>
#include <vector>
#include <stdexcept>

#include "duoplot/fillable_uint8_array.h"

class UInt8ArrayViewTest : public ::testing::Test
{
protected:
    void SetUp() override {}
    void TearDown() override {}
};

class FillableUInt8ArrayTest : public ::testing::Test
{
protected:
    void SetUp() override {}
    void TearDown() override {}
};

// UInt8ArrayView Tests

TEST_F(UInt8ArrayViewTest, ConstructorAndBasicAccess)
{
    uint8_t test_data[] = {1, 2, 3, 4, 5};
    const size_t test_size = sizeof(test_data);
    
    UInt8ArrayView view(test_data, test_size);
    
    EXPECT_EQ(view.data(), test_data);
    EXPECT_EQ(view.size(), test_size);
    
    // Verify data can be read through the view
    for (size_t i = 0; i < test_size; ++i)
    {
        EXPECT_EQ(view.data()[i], test_data[i]);
    }
}

TEST_F(UInt8ArrayViewTest, EmptyArrayView)
{
    uint8_t* null_data = nullptr;
    UInt8ArrayView view(null_data, 0);
    
    EXPECT_EQ(view.data(), nullptr);
    EXPECT_EQ(view.size(), 0U);
}

TEST_F(UInt8ArrayViewTest, LargeArrayView)
{
    const size_t large_size = 10000;
    std::vector<uint8_t> large_data(large_size);
    
    // Fill with test pattern
    for (size_t i = 0; i < large_size; ++i)
    {
        large_data[i] = static_cast<uint8_t>(i % 256);
    }
    
    UInt8ArrayView view(large_data.data(), large_size);
    
    EXPECT_EQ(view.data(), large_data.data());
    EXPECT_EQ(view.size(), large_size);
    
    // Verify a few random elements
    EXPECT_EQ(view.data()[0], 0);
    EXPECT_EQ(view.data()[255], 255);
    EXPECT_EQ(view.data()[256], 0);
    EXPECT_EQ(view.data()[large_size - 1], static_cast<uint8_t>((large_size - 1) % 256));
}

// FillableUInt8Array Tests

TEST_F(FillableUInt8ArrayTest, BasicConstruction)
{
    const size_t test_size = 100;
    FillableUInt8Array array(test_size);
    
    EXPECT_NE(array.data(), nullptr);
    EXPECT_EQ(array.size(), test_size);
}

TEST_F(FillableUInt8ArrayTest, ZeroSizeThrowsException)
{
    EXPECT_THROW(FillableUInt8Array(0), std::runtime_error);
}

TEST_F(FillableUInt8ArrayTest, FillWithStaticTypeIntegers)
{
    FillableUInt8Array array(100);
    
    // Test filling with different integer types
    int8_t val_int8 = -42;
    uint8_t val_uint8 = 255;
    int16_t val_int16 = -1000;
    uint16_t val_uint16 = 65535;
    int32_t val_int32 = -100000;
    uint32_t val_uint32 = 4294967295U;
    int64_t val_int64 = -1000000000LL;
    uint64_t val_uint64 = 18446744073709551615ULL;
    
    array.fillWithStaticType(val_int8);
    array.fillWithStaticType(val_uint8);
    array.fillWithStaticType(val_int16);
    array.fillWithStaticType(val_uint16);
    array.fillWithStaticType(val_int32);
    array.fillWithStaticType(val_uint32);
    array.fillWithStaticType(val_int64);
    array.fillWithStaticType(val_uint64);
    
    // Verify the data was written correctly
    const uint8_t* data = array.data();
    size_t offset = 0;
    
    EXPECT_EQ(*reinterpret_cast<const int8_t*>(data + offset), val_int8);
    offset += sizeof(int8_t);
    
    EXPECT_EQ(*reinterpret_cast<const uint8_t*>(data + offset), val_uint8);
    offset += sizeof(uint8_t);
    
    EXPECT_EQ(*reinterpret_cast<const int16_t*>(data + offset), val_int16);
    offset += sizeof(int16_t);
    
    EXPECT_EQ(*reinterpret_cast<const uint16_t*>(data + offset), val_uint16);
    offset += sizeof(uint16_t);
    
    EXPECT_EQ(*reinterpret_cast<const int32_t*>(data + offset), val_int32);
    offset += sizeof(int32_t);
    
    EXPECT_EQ(*reinterpret_cast<const uint32_t*>(data + offset), val_uint32);
    offset += sizeof(uint32_t);
    
    EXPECT_EQ(*reinterpret_cast<const int64_t*>(data + offset), val_int64);
    offset += sizeof(int64_t);
    
    EXPECT_EQ(*reinterpret_cast<const uint64_t*>(data + offset), val_uint64);
}

TEST_F(FillableUInt8ArrayTest, FillWithStaticTypeFloatingPoint)
{
    FillableUInt8Array array(100);
    
    float val_float = 3.14159f;
    double val_double = 2.718281828459045;
    
    array.fillWithStaticType(val_float);
    array.fillWithStaticType(val_double);
    
    // Verify the data was written correctly
    const uint8_t* data = array.data();
    size_t offset = 0;
    
    EXPECT_FLOAT_EQ(*reinterpret_cast<const float*>(data + offset), val_float);
    offset += sizeof(float);
    
    EXPECT_DOUBLE_EQ(*reinterpret_cast<const double*>(data + offset), val_double);
}

TEST_F(FillableUInt8ArrayTest, FillWithStaticTypeSpecialFloats)
{
    FillableUInt8Array array(100);
    
    float infinity = std::numeric_limits<float>::infinity();
    float neg_infinity = -std::numeric_limits<float>::infinity();
    float quiet_nan = std::numeric_limits<float>::quiet_NaN();
    float zero = 0.0f;
    float neg_zero = -0.0f;
    
    array.fillWithStaticType(infinity);
    array.fillWithStaticType(neg_infinity);
    array.fillWithStaticType(quiet_nan);
    array.fillWithStaticType(zero);
    array.fillWithStaticType(neg_zero);
    
    // Verify the data was written correctly
    const uint8_t* data = array.data();
    size_t offset = 0;
    
    EXPECT_EQ(*reinterpret_cast<const float*>(data + offset), infinity);
    offset += sizeof(float);
    
    EXPECT_EQ(*reinterpret_cast<const float*>(data + offset), neg_infinity);
    offset += sizeof(float);
    
    EXPECT_TRUE(std::isnan(*reinterpret_cast<const float*>(data + offset)));
    offset += sizeof(float);
    
    EXPECT_FLOAT_EQ(*reinterpret_cast<const float*>(data + offset), zero);
    offset += sizeof(float);
    
    EXPECT_FLOAT_EQ(*reinterpret_cast<const float*>(data + offset), neg_zero);
}

TEST_F(FillableUInt8ArrayTest, FillWithDataFromPointer)
{
    FillableUInt8Array array(1000);
    
    // Test with uint8_t array
    uint8_t test_bytes[] = {10, 20, 30, 40, 50};
    array.fillWithDataFromPointer(test_bytes, sizeof(test_bytes));
    
    for (size_t i = 0; i < sizeof(test_bytes); ++i)
    {
        EXPECT_EQ(array.data()[i], test_bytes[i]);
    }
    
    // Test with int32_t array
    int32_t test_ints[] = {-100, 200, -300, 400, -500};
    const size_t num_ints = sizeof(test_ints) / sizeof(int32_t);
    array.fillWithDataFromPointer(test_ints, num_ints);
    
    // Verify int32_t data
    const uint8_t* data = array.data();
    size_t offset = sizeof(test_bytes);
    
    for (size_t i = 0; i < num_ints; ++i)
    {
        EXPECT_EQ(*reinterpret_cast<const int32_t*>(data + offset), test_ints[i]);
        offset += sizeof(int32_t);
    }
    
    // Test with double array
    double test_doubles[] = {1.1, 2.2, 3.3};
    const size_t num_doubles = sizeof(test_doubles) / sizeof(double);
    array.fillWithDataFromPointer(test_doubles, num_doubles);
    
    // Verify double data
    for (size_t i = 0; i < num_doubles; ++i)
    {
        EXPECT_DOUBLE_EQ(*reinterpret_cast<const double*>(data + offset), test_doubles[i]);
        offset += sizeof(double);
    }
}

TEST_F(FillableUInt8ArrayTest, FillWithDataFromPointerZeroElements)
{
    FillableUInt8Array array(100);
    
    int32_t test_data[] = {1, 2, 3};
    array.fillWithDataFromPointer(test_data, 0);
    
    // Array should still be valid but no data should have been copied
    EXPECT_NE(array.data(), nullptr);
    EXPECT_EQ(array.size(), 100U);
}

TEST_F(FillableUInt8ArrayTest, ViewCreation)
{
    const size_t test_size = 50;
    FillableUInt8Array array(test_size);
    
    // Fill with some test data
    for (int i = 0; i < 10; ++i)
    {
        array.fillWithStaticType(static_cast<uint32_t>(i * 42));
    }
    
    UInt8ArrayView view = array.view();
    
    EXPECT_EQ(view.data(), array.data());
    EXPECT_EQ(view.size(), array.size());
}

TEST_F(FillableUInt8ArrayTest, MixedDataTypes)
{
    FillableUInt8Array array(200);
    
    // Fill with mixed data types to simulate real usage
    uint8_t header = 0xAA;
    uint32_t size = 1024;
    float version = 1.5f;
    double timestamp = 1234567890.123456;
    
    uint8_t payload[] = {0x01, 0x02, 0x03, 0x04, 0x05};
    
    array.fillWithStaticType(header);
    array.fillWithStaticType(size);
    array.fillWithStaticType(version);
    array.fillWithStaticType(timestamp);
    array.fillWithDataFromPointer(payload, sizeof(payload));
    
    // Verify all data is correctly stored
    const uint8_t* data = array.data();
    size_t offset = 0;
    
    EXPECT_EQ(*reinterpret_cast<const uint8_t*>(data + offset), header);
    offset += sizeof(uint8_t);
    
    EXPECT_EQ(*reinterpret_cast<const uint32_t*>(data + offset), size);
    offset += sizeof(uint32_t);
    
    EXPECT_FLOAT_EQ(*reinterpret_cast<const float*>(data + offset), version);
    offset += sizeof(float);
    
    EXPECT_DOUBLE_EQ(*reinterpret_cast<const double*>(data + offset), timestamp);
    offset += sizeof(double);
    
    for (size_t i = 0; i < sizeof(payload); ++i)
    {
        EXPECT_EQ(data[offset + i], payload[i]);
    }
}

TEST_F(FillableUInt8ArrayTest, ConsecutiveFillOperations)
{
    const size_t iterations = 100;
    FillableUInt8Array array(iterations * sizeof(uint32_t));
    
    // Fill array with consecutive integers
    for (uint32_t i = 0; i < iterations; ++i)
    {
        array.fillWithStaticType(i);
    }
    
    // Verify all data is correctly stored
    const uint8_t* data = array.data();
    for (uint32_t i = 0; i < iterations; ++i)
    {
        size_t offset = i * sizeof(uint32_t);
        EXPECT_EQ(*reinterpret_cast<const uint32_t*>(data + offset), i);
    }
}

TEST_F(FillableUInt8ArrayTest, LargeDataPointerFill)
{
    const size_t large_array_size = 10000;
    const size_t buffer_size = large_array_size * sizeof(uint16_t);
    
    FillableUInt8Array array(buffer_size);
    
    // Create large test data
    std::vector<uint16_t> large_data(large_array_size);
    for (size_t i = 0; i < large_array_size; ++i)
    {
        large_data[i] = static_cast<uint16_t>(i % 65536);
    }
    
    array.fillWithDataFromPointer(large_data.data(), large_array_size);
    
    // Verify data integrity
    const uint8_t* data = array.data();
    for (size_t i = 0; i < large_array_size; ++i)
    {
        size_t offset = i * sizeof(uint16_t);
        uint16_t retrieved = *reinterpret_cast<const uint16_t*>(data + offset);
        EXPECT_EQ(retrieved, large_data[i]);
    }
}

// Edge case and error condition tests

TEST_F(FillableUInt8ArrayTest, DestructorCleanup)
{
    uint8_t* data_ptr = nullptr;
    
    {
        FillableUInt8Array array(100);
        data_ptr = array.data();
        EXPECT_NE(data_ptr, nullptr);
        
        // Fill with some data
        array.fillWithStaticType(static_cast<uint32_t>(12345));
    }
    // Array is now destructed - we can't safely access data_ptr anymore
    // This test mainly ensures no crashes occur during destruction
}

TEST_F(FillableUInt8ArrayTest, SingleByteArray)
{
    FillableUInt8Array array(1);
    
    uint8_t test_byte = 42;
    array.fillWithStaticType(test_byte);
    
    EXPECT_EQ(array.data()[0], test_byte);
}

TEST_F(FillableUInt8ArrayTest, ViewFromFilledArray)
{
    FillableUInt8Array array(20);
    
    uint64_t test_value = 0x123456789ABCDEF0ULL;
    array.fillWithStaticType(test_value);
    
    UInt8ArrayView view = array.view();
    
    // Verify view provides access to the filled data
    uint64_t retrieved = *reinterpret_cast<const uint64_t*>(view.data());
    EXPECT_EQ(retrieved, test_value);
}

TEST_F(FillableUInt8ArrayTest, StructDataFill)
{
    struct TestStruct
    {
        uint32_t id;
        float value;
        uint8_t flags[4];
    };
    
    FillableUInt8Array array(sizeof(TestStruct) * 5);
    
    TestStruct test_struct = {12345, 6.78f, {0xAA, 0xBB, 0xCC, 0xDD}};
    array.fillWithStaticType(test_struct);
    
    // Verify struct data
    const TestStruct* retrieved = reinterpret_cast<const TestStruct*>(array.data());
    EXPECT_EQ(retrieved->id, test_struct.id);
    EXPECT_FLOAT_EQ(retrieved->value, test_struct.value);
    
    for (int i = 0; i < 4; ++i)
    {
        EXPECT_EQ(retrieved->flags[i], test_struct.flags[i]);
    }
}