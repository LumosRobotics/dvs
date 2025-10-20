#include "duoplot/communication_header_object.h"

#include <gtest/gtest.h>

#include <cmath>
#include <cstring>
#include <limits>
#include <memory>

#include "duoplot/constants.h"
#include "duoplot/encode_decode_functions.h"
#include "duoplot/enumerations.h"
#include "duoplot/item_id.h"
#include "duoplot/math/math.h"
#include "duoplot/plot_properties.h"

using namespace duoplot;
using namespace duoplot::internal;

class CommunicationHeaderObjectTest : public ::testing::Test
{
protected:
    void SetUp() override {}

    void TearDown() override {}

    // Helper function to test roundtrip serialization/deserialization
    template <typename T> void TestRoundtrip(CommunicationHeaderObjectType type, const T& original_value)
    {
        CommunicationHeaderObject obj(type, original_value);
        EXPECT_EQ(obj.type, type);

        T retrieved_value = obj.as<T>();
        EXPECT_EQ(retrieved_value, original_value);
    }

    // Helper for floating point comparisons
    template <> void TestRoundtrip<float>(CommunicationHeaderObjectType type, const float& original_value)
    {
        CommunicationHeaderObject obj(type, original_value);
        EXPECT_EQ(obj.type, type);

        float retrieved_value = obj.as<float>();
        if (std::isnan(original_value))
        {
            EXPECT_TRUE(std::isnan(retrieved_value));
        }
        else
        {
            EXPECT_FLOAT_EQ(retrieved_value, original_value);
        }
    }

    template <> void TestRoundtrip<double>(CommunicationHeaderObjectType type, const double& original_value)
    {
        CommunicationHeaderObject obj(type, original_value);
        EXPECT_EQ(obj.type, type);

        double retrieved_value = obj.as<double>();
        if (std::isnan(original_value))
        {
            EXPECT_TRUE(std::isnan(retrieved_value));
        }
        else
        {
            EXPECT_DOUBLE_EQ(retrieved_value, original_value);
        }
    }
};

// Test default constructor - should create object with UNKNOWN type
TEST_F(CommunicationHeaderObjectTest, DefaultConstructor)
{
    {
        CommunicationHeaderObject obj;
        EXPECT_EQ(obj.type, CommunicationHeaderObjectType::UNKNOWN);
    }
    {
        CommunicationHeaderObject obj{};
        EXPECT_EQ(obj.type, CommunicationHeaderObjectType::UNKNOWN);
    }
}

// Test constructor with type only - should set type and have zero size
TEST_F(CommunicationHeaderObjectTest, TypeOnlyConstructor)
{
    CommunicationHeaderObject obj(CommunicationHeaderObjectType::FUNCTION);
    EXPECT_EQ(obj.type, CommunicationHeaderObjectType::FUNCTION);
}

// Test integer types roundtrip - store and retrieve should preserve values
TEST_F(CommunicationHeaderObjectTest, IntegerTypesRoundtrip)
{
    // Test various integer sizes and signs
    TestRoundtrip(CommunicationHeaderObjectType::INT32, static_cast<int8_t>(-42));
    TestRoundtrip(CommunicationHeaderObjectType::INT32, static_cast<int16_t>(-1024));
    TestRoundtrip(CommunicationHeaderObjectType::INT32, static_cast<int32_t>(-65536));
    TestRoundtrip(CommunicationHeaderObjectType::INT32, static_cast<int64_t>(-4294967296LL));

    TestRoundtrip(CommunicationHeaderObjectType::NUM_BYTES, static_cast<uint8_t>(255U));
    TestRoundtrip(CommunicationHeaderObjectType::NUM_ELEMENTS, static_cast<uint16_t>(65535U));
    TestRoundtrip(CommunicationHeaderObjectType::NUM_BYTES, static_cast<uint32_t>(4294967295U));
    TestRoundtrip(CommunicationHeaderObjectType::NUM_BYTES, static_cast<uint64_t>(18446744073709551615ULL));
}

