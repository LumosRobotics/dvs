#include <gtest/gtest.h>
#include <cstring>
#include <memory>
#include <limits>
#include <vector>
#include <stdexcept>

#include "duoplot/plot_properties.h"
#include "duoplot/enumerations.h"
#include "duoplot/math/math.h"

using namespace duoplot;
using namespace duoplot::internal;
using namespace duoplot::properties;

// PropertyBase tests
class PropertyBaseTest : public ::testing::Test
{
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(PropertyBaseTest, DefaultConstruction)
{
    PropertyBase prop;
    EXPECT_EQ(prop.getPropertyType(), PropertyType::UNKNOWN);
}

TEST_F(PropertyBaseTest, ConstructionWithType)
{
    PropertyBase prop(PropertyType::ALPHA);
    EXPECT_EQ(prop.getPropertyType(), PropertyType::ALPHA);
}

TEST_F(PropertyBaseTest, SetPropertyType)
{
    PropertyBase prop;
    prop.setPropertyType(PropertyType::LINE_WIDTH);
    EXPECT_EQ(prop.getPropertyType(), PropertyType::LINE_WIDTH);
}

// Utility function tests
class UtilityFunctionTest : public ::testing::Test
{
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(UtilityFunctionTest, SafeStringLenCheck)
{
    const char* test_str = "hello";
    EXPECT_EQ(safeStringLenCheck(test_str, 10), 5U);
    
    const char* longer_str = "this is a longer string";
    EXPECT_EQ(safeStringLenCheck(longer_str, 10), 10U); // Should stop at max_length
    
    const char* empty_str = "";
    EXPECT_EQ(safeStringLenCheck(empty_str, 10), 0U);
    
    const char* exact_length = "exact";
    EXPECT_EQ(safeStringLenCheck(exact_length, 5), 5U);
}

// LineWidth tests
class LineWidthTest : public ::testing::Test
{
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(LineWidthTest, DefaultConstruction)
{
    LineWidth lw;
    EXPECT_EQ(lw.getPropertyType(), PropertyType::LINE_WIDTH);
}

TEST_F(LineWidthTest, ConstructionWithValue)
{
    LineWidth lw(5);
    EXPECT_EQ(lw.getPropertyType(), PropertyType::LINE_WIDTH);
    EXPECT_EQ(lw.data, 5);
}

TEST_F(LineWidthTest, BoundaryValues)
{
    LineWidth lw_min(0);
    EXPECT_EQ(lw_min.data, 0);
    
    LineWidth lw_max(255);
    EXPECT_EQ(lw_max.data, 255);
}

// Alpha tests
class AlphaTest : public ::testing::Test
{
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(AlphaTest, DefaultConstruction)
{
    Alpha alpha;
    EXPECT_EQ(alpha.getPropertyType(), PropertyType::ALPHA);
}

TEST_F(AlphaTest, ConstructionWithValue)
{
    Alpha alpha(0.75f);
    EXPECT_EQ(alpha.getPropertyType(), PropertyType::ALPHA);
    EXPECT_FLOAT_EQ(alpha.data, 0.75f);
}

TEST_F(AlphaTest, BoundaryValues)
{
    Alpha alpha_min(0.0f);
    EXPECT_FLOAT_EQ(alpha_min.data, 0.0f);
    
    Alpha alpha_max(1.0f);
    EXPECT_FLOAT_EQ(alpha_max.data, 1.0f);
    
    Alpha alpha_beyond(2.0f); // Should accept values beyond range
    EXPECT_FLOAT_EQ(alpha_beyond.data, 2.0f);
}

// ZOffset tests
class ZOffsetTest : public ::testing::Test
{
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(ZOffsetTest, DefaultConstruction)
{
    ZOffset z_offset;
    EXPECT_EQ(z_offset.getPropertyType(), PropertyType::Z_OFFSET);
}

TEST_F(ZOffsetTest, ConstructionWithValue)
{
    ZOffset z_offset(1.5f);
    EXPECT_EQ(z_offset.getPropertyType(), PropertyType::Z_OFFSET);
    EXPECT_FLOAT_EQ(z_offset.data, 1.5f);
}

TEST_F(ZOffsetTest, NegativeValues)
{
    ZOffset z_offset(-2.3f);
    EXPECT_FLOAT_EQ(z_offset.data, -2.3f);
}

// Transform tests
class TransformTest : public ::testing::Test
{
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(TransformTest, DefaultConstruction)
{
    Transform transform;
    EXPECT_EQ(transform.getPropertyType(), PropertyType::TRANSFORM);
}

TEST_F(TransformTest, ConstructionWithMatricesAndVector)
{
    MatrixFixed<double, 3, 3> scale_mat;
    MatrixFixed<double, 3, 3> rotation_mat;
    Vec3<double> translation(1.0, 2.0, 3.0);
    
    // Initialize matrices
    for (size_t r = 0; r < 3; ++r)
    {
        for (size_t c = 0; c < 3; ++c)
        {
            scale_mat(r, c) = r + c + 1.0;
            rotation_mat(r, c) = (r + c) * 2.0;
        }
    }
    
    Transform transform(scale_mat, rotation_mat, translation);
    
    EXPECT_EQ(transform.getPropertyType(), PropertyType::TRANSFORM);
    EXPECT_DOUBLE_EQ(transform.translation.x, 1.0);
    EXPECT_DOUBLE_EQ(transform.translation.y, 2.0);
    EXPECT_DOUBLE_EQ(transform.translation.z, 3.0);
    
    // Check matrices
    for (size_t r = 0; r < 3; ++r)
    {
        for (size_t c = 0; c < 3; ++c)
        {
            EXPECT_DOUBLE_EQ(transform.scale(r, c), r + c + 1.0);
            EXPECT_DOUBLE_EQ(transform.rotation(r, c), (r + c) * 2.0);
        }
    }
}

// Label tests
class LabelTest : public ::testing::Test
{
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(LabelTest, DefaultConstruction)
{
    Label label;
    EXPECT_EQ(label.getPropertyType(), PropertyType::NAME);
    EXPECT_EQ(label.length, 0U);
    EXPECT_STREQ(label.data, "");
}

TEST_F(LabelTest, ConstructionWithString)
{
    Label label("test_label");
    EXPECT_EQ(label.getPropertyType(), PropertyType::NAME);
    EXPECT_EQ(label.length, 10U);
    EXPECT_STREQ(label.data, "test_label");
}

TEST_F(LabelTest, EmptyString)
{
    Label label("");
    EXPECT_EQ(label.length, 0U);
    EXPECT_STREQ(label.data, "");
}

TEST_F(LabelTest, MaxLengthString)
{
    // Create a string of exactly 100 characters
    std::string max_str(100, 'a');
    Label label(max_str.c_str());
    EXPECT_EQ(label.length, 100U);
    EXPECT_STREQ(label.data, max_str.c_str());
}

TEST_F(LabelTest, ResetData)
{
    Label label("test");
    EXPECT_EQ(label.length, 4U);
    
    label.resetData();
    EXPECT_EQ(label.length, 0U);
    EXPECT_STREQ(label.data, "");
}

TEST_F(LabelTest, EqualityOperator)
{
    Label label1("same");
    Label label2("same");
    Label label3("different");
    
    EXPECT_TRUE(label1 == label2);
    EXPECT_FALSE(label1 == label3);
}

// Color tests
class ColorTest : public ::testing::Test
{
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(ColorTest, DefaultConstruction)
{
    Color color;
    // Default constructor doesn't initialize values
}

TEST_F(ColorTest, ConstructionWithRGB)
{
    Color color(255, 128, 64);
    EXPECT_EQ(color.red, 255);
    EXPECT_EQ(color.green, 128);
    EXPECT_EQ(color.blue, 64);
}

TEST_F(ColorTest, ConstructionFromColorT)
{
    Color red_color(ColorT::RED);
    EXPECT_EQ(red_color.red, 255);
    EXPECT_EQ(red_color.green, 0);
    EXPECT_EQ(red_color.blue, 0);
    
    Color green_color(ColorT::GREEN);
    EXPECT_EQ(green_color.red, 0);
    EXPECT_EQ(green_color.green, 255);
    EXPECT_EQ(green_color.blue, 0);
    
    Color blue_color(ColorT::BLUE);
    EXPECT_EQ(blue_color.red, 0);
    EXPECT_EQ(blue_color.green, 0);
    EXPECT_EQ(blue_color.blue, 255);
    
    Color white_color(ColorT::WHITE);
    EXPECT_EQ(white_color.red, 255);
    EXPECT_EQ(white_color.green, 255);
    EXPECT_EQ(white_color.blue, 255);
    
    Color black_color(ColorT::BLACK);
    EXPECT_EQ(black_color.red, 0);
    EXPECT_EQ(black_color.green, 0);
    EXPECT_EQ(black_color.blue, 0);
}

TEST_F(ColorTest, AssignmentFromColorT)
{
    Color color;
    color = ColorT::CYAN;
    EXPECT_EQ(color.red, 0);
    EXPECT_EQ(color.green, 255);
    EXPECT_EQ(color.blue, 255);
}

TEST_F(ColorTest, EqualityOperators)
{
    Color color1(255, 128, 64);
    Color color2(255, 128, 64);
    Color color3(128, 255, 64);
    
    EXPECT_TRUE(color1 == color2);
    EXPECT_FALSE(color1 == color3);
    EXPECT_FALSE(color1 != color2);
    EXPECT_TRUE(color1 != color3);
}

TEST_F(ColorTest, StaticConstants)
{
    // Test that static constants are accessible
    EXPECT_EQ(Color::RED, ColorT::RED);
    EXPECT_EQ(Color::GREEN, ColorT::GREEN);
    EXPECT_EQ(Color::BLUE, ColorT::BLUE);
}

// EdgeColor tests
class EdgeColorTest : public ::testing::Test
{
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(EdgeColorTest, DefaultConstruction)
{
    EdgeColor edge_color;
    EXPECT_EQ(edge_color.getPropertyType(), PropertyType::EDGE_COLOR);
    EXPECT_EQ(edge_color.use_color, 1U);
}

TEST_F(EdgeColorTest, ConstructionWithRGB)
{
    EdgeColor edge_color(255, 128, 64);
    EXPECT_EQ(edge_color.getPropertyType(), PropertyType::EDGE_COLOR);
    EXPECT_EQ(edge_color.use_color, 1U);
    EXPECT_EQ(edge_color.red, 255);
    EXPECT_EQ(edge_color.green, 128);
    EXPECT_EQ(edge_color.blue, 64);
}

TEST_F(EdgeColorTest, ConstructionWithUseColorFlag)
{
    EdgeColor edge_color(0U); // No color
    EXPECT_EQ(edge_color.use_color, 0U);
    EXPECT_EQ(edge_color.red, 0U);
    EXPECT_EQ(edge_color.green, 0U);
    EXPECT_EQ(edge_color.blue, 0U);
}

TEST_F(EdgeColorTest, ConstructionFromEdgeColorT)
{
    EdgeColor red_edge(EdgeColorT::RED);
    EXPECT_EQ(red_edge.use_color, 1U);
    EXPECT_EQ(red_edge.red, 255);
    EXPECT_EQ(red_edge.green, 0);
    EXPECT_EQ(red_edge.blue, 0);
    
    EdgeColor none_edge(EdgeColorT::NONE);
    EXPECT_EQ(none_edge.use_color, 0U);
}

// FaceColor tests
class FaceColorTest : public ::testing::Test
{
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(FaceColorTest, DefaultConstruction)
{
    FaceColor face_color;
    EXPECT_EQ(face_color.getPropertyType(), PropertyType::FACE_COLOR);
    EXPECT_EQ(face_color.use_color, 1U);
    EXPECT_EQ(face_color.red, 0);
    EXPECT_EQ(face_color.green, 0);
    EXPECT_EQ(face_color.blue, 0);
}

TEST_F(FaceColorTest, ConstructionWithRGB)
{
    FaceColor face_color(255, 128, 64);
    EXPECT_EQ(face_color.getPropertyType(), PropertyType::FACE_COLOR);
    EXPECT_EQ(face_color.use_color, 1U);
    EXPECT_EQ(face_color.red, 255);
    EXPECT_EQ(face_color.green, 128);
    EXPECT_EQ(face_color.blue, 64);
}

TEST_F(FaceColorTest, ConstructionFromFaceColorT)
{
    FaceColor magenta_face(FaceColorT::MAGENTA);
    EXPECT_EQ(magenta_face.use_color, 1U);
    EXPECT_EQ(magenta_face.red, 255);
    EXPECT_EQ(magenta_face.green, 0);
    EXPECT_EQ(magenta_face.blue, 255);
    
    FaceColor none_face(FaceColorT::NONE);
    EXPECT_EQ(none_face.use_color, 0U);
}

// Silhouette tests
class SilhouetteTest : public ::testing::Test
{
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(SilhouetteTest, DefaultConstruction)
{
    Silhouette silhouette;
    EXPECT_EQ(silhouette.getPropertyType(), PropertyType::SILHOUETTE);
}

TEST_F(SilhouetteTest, ConstructionWithRGB)
{
    Silhouette silhouette(255, 128, 64);
    EXPECT_EQ(silhouette.getPropertyType(), PropertyType::SILHOUETTE);
    EXPECT_EQ(silhouette.red, 255);
    EXPECT_EQ(silhouette.green, 128);
    EXPECT_EQ(silhouette.blue, 64);
    EXPECT_FLOAT_EQ(silhouette.percentage, 0.1f); // Default percentage
}

TEST_F(SilhouetteTest, ConstructionWithRGBAndPercentage)
{
    Silhouette silhouette(255, 128, 64, 0.5f);
    EXPECT_EQ(silhouette.red, 255);
    EXPECT_EQ(silhouette.green, 128);
    EXPECT_EQ(silhouette.blue, 64);
    EXPECT_FLOAT_EQ(silhouette.percentage, 0.5f);
}

TEST_F(SilhouetteTest, ConstructionFromSilhouetteT)
{
    Silhouette yellow_silhouette(SilhouetteT::YELLOW);
    EXPECT_EQ(yellow_silhouette.red, 255);
    EXPECT_EQ(yellow_silhouette.green, 255);
    EXPECT_EQ(yellow_silhouette.blue, 0);
}

TEST_F(SilhouetteTest, AssignmentFromSilhouetteT)
{
    Silhouette silhouette;
    silhouette = SilhouetteT::GRAY;
    EXPECT_EQ(silhouette.red, 127);
    EXPECT_EQ(silhouette.green, 127);
    EXPECT_EQ(silhouette.blue, 127);
}

// PointSize tests
class PointSizeTest : public ::testing::Test
{
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(PointSizeTest, DefaultConstruction)
{
    PointSize point_size;
    EXPECT_EQ(point_size.getPropertyType(), PropertyType::POINT_SIZE);
}

TEST_F(PointSizeTest, ConstructionWithValue)
{
    PointSize point_size(10);
    EXPECT_EQ(point_size.getPropertyType(), PropertyType::POINT_SIZE);
    EXPECT_EQ(point_size.data, 10);
}

// DistanceFrom tests
class DistanceFromTest : public ::testing::Test
{
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(DistanceFromTest, DefaultConstruction)
{
    DistanceFrom distance_from;
    EXPECT_EQ(distance_from.getPropertyType(), PropertyType::DISTANCE_FROM);
}

TEST_F(DistanceFromTest, StaticFactoryMethodX)
{
    DistanceFrom df = DistanceFrom::x(5.0, 1.0, 10.0);
    EXPECT_EQ(df.getPropertyType(), PropertyType::DISTANCE_FROM);
    EXPECT_DOUBLE_EQ(df.getPoint().x, 5.0);
    EXPECT_DOUBLE_EQ(df.getPoint().y, 0.0);
    EXPECT_DOUBLE_EQ(df.getPoint().z, 0.0);
    EXPECT_DOUBLE_EQ(df.getMinDist(), 1.0);
    EXPECT_DOUBLE_EQ(df.getMaxDist(), 10.0);
    EXPECT_EQ(df.getDistFromType(), DistanceFromType::X);
}

TEST_F(DistanceFromTest, StaticFactoryMethodY)
{
    DistanceFrom df = DistanceFrom::y(3.0, 0.5, 8.0);
    EXPECT_DOUBLE_EQ(df.getPoint().x, 0.0);
    EXPECT_DOUBLE_EQ(df.getPoint().y, 3.0);
    EXPECT_DOUBLE_EQ(df.getPoint().z, 0.0);
    EXPECT_EQ(df.getDistFromType(), DistanceFromType::Y);
}

TEST_F(DistanceFromTest, StaticFactoryMethodZ)
{
    DistanceFrom df = DistanceFrom::z(7.0, 2.0, 15.0);
    EXPECT_DOUBLE_EQ(df.getPoint().x, 0.0);
    EXPECT_DOUBLE_EQ(df.getPoint().y, 0.0);
    EXPECT_DOUBLE_EQ(df.getPoint().z, 7.0);
    EXPECT_EQ(df.getDistFromType(), DistanceFromType::Z);
}

TEST_F(DistanceFromTest, StaticFactoryMethodXY)
{
    PointXY<double> p{2.0, 4.0};
    DistanceFrom df = DistanceFrom::xy(p, 1.0, 5.0);
    EXPECT_DOUBLE_EQ(df.getPoint().x, 2.0);
    EXPECT_DOUBLE_EQ(df.getPoint().y, 4.0);
    EXPECT_DOUBLE_EQ(df.getPoint().z, 0.0);
    EXPECT_EQ(df.getDistFromType(), DistanceFromType::XY);
}

TEST_F(DistanceFromTest, StaticFactoryMethodXZ)
{
    PointXZ<double> p{3.0, 6.0};
    DistanceFrom df = DistanceFrom::xz(p, 0.5, 12.0);
    EXPECT_DOUBLE_EQ(df.getPoint().x, 3.0);
    EXPECT_DOUBLE_EQ(df.getPoint().y, 0.0);
    EXPECT_DOUBLE_EQ(df.getPoint().z, 6.0);
    EXPECT_EQ(df.getDistFromType(), DistanceFromType::XZ);
}

TEST_F(DistanceFromTest, StaticFactoryMethodYZ)
{
    PointYZ<double> p{1.5, 2.5};
    DistanceFrom df = DistanceFrom::yz(p, 0.1, 20.0);
    EXPECT_DOUBLE_EQ(df.getPoint().x, 0.0);
    EXPECT_DOUBLE_EQ(df.getPoint().y, 1.5);
    EXPECT_DOUBLE_EQ(df.getPoint().z, 2.5);
    EXPECT_EQ(df.getDistFromType(), DistanceFromType::YZ);
}

TEST_F(DistanceFromTest, StaticFactoryMethodXYZ)
{
    Point3<double> p{1.0, 2.0, 3.0};
    DistanceFrom df = DistanceFrom::xyz(p, 0.5, 25.0);
    EXPECT_DOUBLE_EQ(df.getPoint().x, 1.0);
    EXPECT_DOUBLE_EQ(df.getPoint().y, 2.0);
    EXPECT_DOUBLE_EQ(df.getPoint().z, 3.0);
    EXPECT_EQ(df.getDistFromType(), DistanceFromType::XYZ);
}

TEST_F(DistanceFromTest, Setters)
{
    DistanceFrom df;
    
    Vec3<double> new_point(10.0, 20.0, 30.0);
    df.setPoint(new_point);
    EXPECT_DOUBLE_EQ(df.getPoint().x, 10.0);
    EXPECT_DOUBLE_EQ(df.getPoint().y, 20.0);
    EXPECT_DOUBLE_EQ(df.getPoint().z, 30.0);
    
    df.setMinDist(5.0);
    EXPECT_DOUBLE_EQ(df.getMinDist(), 5.0);
    
    df.setMaxDist(50.0);
    EXPECT_DOUBLE_EQ(df.getMaxDist(), 50.0);
    
    df.setDistFromType(DistanceFromType::XYZ);
    EXPECT_EQ(df.getDistFromType(), DistanceFromType::XYZ);
}

// BufferSize tests
class BufferSizeTest : public ::testing::Test
{
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(BufferSizeTest, DefaultConstruction)
{
    BufferSize buffer_size;
    EXPECT_EQ(buffer_size.getPropertyType(), PropertyType::BUFFER_SIZE);
}

TEST_F(BufferSizeTest, ConstructionWithValue)
{
    BufferSize buffer_size(1024);
    EXPECT_EQ(buffer_size.getPropertyType(), PropertyType::BUFFER_SIZE);
    EXPECT_EQ(buffer_size.data, 1024);
}

TEST_F(BufferSizeTest, BoundaryValues)
{
    BufferSize buffer_size_min(0);
    EXPECT_EQ(buffer_size_min.data, 0);
    
    BufferSize buffer_size_max(65535);
    EXPECT_EQ(buffer_size_max.data, 65535);
}

// ColorInternal tests
class ColorInternalTest : public ::testing::Test
{
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(ColorInternalTest, DefaultConstruction)
{
    ColorInternal color_internal;
    EXPECT_EQ(color_internal.getPropertyType(), PropertyType::COLOR);
}

TEST_F(ColorInternalTest, ConstructionFromColor)
{
    Color color(255, 128, 64);
    ColorInternal color_internal(color);
    EXPECT_EQ(color_internal.getPropertyType(), PropertyType::COLOR);
    EXPECT_EQ(color_internal.red, 255);
    EXPECT_EQ(color_internal.green, 128);
    EXPECT_EQ(color_internal.blue, 64);
}

TEST_F(ColorInternalTest, ConstructionWithRGB)
{
    ColorInternal color_internal(200, 150, 100);
    EXPECT_EQ(color_internal.red, 200);
    EXPECT_EQ(color_internal.green, 150);
    EXPECT_EQ(color_internal.blue, 100);
}

TEST_F(ColorInternalTest, ConstructionFromColorT)
{
    ColorInternal red_internal(ColorT::RED);
    EXPECT_EQ(red_internal.red, 255);
    EXPECT_EQ(red_internal.green, 0);
    EXPECT_EQ(red_internal.blue, 0);
}

// Dimension2D tests
class Dimension2DTest : public ::testing::Test
{
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(Dimension2DTest, DefaultConstruction)
{
    Dimension2D dim;
    EXPECT_EQ(dim.rows, 0U);
    EXPECT_EQ(dim.cols, 0U);
}

TEST_F(Dimension2DTest, ConstructionWithValues)
{
    Dimension2D dim(10, 20);
    EXPECT_EQ(dim.rows, 10U);
    EXPECT_EQ(dim.cols, 20U);
}

// Enum tests
class EnumTest : public ::testing::Test
{
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(EnumTest, LineStyleEnum)
{
    LineStyle solid = LineStyle::SOLID;
    LineStyle dashed = LineStyle::DASHED;
    LineStyle short_dashed = LineStyle::SHORT_DASHED;
    LineStyle long_dashed = LineStyle::LONG_DASHED;
    
    EXPECT_NE(solid, dashed);
    EXPECT_NE(dashed, short_dashed);
    EXPECT_NE(short_dashed, long_dashed);
}

TEST_F(EnumTest, ScatterStyleEnum)
{
    ScatterStyle square = ScatterStyle::SQUARE;
    ScatterStyle circle = ScatterStyle::CIRCLE;
    ScatterStyle disc = ScatterStyle::DISC;
    ScatterStyle plus = ScatterStyle::PLUS;
    ScatterStyle cross = ScatterStyle::CROSS;
    
    EXPECT_NE(square, circle);
    EXPECT_NE(circle, disc);
    EXPECT_NE(disc, plus);
    EXPECT_NE(plus, cross);
}

TEST_F(EnumTest, ColorMapEnum)
{
    ColorMap jet = ColorMap::JET;
    ColorMap hsv = ColorMap::HSV;
    ColorMap magma = ColorMap::MAGMA;
    ColorMap viridis = ColorMap::VIRIDIS;
    ColorMap pastel = ColorMap::PASTEL;
    ColorMap jet_soft = ColorMap::JET_SOFT;
    ColorMap jet_bright = ColorMap::JET_BRIGHT;
    ColorMap unknown = ColorMap::UNKNOWN;
    
    EXPECT_NE(jet, hsv);
    EXPECT_NE(hsv, magma);
    EXPECT_NE(magma, viridis);
    EXPECT_NE(viridis, pastel);
    EXPECT_NE(pastel, jet_soft);
    EXPECT_NE(jet_soft, jet_bright);
    EXPECT_NE(jet_bright, unknown);
}

// Property flag constants tests
class PropertyFlagTest : public ::testing::Test
{
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(PropertyFlagTest, AvailableFlags)
{
    // Test that the constexpr flags are accessible
    PropertyFlag persistent = PERSISTENT;
    PropertyFlag interpolate = INTERPOLATE_COLORMAP;
    PropertyFlag exclude = EXCLUDE_FROM_SELECTION;
    
    EXPECT_EQ(persistent, PropertyFlag::PERSISTENT);
    EXPECT_EQ(interpolate, PropertyFlag::INTERPOLATE_COLORMAP);
    EXPECT_EQ(exclude, PropertyFlag::EXCLUDE_FROM_SELECTION);
}

TEST_F(PropertyFlagTest, NotReadyFlags)
{
    // Test that not_ready flags are accessible
    PropertyFlag appendable = not_ready::APPENDABLE;
    PropertyFlag updatable = not_ready::UPDATABLE;
    PropertyFlag selectable = not_ready::SELECTABLE;
    PropertyFlag fast_plot = not_ready::FAST_PLOT;
    
    EXPECT_EQ(appendable, PropertyFlag::APPENDABLE);
    EXPECT_EQ(updatable, PropertyFlag::UPDATABLE);
    EXPECT_EQ(selectable, PropertyFlag::SELECTABLE);
    EXPECT_EQ(fast_plot, PropertyFlag::FAST_PLOT);
}

// Complex construction and interaction tests
class PropertyInteractionTest : public ::testing::Test
{
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(PropertyInteractionTest, PropertyTypeConsistency)
{
    // Test that all properties have correct types
    LineWidth lw;
    Alpha alpha;
    ZOffset z_offset;
    Transform transform;
    Label label;
    EdgeColor edge_color;
    FaceColor face_color;
    Silhouette silhouette;
    PointSize point_size;
    DistanceFrom distance_from;
    BufferSize buffer_size;
    ColorInternal color_internal;
    
    EXPECT_EQ(lw.getPropertyType(), PropertyType::LINE_WIDTH);
    EXPECT_EQ(alpha.getPropertyType(), PropertyType::ALPHA);
    EXPECT_EQ(z_offset.getPropertyType(), PropertyType::Z_OFFSET);
    EXPECT_EQ(transform.getPropertyType(), PropertyType::TRANSFORM);
    EXPECT_EQ(label.getPropertyType(), PropertyType::NAME);
    EXPECT_EQ(edge_color.getPropertyType(), PropertyType::EDGE_COLOR);
    EXPECT_EQ(face_color.getPropertyType(), PropertyType::FACE_COLOR);
    EXPECT_EQ(silhouette.getPropertyType(), PropertyType::SILHOUETTE);
    EXPECT_EQ(point_size.getPropertyType(), PropertyType::POINT_SIZE);
    EXPECT_EQ(distance_from.getPropertyType(), PropertyType::DISTANCE_FROM);
    EXPECT_EQ(buffer_size.getPropertyType(), PropertyType::BUFFER_SIZE);
    EXPECT_EQ(color_internal.getPropertyType(), PropertyType::COLOR);
}

TEST_F(PropertyInteractionTest, ColorConversions)
{
    // Test color conversions between different color types
    Color prop_color(ColorT::MAGENTA);
    ColorInternal internal_color(prop_color);
    
    EXPECT_EQ(prop_color.red, internal_color.red);
    EXPECT_EQ(prop_color.green, internal_color.green);
    EXPECT_EQ(prop_color.blue, internal_color.blue);
    
    EdgeColor edge_color(EdgeColorT::CYAN);
    EXPECT_EQ(edge_color.red, 0);
    EXPECT_EQ(edge_color.green, 255);
    EXPECT_EQ(edge_color.blue, 255);
    
    FaceColor face_color(FaceColorT::YELLOW);
    EXPECT_EQ(face_color.red, 255);
    EXPECT_EQ(face_color.green, 255);
    EXPECT_EQ(face_color.blue, 0);
}

TEST_F(PropertyInteractionTest, PolymorphicBehavior)
{
    // Test polymorphic behavior through PropertyBase
    std::vector<std::unique_ptr<PropertyBase>> properties;
    
    properties.push_back(std::make_unique<LineWidth>(5));
    properties.push_back(std::make_unique<Alpha>(0.8f));
    properties.push_back(std::make_unique<ZOffset>(1.5f));
    
    EXPECT_EQ(properties[0]->getPropertyType(), PropertyType::LINE_WIDTH);
    EXPECT_EQ(properties[1]->getPropertyType(), PropertyType::ALPHA);
    EXPECT_EQ(properties[2]->getPropertyType(), PropertyType::Z_OFFSET);
}

TEST_F(PropertyInteractionTest, LabelEdgeCases)
{
    // Test label with special characters
    Label special_label("test\ttab\nnewline");
    EXPECT_GT(special_label.length, 0U);
    
    // Test label equality with different strings
    Label label1("identical");
    Label label2("identical");
    Label label3("different");
    
    EXPECT_TRUE(label1 == label2);
    EXPECT_FALSE(label1 == label3);
    
    // Test label reset functionality
    label1.resetData();
    EXPECT_EQ(label1.length, 0U);
    EXPECT_FALSE(label1 == label2); // Should no longer be equal
}