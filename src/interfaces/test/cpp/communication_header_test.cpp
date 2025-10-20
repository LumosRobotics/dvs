#include <gtest/gtest.h>
#include <cstring>
#include <memory>
#include <limits>
#include <vector>
#include <stdexcept>

#include "duoplot/communication_header.h"
#include "duoplot/enumerations.h"
#include "duoplot/plot_properties.h"

using namespace duoplot;
using namespace duoplot::internal;

// Helper functions tests
class CommunicationHeaderUtilityTest : public ::testing::Test
{
protected:
    void SetUp() override {}
    void TearDown() override {}
};

// Test utility template functions
TEST_F(CommunicationHeaderUtilityTest, TypeConversionFunctions)
{
    // Test toUInt8
    EXPECT_EQ(toUInt8(127), 127);
    EXPECT_EQ(toUInt8(255), 255);
    EXPECT_EQ(toUInt8(-1), 255); // Should wrap around for negative values

    // Test toUInt32
    EXPECT_EQ(toUInt32(12345), 12345U);
    EXPECT_EQ(toUInt32(-1), 4294967295U); // Should wrap around

    // Test toInt32
    EXPECT_EQ(toInt32(12345U), 12345);
    EXPECT_EQ(toInt32(-12345), -12345);
}

TEST_F(CommunicationHeaderUtilityTest, DataTypeToNumBytes)
{
    EXPECT_EQ(dataTypeToNumBytes(DataType::FLOAT), sizeof(float));
    EXPECT_EQ(dataTypeToNumBytes(DataType::DOUBLE), sizeof(double));
    EXPECT_EQ(dataTypeToNumBytes(DataType::INT8), sizeof(int8_t));
    EXPECT_EQ(dataTypeToNumBytes(DataType::INT16), sizeof(int16_t));
    EXPECT_EQ(dataTypeToNumBytes(DataType::INT32), sizeof(int32_t));
    EXPECT_EQ(dataTypeToNumBytes(DataType::INT64), sizeof(int64_t));
    EXPECT_EQ(dataTypeToNumBytes(DataType::UINT8), sizeof(uint8_t));
    EXPECT_EQ(dataTypeToNumBytes(DataType::UINT16), sizeof(uint16_t));
    EXPECT_EQ(dataTypeToNumBytes(DataType::UINT32), sizeof(uint32_t));
    EXPECT_EQ(dataTypeToNumBytes(DataType::UINT64), sizeof(uint64_t));
    
    // Test error cases
    EXPECT_THROW(dataTypeToNumBytes(DataType::UNKNOWN), std::runtime_error);
}

TEST_F(CommunicationHeaderUtilityTest, TypeToDataTypeEnum)
{
    EXPECT_EQ(typeToDataTypeEnum<float>(), DataType::FLOAT);
    EXPECT_EQ(typeToDataTypeEnum<double>(), DataType::DOUBLE);
    EXPECT_EQ(typeToDataTypeEnum<int8_t>(), DataType::INT8);
    EXPECT_EQ(typeToDataTypeEnum<int16_t>(), DataType::INT16);
    EXPECT_EQ(typeToDataTypeEnum<int32_t>(), DataType::INT32);
    EXPECT_EQ(typeToDataTypeEnum<int64_t>(), DataType::INT64);
    EXPECT_EQ(typeToDataTypeEnum<uint8_t>(), DataType::UINT8);
    EXPECT_EQ(typeToDataTypeEnum<uint16_t>(), DataType::UINT16);
    EXPECT_EQ(typeToDataTypeEnum<uint32_t>(), DataType::UINT32);
    EXPECT_EQ(typeToDataTypeEnum<uint64_t>(), DataType::UINT64);
}