// Test floating point types roundtrip - store and retrieve should preserve values
TEST_F(CommunicationHeaderObjectTest, FloatingPointTypesRoundtrip)
{
    TestRoundtrip(CommunicationHeaderObjectType::NUM_BYTES, 3.14159f);
    TestRoundtrip(CommunicationHeaderObjectType::NUM_BYTES, 2.718281828459045);

    // Test special floating point values
    TestRoundtrip(CommunicationHeaderObjectType::NUM_BYTES, 0.0f);
    TestRoundtrip(CommunicationHeaderObjectType::NUM_BYTES, -0.0f);
    TestRoundtrip(CommunicationHeaderObjectType::NUM_BYTES, std::numeric_limits<float>::infinity());
    TestRoundtrip(CommunicationHeaderObjectType::NUM_BYTES, -std::numeric_limits<float>::infinity());
    TestRoundtrip(CommunicationHeaderObjectType::NUM_BYTES, std::numeric_limits<float>::quiet_NaN());
}

// Test boundary values for integer types
TEST_F(CommunicationHeaderObjectTest, IntegerBoundaryValues)
{
    // Test minimum and maximum values for different integer types
    TestRoundtrip(CommunicationHeaderObjectType::INT32, std::numeric_limits<int8_t>::min());
    TestRoundtrip(CommunicationHeaderObjectType::INT32, std::numeric_limits<int8_t>::max());

    TestRoundtrip(CommunicationHeaderObjectType::NUM_BYTES, std::numeric_limits<uint8_t>::min());
    TestRoundtrip(CommunicationHeaderObjectType::NUM_BYTES, std::numeric_limits<uint8_t>::max());

    TestRoundtrip(CommunicationHeaderObjectType::INT32, std::numeric_limits<int32_t>::min());
    TestRoundtrip(CommunicationHeaderObjectType::INT32, std::numeric_limits<int32_t>::max());

    TestRoundtrip(CommunicationHeaderObjectType::NUM_BYTES, std::numeric_limits<uint64_t>::min());
    TestRoundtrip(CommunicationHeaderObjectType::NUM_BYTES, std::numeric_limits<uint64_t>::max());
}

// Test Dimension2D roundtrip - should preserve rows and cols values
TEST_F(CommunicationHeaderObjectTest, Dimension2DRoundtrip)
{
    internal::Dimension2D original_dim;
    original_dim.rows = 100U;
    original_dim.cols = 200U;

    CommunicationHeaderObject obj(CommunicationHeaderObjectType::DIMENSION_2D, original_dim);
    EXPECT_EQ(obj.type, CommunicationHeaderObjectType::DIMENSION_2D);

    internal::Dimension2D retrieved = obj.as<internal::Dimension2D>();
    EXPECT_EQ(retrieved.rows, original_dim.rows);
    EXPECT_EQ(retrieved.cols, original_dim.cols);
}

// Test Vec3 pair roundtrip - should preserve both vectors exactly
TEST_F(CommunicationHeaderObjectTest, Vec3PairRoundtrip)
{
    Vec3<double> first_vec(1.0, 2.0, 3.0);
    Vec3<double> second_vec(4.0, 5.0, 6.0);
    std::pair<Vec3<double>, Vec3<double>> original_pair = std::make_pair(first_vec, second_vec);

    CommunicationHeaderObject obj(CommunicationHeaderObjectType::AXIS_MIN_MAX_VEC, original_pair);
    EXPECT_EQ(obj.type, CommunicationHeaderObjectType::AXIS_MIN_MAX_VEC);

    auto retrieved = obj.as<std::pair<Vec3<double>, Vec3<double>>>();
    EXPECT_DOUBLE_EQ(retrieved.first.x, original_pair.first.x);
    EXPECT_DOUBLE_EQ(retrieved.first.y, original_pair.first.y);
    EXPECT_DOUBLE_EQ(retrieved.first.z, original_pair.first.z);
    EXPECT_DOUBLE_EQ(retrieved.second.x, original_pair.second.x);
    EXPECT_DOUBLE_EQ(retrieved.second.y, original_pair.second.y);
    EXPECT_DOUBLE_EQ(retrieved.second.z, original_pair.second.z);
}

// Test ItemId roundtrip - should preserve enum value exactly
TEST_F(CommunicationHeaderObjectTest, ItemIdRoundtrip)
{
    // Test different ItemId values
    std::vector<ItemId> test_ids = {ItemId::ID0, ItemId::ID5, ItemId::ID10};

    for (const auto& original_id : test_ids)
    {
        CommunicationHeaderObject obj(CommunicationHeaderObjectType::ITEM_ID, original_id);
        EXPECT_EQ(obj.type, CommunicationHeaderObjectType::ITEM_ID);

        ItemId retrieved = obj.as<ItemId>();
        EXPECT_EQ(retrieved, original_id);
    }
}

// Test DataType roundtrip - should preserve enum value exactly
TEST_F(CommunicationHeaderObjectTest, DataTypeRoundtrip)
{
    // Test different DataType values
    std::vector<internal::DataType> test_types = {
        internal::DataType::FLOAT, internal::DataType::DOUBLE, internal::DataType::INT32, internal::DataType::UINT8};

    for (const auto& original_type : test_types)
    {
        CommunicationHeaderObject obj(CommunicationHeaderObjectType::DATA_TYPE, original_type);
        EXPECT_EQ(obj.type, CommunicationHeaderObjectType::DATA_TYPE);

        internal::DataType retrieved = obj.as<internal::DataType>();
        EXPECT_EQ(retrieved, original_type);
    }
}

// Test Vec3<double> roundtrip - should preserve all components exactly
TEST_F(CommunicationHeaderObjectTest, Vec3DoubleRoundtrip)
{
    std::vector<Vec3<double>> test_vectors = {Vec3<double>(0.0, 0.0, 0.0),
                                              Vec3<double>(1.0, 2.0, 3.0),
                                              Vec3<double>(-1.5, 2.5, -3.5),
                                              Vec3<double>(1e10, -1e-10, 1e100)};

    for (const auto& original_vec : test_vectors)
    {
        CommunicationHeaderObject obj(CommunicationHeaderObjectType::VEC3, original_vec);
        EXPECT_EQ(obj.type, CommunicationHeaderObjectType::VEC3);

        Vec3<double> retrieved = obj.as<Vec3<double>>();
        EXPECT_DOUBLE_EQ(retrieved.x, original_vec.x);
        EXPECT_DOUBLE_EQ(retrieved.y, original_vec.y);
        EXPECT_DOUBLE_EQ(retrieved.z, original_vec.z);
    }
}

// Test MatrixFixed<double, 3, 3> roundtrip - should preserve all elements exactly
TEST_F(CommunicationHeaderObjectTest, MatrixFixed3x3Roundtrip)
{
    MatrixFixed<double, 3, 3> original_mat;

    // Fill with test pattern
    double value = 1.0;
    for (size_t r = 0; r < 3; r++)
    {
        for (size_t c = 0; c < 3; c++)
        {
            original_mat(r, c) = value;
            value += 1.1;  // Non-integer increment to test precision
        }
    }

    CommunicationHeaderObject obj(CommunicationHeaderObjectType::SCALE_MATRIX, original_mat);
    EXPECT_EQ(obj.type, CommunicationHeaderObjectType::SCALE_MATRIX);

    MatrixFixed<double, 3, 3> retrieved = obj.as<MatrixFixed<double, 3, 3>>();
    for (size_t r = 0; r < 3; r++)
    {
        for (size_t c = 0; c < 3; c++)
        {
            EXPECT_DOUBLE_EQ(retrieved(r, c), original_mat(r, c));
        }
    }
}

// Test Label roundtrip - should preserve string content and length
TEST_F(CommunicationHeaderObjectTest, LabelRoundtrip)
{
    std::vector<std::string> test_strings = {"short",
                                             "test_label",
                                             "",  // Empty string
                                             "a_much_longer_label_with_underscores_and_numbers_123"};

    for (const auto& test_str : test_strings)
    {
        properties::Label original_label(test_str.c_str());
        CommunicationHeaderObject obj(CommunicationHeaderObjectType::LABEL, original_label);
        EXPECT_EQ(obj.type, CommunicationHeaderObjectType::LABEL);

        properties::Label retrieved = obj.as<properties::Label>();
        EXPECT_STREQ(retrieved.data, original_label.data);
        EXPECT_EQ(retrieved.length, original_label.length);
    }
}