// Lookup table tests
class CommunicationHeaderObjectLookupTableTest : public ::testing::Test
{
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(CommunicationHeaderObjectLookupTableTest, Initialization)
{
    CommunicationHeaderObjectLookupTable lut;
    
    // All entries should be initialized to 255
    for (size_t i = 0; i < CommunicationHeaderObjectLookupTable::kTableSize; ++i)
    {
        EXPECT_EQ(lut.data[i], 255);
    }
}

TEST_F(CommunicationHeaderObjectLookupTableTest, AppendObjectIndex)
{
    CommunicationHeaderObjectLookupTable lut;
    
    lut.appendObjectIndex(CommunicationHeaderObjectType::FUNCTION, 10);
    lut.appendObjectIndex(CommunicationHeaderObjectType::DATA_TYPE, 20);
    
    EXPECT_EQ(lut.data[static_cast<uint8_t>(CommunicationHeaderObjectType::FUNCTION)], 10);
    EXPECT_EQ(lut.data[static_cast<uint8_t>(CommunicationHeaderObjectType::DATA_TYPE)], 20);
}

class PropertyLookupTableTest : public ::testing::Test
{
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(PropertyLookupTableTest, InitializationAndClear)
{
    PropertyLookupTable lut;
    
    // All entries should be initialized to 255
    for (size_t i = 0; i < PropertyLookupTable::kTableSize; ++i)
    {
        EXPECT_EQ(lut.data[i], 255);
    }
    
    // Test append and clear
    lut.appendPropertyIndex(PropertyType::ALPHA, 5);
    EXPECT_EQ(lut.data[static_cast<uint8_t>(PropertyType::ALPHA)], 5);
    
    lut.clear();
    EXPECT_EQ(lut.data[static_cast<uint8_t>(PropertyType::ALPHA)], 255);
}

// Array class tests
class CommunicationHeaderArrayTest : public ::testing::Test
{
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(CommunicationHeaderArrayTest, BasicFunctionality)
{
    CommunicationHeader::Array<10> array;
    
    EXPECT_EQ(array.usedSize(), 0U);
    
    CommunicationHeaderObject obj1(CommunicationHeaderObjectType::FUNCTION);
    CommunicationHeaderObject obj2(CommunicationHeaderObjectType::DATA_TYPE);
    
    array.pushBack(obj1);
    EXPECT_EQ(array.usedSize(), 1U);
    
    array.pushBack(obj2);
    EXPECT_EQ(array.usedSize(), 2U);
    
    // Test lastElement access
    CommunicationHeaderObject& last = array.lastElement();
    EXPECT_EQ(last.type, CommunicationHeaderObjectType::DATA_TYPE);
}

TEST_F(CommunicationHeaderArrayTest, BoundsChecking)
{
    CommunicationHeader::Array<2> small_array;
    
    CommunicationHeaderObject obj(CommunicationHeaderObjectType::FUNCTION);
    
    small_array.pushBack(obj);
    small_array.pushBack(obj);
    
    // Should throw when trying to add beyond capacity
    EXPECT_THROW(small_array.pushBack(obj), std::runtime_error);
}

TEST_F(CommunicationHeaderArrayTest, EmptyArrayAccess)
{
    CommunicationHeader::Array<10> array;
    
    // Should throw when trying to access last element of empty array
    EXPECT_THROW(array.lastElement(), std::runtime_error);
}

TEST_F(CommunicationHeaderArrayTest, ClearAndSetUsedSize)
{
    CommunicationHeader::Array<10> array;
    CommunicationHeaderObject obj(CommunicationHeaderObjectType::FUNCTION);
    
    array.pushBack(obj);
    array.pushBack(obj);
    EXPECT_EQ(array.usedSize(), 2U);
    
    array.clear();
    EXPECT_EQ(array.usedSize(), 0U);
    
    // Test setUsedSize
    array.setUsedSize(5);
    EXPECT_EQ(array.usedSize(), 5U);
    
    // Should throw if trying to set size larger than capacity
    EXPECT_THROW(array.setUsedSize(15), std::runtime_error);
}

// Main CommunicationHeader tests
class CommunicationHeaderTest : public ::testing::Test
{
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(CommunicationHeaderTest, DefaultConstruction)
{
    CommunicationHeader header;
    
    EXPECT_EQ(header.getFunction(), Function::UNKNOWN);
    EXPECT_EQ(header.getObjects().usedSize(), 0U);
    EXPECT_EQ(header.getPropertiesObjects().usedSize(), 0U);
    
    // Check that flags are initialized to 0
    auto flags = header.getFlags();
    for (size_t i = 0; i < flags.size(); ++i)
    {
        EXPECT_EQ(flags[i], 0U);
    }
}

TEST_F(CommunicationHeaderTest, ConstructionWithFunction)
{
    CommunicationHeader header(Function::PLOT2);
    
    EXPECT_EQ(header.getFunction(), Function::PLOT2);
    EXPECT_EQ(header.getObjects().usedSize(), 0U);
    EXPECT_EQ(header.getPropertiesObjects().usedSize(), 0U);
}

TEST_F(CommunicationHeaderTest, FunctionSetterGetter)
{
    CommunicationHeader header;
    
    header.setFunction(Function::PLOT3);
    EXPECT_EQ(header.getFunction(), Function::PLOT3);
}

TEST_F(CommunicationHeaderTest, BasicObjectAppending)
{
    CommunicationHeader header;
    
    uint32_t test_value = 12345;
    header.append(CommunicationHeaderObjectType::NUM_ELEMENTS, test_value);
    
    EXPECT_EQ(header.getObjects().usedSize(), 1U);
    EXPECT_TRUE(header.hasObjectWithType(CommunicationHeaderObjectType::NUM_ELEMENTS));
    
    auto retrieved_obj = header.get(CommunicationHeaderObjectType::NUM_ELEMENTS);
    EXPECT_EQ(retrieved_obj.type, CommunicationHeaderObjectType::NUM_ELEMENTS);
    EXPECT_EQ(retrieved_obj.as<uint32_t>(), test_value);
}

TEST_F(CommunicationHeaderTest, MultipleObjectAppending)
{
    CommunicationHeader header;
    
    uint32_t num_elements = 1000;
    float version = 2.5f;
    DataType data_type = DataType::FLOAT;
    
    header.append(CommunicationHeaderObjectType::NUM_ELEMENTS, num_elements);
    header.append(CommunicationHeaderObjectType::NUM_BYTES, version);
    header.append(CommunicationHeaderObjectType::DATA_TYPE, data_type);
    
    EXPECT_EQ(header.getObjects().usedSize(), 3U);
    
    EXPECT_TRUE(header.hasObjectWithType(CommunicationHeaderObjectType::NUM_ELEMENTS));
    EXPECT_TRUE(header.hasObjectWithType(CommunicationHeaderObjectType::NUM_BYTES));
    EXPECT_TRUE(header.hasObjectWithType(CommunicationHeaderObjectType::DATA_TYPE));
    
    EXPECT_EQ(header.get(CommunicationHeaderObjectType::NUM_ELEMENTS).as<uint32_t>(), num_elements);
    EXPECT_FLOAT_EQ(header.get(CommunicationHeaderObjectType::NUM_BYTES).as<float>(), version);
    EXPECT_EQ(header.get(CommunicationHeaderObjectType::DATA_TYPE).as<DataType>(), data_type);
}

TEST_F(CommunicationHeaderTest, ObjectNotPresent)
{
    CommunicationHeader header;
    
    EXPECT_FALSE(header.hasObjectWithType(CommunicationHeaderObjectType::FUNCTION));
    EXPECT_THROW(header.get(CommunicationHeaderObjectType::FUNCTION), std::runtime_error);
}

TEST_F(CommunicationHeaderTest, PropertyAppending)
{
    CommunicationHeader header;
    
    properties::Alpha alpha(0.75f);
    properties::LineWidth line_width(3U);
    
    // Use extend to add properties
    header.extend(alpha, line_width);
    
    EXPECT_EQ(header.getPropertiesObjects().usedSize(), 2U);
    
    // Check property lookup table
    const auto& prop_lut = header.getPropertyLookupTable();
    EXPECT_NE(prop_lut.data[static_cast<uint8_t>(PropertyType::ALPHA)], 255);
    EXPECT_NE(prop_lut.data[static_cast<uint8_t>(PropertyType::LINE_WIDTH)], 255);
}

TEST_F(CommunicationHeaderTest, PropertyFlags)
{
    CommunicationHeader header;
    
    header.extend(PropertyFlag::PERSISTENT, PropertyFlag::UPDATABLE);
    
    EXPECT_TRUE(header.hasPropertyFlag(PropertyFlag::PERSISTENT));
    EXPECT_TRUE(header.hasPropertyFlag(PropertyFlag::UPDATABLE));
    EXPECT_FALSE(header.hasPropertyFlag(PropertyFlag::APPENDABLE));
}

TEST_F(CommunicationHeaderTest, ItemIdHandling)
{
    CommunicationHeader header;
    
    ItemId test_id = ItemId::ID5;
    header.extend(test_id);
    
    EXPECT_TRUE(header.hasObjectWithType(CommunicationHeaderObjectType::ITEM_ID));
    EXPECT_EQ(header.get(CommunicationHeaderObjectType::ITEM_ID).as<ItemId>(), test_id);
}

TEST_F(CommunicationHeaderTest, ValueOrFunctionality)
{
    CommunicationHeader header;
    
    ItemId test_id = ItemId::ID10;
    ItemId alternative_id = ItemId::ID0;
    
    // Test when value is not present
    EXPECT_EQ(header.valueOr(alternative_id), alternative_id);
    
    // Test when value is present
    header.extend(test_id);
    EXPECT_EQ(header.valueOr(alternative_id), test_id);
}

TEST_F(CommunicationHeaderTest, NumBytesCalculation)
{
    CommunicationHeader header;
    
    // Empty header should have base size
    size_t base_size = header.numBytes();
    EXPECT_GT(base_size, 0U);
    
    // Add some objects and verify size increases
    header.append(CommunicationHeaderObjectType::NUM_ELEMENTS, static_cast<uint32_t>(1000));
    size_t with_object = header.numBytes();
    EXPECT_GT(with_object, base_size);
    
    // Add property and verify size increases again
    properties::Alpha alpha(0.5f);
    header.extend(alpha);
    size_t with_property = header.numBytes();
    EXPECT_GT(with_property, with_object);
}

TEST_F(CommunicationHeaderTest, BufferSerialization)
{
    CommunicationHeader header(Function::PLOT2);
    
    // Add some test data
    uint32_t num_elements = 500;
    float test_float = 3.14159f;
    properties::Alpha alpha(0.8f);
    
    header.append(CommunicationHeaderObjectType::NUM_ELEMENTS, num_elements);
    header.append(CommunicationHeaderObjectType::NUM_BYTES, test_float);
    header.extend(alpha, PropertyFlag::PERSISTENT);
    
    // Serialize to buffer
    size_t buffer_size = header.numBytes();
    FillableUInt8Array buffer(buffer_size);
    header.fillBufferWithData(buffer);
    
    // Verify buffer was filled
    EXPECT_EQ(buffer.size(), buffer_size);
}

TEST_F(CommunicationHeaderTest, SerializationOnly)
{
    // Create header with test data
    CommunicationHeader header(Function::SURF);
    
    uint32_t num_elements = 750;
    DataType data_type = DataType::DOUBLE;
    properties::LineWidth line_width(5U);
    properties::Alpha alpha(0.9f);
    
    header.append(CommunicationHeaderObjectType::NUM_ELEMENTS, num_elements);
    header.append(CommunicationHeaderObjectType::DATA_TYPE, data_type);
    header.extend(line_width, alpha, PropertyFlag::APPENDABLE);
    
    // Test serialization to buffer
    size_t buffer_size = header.numBytes();
    EXPECT_GT(buffer_size, 50U); // Should be substantial
    
    FillableUInt8Array buffer(buffer_size);
    header.fillBufferWithData(buffer);
    
    // Verify buffer was populated
    EXPECT_EQ(buffer.size(), buffer_size);
    
    // Verify the data starts correctly (first byte should be num objects)
    const uint8_t* data = buffer.data();
    EXPECT_EQ(data[0], 2); // 2 objects
    EXPECT_EQ(data[1], 2); // 2 properties
}

TEST_F(CommunicationHeaderTest, ComplexDataTypes)
{
    CommunicationHeader header;
    
    // Test with Vec3 and matrix data
    Vec3<double> test_vec(1.0, 2.0, 3.0);
    internal::Dimension2D test_dim;
    test_dim.rows = 10;
    test_dim.cols = 20;
    
    header.append(CommunicationHeaderObjectType::VEC3, test_vec);
    header.append(CommunicationHeaderObjectType::DIMENSION_2D, test_dim);
    
    EXPECT_TRUE(header.hasObjectWithType(CommunicationHeaderObjectType::VEC3));
    EXPECT_TRUE(header.hasObjectWithType(CommunicationHeaderObjectType::DIMENSION_2D));
    
    Vec3<double> retrieved_vec = header.get(CommunicationHeaderObjectType::VEC3).as<Vec3<double>>();
    EXPECT_DOUBLE_EQ(retrieved_vec.x, test_vec.x);
    EXPECT_DOUBLE_EQ(retrieved_vec.y, test_vec.y);
    EXPECT_DOUBLE_EQ(retrieved_vec.z, test_vec.z);
    
    internal::Dimension2D retrieved_dim = header.get(CommunicationHeaderObjectType::DIMENSION_2D).as<internal::Dimension2D>();
    EXPECT_EQ(retrieved_dim.rows, test_dim.rows);
    EXPECT_EQ(retrieved_dim.cols, test_dim.cols);
}

TEST_F(CommunicationHeaderTest, PropertyFromRawObject)
{
    CommunicationHeader header;
    
    // Create a raw property object
    properties::Alpha alpha(0.7f);
    CommunicationHeaderObject raw_obj;
    serializeToCommunicationHeaderObject(raw_obj, alpha);
    raw_obj.type = CommunicationHeaderObjectType::PROPERTY;
    
    header.appendPropertyFromRawObject(raw_obj);
    
    EXPECT_EQ(header.getPropertiesObjects().usedSize(), 1U);
    
    // Verify the property is in the lookup table
    const auto& prop_lut = header.getPropertyLookupTable();
    EXPECT_NE(prop_lut.data[static_cast<uint8_t>(PropertyType::ALPHA)], 255);
}

TEST_F(CommunicationHeaderTest, LargeHeaderSerialization)
{
    CommunicationHeader header(Function::PLOT3);
    
    // Add several objects (staying well under the limit)
    header.append(CommunicationHeaderObjectType::NUM_ELEMENTS, static_cast<uint32_t>(1000));
    header.append(CommunicationHeaderObjectType::NUM_BYTES, static_cast<uint32_t>(4000));
    header.append(CommunicationHeaderObjectType::DATA_TYPE, DataType::FLOAT);
    header.append(CommunicationHeaderObjectType::NUM_CHANNELS, static_cast<uint32_t>(3));
    header.append(CommunicationHeaderObjectType::BYTES_PER_ELEMENT, static_cast<uint32_t>(4));
    
    // Add several properties (staying under the limit)
    properties::Alpha alpha(0.5f);
    properties::LineWidth line_width(2U);
    properties::ZOffset z_offset(1.5f);
    
    header.extend(alpha, line_width, z_offset);
    header.extend(PropertyFlag::PERSISTENT, PropertyFlag::UPDATABLE);
    
    // Verify counts
    EXPECT_EQ(header.getObjects().usedSize(), 5U);
    EXPECT_EQ(header.getPropertiesObjects().usedSize(), 3U);
    
    // Test serialization
    size_t buffer_size = header.numBytes();
    EXPECT_GT(buffer_size, 100U); // Should be a substantial size
    
    FillableUInt8Array buffer(buffer_size);
    header.fillBufferWithData(buffer);
    
    // Verify buffer is populated correctly
    EXPECT_EQ(buffer.size(), buffer_size);
    
    // Verify the serialized data starts correctly
    const uint8_t* data = buffer.data();
    EXPECT_EQ(data[0], 5); // 5 objects
    EXPECT_EQ(data[1], 3); // 3 properties
    
    // Verify function is serialized
    Function* func_ptr = reinterpret_cast<Function*>(const_cast<uint8_t*>(data + 2));
    EXPECT_EQ(*func_ptr, Function::PLOT3);
}

TEST_F(CommunicationHeaderTest, EmptyHeaderSerialization)
{
    CommunicationHeader empty_header;
    
    size_t buffer_size = empty_header.numBytes();
    FillableUInt8Array buffer(buffer_size);
    empty_header.fillBufferWithData(buffer);
    
    // Verify empty header serialization
    EXPECT_EQ(buffer.size(), buffer_size);
    
    // Verify the data starts correctly (should be 0 objects, 0 properties)
    const uint8_t* data = buffer.data();
    EXPECT_EQ(data[0], 0); // 0 objects
    EXPECT_EQ(data[1], 0); // 0 properties
    
    // Function should be serialized next
    Function* func_ptr = reinterpret_cast<Function*>(const_cast<uint8_t*>(data + 2));
    EXPECT_EQ(*func_ptr, Function::UNKNOWN);
}

TEST_F(CommunicationHeaderTest, ColorHandling)
{
    CommunicationHeader header;
    
    // Test different color types with proper constructor parameters
    properties::Color color(255, 128, 0); // RGB values as integers
    header.extend(color);
    
    EXPECT_EQ(header.getPropertiesObjects().usedSize(), 1U);
    
    // Colors should be stored as properties in the lookup table
    const auto& prop_lut = header.getPropertyLookupTable();
    EXPECT_NE(prop_lut.data[static_cast<uint8_t>(PropertyType::COLOR)], 255);
}

TEST_F(CommunicationHeaderTest, BoundaryConditions)
{
    CommunicationHeader header;
    
    // Test with boundary values
    uint8_t min_uint8 = std::numeric_limits<uint8_t>::min();
    uint8_t max_uint8 = std::numeric_limits<uint8_t>::max();
    uint64_t max_uint64 = std::numeric_limits<uint64_t>::max();
    
    header.append(CommunicationHeaderObjectType::NUM_BYTES, min_uint8);
    header.append(CommunicationHeaderObjectType::NUM_ELEMENTS, max_uint8);
    header.append(CommunicationHeaderObjectType::BYTES_PER_ELEMENT, max_uint64);
    
    EXPECT_EQ(header.get(CommunicationHeaderObjectType::NUM_BYTES).as<uint8_t>(), min_uint8);
    EXPECT_EQ(header.get(CommunicationHeaderObjectType::NUM_ELEMENTS).as<uint8_t>(), max_uint8);
    EXPECT_EQ(header.get(CommunicationHeaderObjectType::BYTES_PER_ELEMENT).as<uint64_t>(), max_uint64);
}