// Test property types roundtrip - should preserve property values and types
TEST_F(CommunicationHeaderObjectTest, PropertyTypesRoundtrip)
{
    // Test Alpha property using serialize/deserialize
    {
        properties::Alpha original_alpha(0.75f);
        CommunicationHeaderObject obj;
        serializeToCommunicationHeaderObject(obj, original_alpha);
        obj.type = CommunicationHeaderObjectType::PROPERTY;

        properties::Alpha retrieved = obj.as<properties::Alpha>();
        EXPECT_FLOAT_EQ(retrieved.data, original_alpha.data);
        EXPECT_EQ(retrieved.getPropertyType(), internal::PropertyType::ALPHA);
    }

    // Test LineWidth property using serialize/deserialize
    {
        properties::LineWidth original_lw(5U);
        CommunicationHeaderObject obj;
        serializeToCommunicationHeaderObject(obj, original_lw);
        obj.type = CommunicationHeaderObjectType::PROPERTY;

        properties::LineWidth retrieved = obj.as<properties::LineWidth>();
        EXPECT_EQ(retrieved.data, original_lw.data);
        EXPECT_EQ(retrieved.getPropertyType(), internal::PropertyType::LINE_WIDTH);
    }

    // Test ZOffset property with different values using serialize/deserialize
    {
        std::vector<float> test_offsets = {0.0f, 1.5f, -2.3f, 100.0f};
        for (float offset_val : test_offsets)
        {
            properties::ZOffset original_zo(offset_val);
            CommunicationHeaderObject obj;
            serializeToCommunicationHeaderObject(obj, original_zo);
            obj.type = CommunicationHeaderObjectType::PROPERTY;

            properties::ZOffset retrieved = obj.as<properties::ZOffset>();
            EXPECT_FLOAT_EQ(retrieved.data, offset_val);
            EXPECT_EQ(retrieved.getPropertyType(), internal::PropertyType::Z_OFFSET);
        }
    }
}

// Test data consistency across multiple operations
TEST_F(CommunicationHeaderObjectTest, MultipleOperationsConsistency)
{
    const int test_iterations = 50;

    for (int i = 0; i < test_iterations; ++i)
    {
        // Create object with deterministic but varied data
        uint32_t original_val = static_cast<uint32_t>(i * 42 + 17);
        CommunicationHeaderObject obj(CommunicationHeaderObjectType::NUM_ELEMENTS, original_val);

        // Verify type is preserved
        EXPECT_EQ(obj.type, CommunicationHeaderObjectType::NUM_ELEMENTS);

        // Verify value is preserved through retrieval
        uint32_t retrieved_val = obj.as<uint32_t>();
        EXPECT_EQ(retrieved_val, original_val) << "Failed at iteration " << i;

        // Verify repeated retrieval gives same result
        uint32_t retrieved_again = obj.as<uint32_t>();
        EXPECT_EQ(retrieved_again, retrieved_val) << "Inconsistent retrieval at iteration " << i;
    }
}

// Test copy and assignment behavior
TEST_F(CommunicationHeaderObjectTest, CopyAndAssignmentBehavior)
{
    double original_value = 12.345;
    CommunicationHeaderObject original(CommunicationHeaderObjectType::NUM_BYTES, original_value);

    // Test copy constructor
    CommunicationHeaderObject copy_constructed = original;
    EXPECT_EQ(copy_constructed.type, original.type);
    EXPECT_DOUBLE_EQ(copy_constructed.as<double>(), original_value);

    // Test assignment operator
    CommunicationHeaderObject assigned;
    assigned = original;
    EXPECT_EQ(assigned.type, original.type);
    EXPECT_DOUBLE_EQ(assigned.as<double>(), original_value);

    // Verify independence - modifying one shouldn't affect others
    double new_value = 98.765;
    CommunicationHeaderObject new_obj(CommunicationHeaderObjectType::NUM_BYTES, new_value);
    assigned = new_obj;

    // Original should be unchanged
    EXPECT_DOUBLE_EQ(original.as<double>(), original_value);
    EXPECT_DOUBLE_EQ(copy_constructed.as<double>(), original_value);
    EXPECT_DOUBLE_EQ(assigned.as<double>(), new_value);
}