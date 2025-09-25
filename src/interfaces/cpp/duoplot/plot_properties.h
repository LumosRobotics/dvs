#ifndef DUOPLOT_PLOT_PROPERTIES_H_
#define DUOPLOT_PLOT_PROPERTIES_H_

#include <cstring>
#include <iostream>

#include "duoplot/enumerations.h"
#include "duoplot/logging.h"
#include "duoplot/math/math.h"

namespace duoplot
{
namespace internal
{
class PropertyBase
{
protected:
    PropertyType property_type_;

public:
    PropertyBase() : property_type_{PropertyType::UNKNOWN} {}

    explicit PropertyBase(const PropertyType property_type) : property_type_{property_type} {}

    PropertyType getPropertyType() const
    {
        return property_type_;
    };

    void setPropertyType(const PropertyType property_type)
    {
        property_type_ = property_type;
    }

    virtual ~PropertyBase() {}
};

inline size_t safeStringLenCheck(const char* const str, const size_t max_length)
{
    // Length does not include the null termination, so 'hello\0' will return 5
    size_t idx = 0;
    while (str[idx] && (idx < max_length))
    {
        idx++;
    }

    return idx;
}

}  // namespace internal

namespace properties
{
struct LineWidth : internal::PropertyBase
{
    uint8_t data;

    LineWidth() : internal::PropertyBase{internal::PropertyType::LINE_WIDTH} {}

    explicit LineWidth(const uint8_t line_width)
        : internal::PropertyBase{internal::PropertyType::LINE_WIDTH}, data{line_width}
    {
    }
};

struct Alpha : internal::PropertyBase
{
    float data;

    Alpha() : internal::PropertyBase{internal::PropertyType::ALPHA} {}

    explicit Alpha(const float alpha) : internal::PropertyBase{internal::PropertyType::ALPHA}, data{alpha} {}
};

struct ZOffset : internal::PropertyBase
{
    float data;

    ZOffset() : internal::PropertyBase{internal::PropertyType::Z_OFFSET} {}

    explicit ZOffset(const float z_offset) : internal::PropertyBase{internal::PropertyType::Z_OFFSET}, data{z_offset} {}
};

struct Transform : internal::PropertyBase
{
    MatrixFixed<double, 3, 3> scale;
    MatrixFixed<double, 3, 3> rotation;
    Vec3<double> translation;

    Transform() : internal::PropertyBase{internal::PropertyType::TRANSFORM} {}

    explicit Transform(const MatrixFixed<double, 3, 3>& scale_,
                       const MatrixFixed<double, 3, 3>& rotation_,
                       const Vec3<double>& translation_)
        : internal::PropertyBase{internal::PropertyType::TRANSFORM},
          scale{scale_},
          rotation{rotation_},
          translation{translation_}
    {
    }

    explicit Transform(const Matrix<double>& scale_, const Matrix<double>& rotation_, const Vec3<double>& translation_)
        : internal::PropertyBase{internal::PropertyType::TRANSFORM}
    {
        translation = translation_;

        DUOPLOT_ASSERT(rotation_.numRows() == 3U) << "Expected 3 rows for rotation matrix, got " << rotation_.numRows();
        DUOPLOT_ASSERT(rotation_.numCols() == 3U) << "Expected 3 cols for rotation matrix, got " << rotation_.numCols();

        DUOPLOT_ASSERT(scale_.numRows() == 3U) << "Expected 3 rows for scale matrix, got " << scale_.numRows();
        DUOPLOT_ASSERT(scale_.numCols() == 3U) << "Expected 3 cols for scale matrix, got " << scale_.numCols();

        for (size_t r = 0; r < 3; r++)
        {
            for (size_t c = 0; c < 3; c++)
            {
                rotation(r, c) = rotation_(r, c);
                scale(r, c) = scale_(r, c);
            }
        }
    }
};

class Label : public internal::PropertyBase
{
private:
    // Number of characters that the name can contain (excluding null termination)
    static constexpr size_t kMaxLength = 100U;
    static constexpr size_t kDataFullLength = kMaxLength + 1U;  // +1 for null termination

public:
    uint8_t length;
    char data[kDataFullLength];

    Label() : internal::PropertyBase{internal::PropertyType::NAME}
    {
        data[0] = '\0';
        length = 0;
    }

    explicit Label(const char* const name) : internal::PropertyBase{internal::PropertyType::NAME}
    {
        DUOPLOT_ASSERT(name) << "Input name string is null!";
        const size_t len = internal::safeStringLenCheck(name, kMaxLength);

        DUOPLOT_ASSERT(len <= kMaxLength) << "Label can't be more than 100 characters!";
        length = len;

        std::memcpy(data, name, len);
        data[len] = '\0';
    }

    void resetData()
    {
        std::memset(data, 0, kDataFullLength);
        length = 0;
    }
};

inline bool operator==(const Label& l0, const Label& l1)
{
    return strcmp(l0.data, l1.data) == 0;
}

enum class LineStyle : uint8_t
{
    SOLID,
    DASHED,
    SHORT_DASHED,
    LONG_DASHED
};

enum class ScatterStyle : uint8_t
{
    SQUARE,
    CIRCLE,
    DISC,
    PLUS,
    CROSS
};

struct Color
{
    uint8_t red, green, blue;

    Color() {}

    explicit Color(const uint8_t red_, const uint8_t green_, const uint8_t blue_)
        : red{red_}, green{green_}, blue{blue_}
    {
    }

    Color(const internal::ColorT color)
    {
        Color c{};

        switch (color)
        {
            case internal::ColorT::RED:
                c = Color{255, 0, 0};
                break;
            case internal::ColorT::GREEN:
                c = Color{0, 255, 0};
                break;
            case internal::ColorT::BLUE:
                c = Color{0, 0, 255};
                break;
            case internal::ColorT::CYAN:
                c = Color{0, 255, 255};
                break;
            case internal::ColorT::MAGENTA:
                c = Color{255, 0, 255};
                break;
            case internal::ColorT::YELLOW:
                c = Color{255, 255, 0};
                break;
            case internal::ColorT::BLACK:
                c = Color{0, 0, 0};
                break;
            case internal::ColorT::WHITE:
                c = Color{255, 255, 255};
                break;
            case internal::ColorT::GRAY:
                c = Color{127, 127, 127};
                break;
        }

        red = c.red;
        green = c.green;
        blue = c.blue;
    }

    Color& operator=(const internal::ColorT color)
    {
        Color c{color};

        red = c.red;
        green = c.green;
        blue = c.blue;

        return *this;
    }

    static constexpr internal::ColorT RED = internal::ColorT::RED;
    static constexpr internal::ColorT GREEN = internal::ColorT::GREEN;
    static constexpr internal::ColorT BLUE = internal::ColorT::BLUE;
    static constexpr internal::ColorT CYAN = internal::ColorT::CYAN;
    static constexpr internal::ColorT MAGENTA = internal::ColorT::MAGENTA;
    static constexpr internal::ColorT YELLOW = internal::ColorT::YELLOW;
    static constexpr internal::ColorT BLACK = internal::ColorT::BLACK;
    static constexpr internal::ColorT WHITE = internal::ColorT::WHITE;
    static constexpr internal::ColorT GRAY = internal::ColorT::GRAY;
};

inline bool operator==(const Color& lhs, const Color& rhs)
{
    return (lhs.red == rhs.red) && (lhs.green == rhs.green) && (lhs.blue == rhs.blue);
}

inline bool operator!=(const Color& lhs, const Color& rhs)
{
    return !(lhs == rhs);
}

struct EdgeColor : internal::PropertyBase
{
    uint8_t use_color;
    uint8_t red, green, blue;

    EdgeColor() : internal::PropertyBase{internal::PropertyType::EDGE_COLOR}, use_color{1U} {}

    explicit EdgeColor(const uint8_t red_, const uint8_t green_, const uint8_t blue_)
        : internal::PropertyBase(internal::PropertyType::EDGE_COLOR),
          use_color{1U},
          red{red_},
          green{green_},
          blue{blue_}
    {
    }

    explicit EdgeColor(const uint8_t use_color_)
        : internal::PropertyBase(internal::PropertyType::EDGE_COLOR),
          use_color{use_color_},
          red{0U},
          green{0U},
          blue{0U}
    {
    }

    EdgeColor(const internal::EdgeColorT color)
        : internal::PropertyBase{internal::PropertyType::EDGE_COLOR}, use_color{1U}
    {
        EdgeColor ec{};

        switch (color)
        {
            case internal::EdgeColorT::RED:
                ec = EdgeColor{255, 0, 0};
                break;
            case internal::EdgeColorT::GREEN:
                ec = EdgeColor{0, 255, 0};
                break;
            case internal::EdgeColorT::BLUE:
                ec = EdgeColor{0, 0, 255};
                break;
            case internal::EdgeColorT::CYAN:
                ec = EdgeColor{0, 255, 255};
                break;
            case internal::EdgeColorT::MAGENTA:
                ec = EdgeColor{255, 0, 255};
                break;
            case internal::EdgeColorT::YELLOW:
                ec = EdgeColor{255, 255, 0};
                break;
            case internal::EdgeColorT::BLACK:
                ec = EdgeColor{0, 0, 0};
                break;
            case internal::EdgeColorT::WHITE:
                ec = EdgeColor{255, 255, 255};
                break;
            case internal::EdgeColorT::GRAY:
                ec = EdgeColor{127, 127, 127};
                break;
            case internal::EdgeColorT::NONE:
                ec = EdgeColor{0U};
                break;
        }

        red = ec.red;
        green = ec.green;
        blue = ec.blue;
        use_color = ec.use_color;
    }

    static constexpr internal::EdgeColorT RED = internal::EdgeColorT::RED;
    static constexpr internal::EdgeColorT GREEN = internal::EdgeColorT::GREEN;
    static constexpr internal::EdgeColorT BLUE = internal::EdgeColorT::BLUE;
    static constexpr internal::EdgeColorT CYAN = internal::EdgeColorT::CYAN;
    static constexpr internal::EdgeColorT MAGENTA = internal::EdgeColorT::MAGENTA;
    static constexpr internal::EdgeColorT YELLOW = internal::EdgeColorT::YELLOW;
    static constexpr internal::EdgeColorT BLACK = internal::EdgeColorT::BLACK;
    static constexpr internal::EdgeColorT WHITE = internal::EdgeColorT::WHITE;
    static constexpr internal::EdgeColorT GRAY = internal::EdgeColorT::GRAY;
    static constexpr internal::EdgeColorT NONE = internal::EdgeColorT::NONE;
};

struct FaceColor : internal::PropertyBase
{
    uint8_t use_color;
    uint8_t red, green, blue;

    FaceColor() : internal::PropertyBase{internal::PropertyType::FACE_COLOR}, use_color{1U}, red(0), green(0), blue(0)
    {
    }

    explicit FaceColor(const uint8_t red_, const uint8_t green_, const uint8_t blue_)
        : internal::PropertyBase(internal::PropertyType::FACE_COLOR),
          use_color{1U},
          red{red_},
          green{green_},
          blue{blue_}
    {
    }

    explicit FaceColor(const uint8_t use_color_)
        : internal::PropertyBase(internal::PropertyType::FACE_COLOR),
          use_color{use_color_},
          red{0U},
          green{0U},
          blue{0U}
    {
    }

    FaceColor(const internal::FaceColorT color)
        : internal::PropertyBase(internal::PropertyType::FACE_COLOR), use_color(1U)
    {
        FaceColor fc{};

        switch (color)
        {
            case internal::FaceColorT::RED:
                fc = FaceColor{255, 0, 0};
                break;
            case internal::FaceColorT::GREEN:
                fc = FaceColor{0, 255, 0};
                break;
            case internal::FaceColorT::BLUE:
                fc = FaceColor{0, 0, 255};
                break;
            case internal::FaceColorT::CYAN:
                fc = FaceColor{0, 255, 255};
                break;
            case internal::FaceColorT::MAGENTA:
                fc = FaceColor{255, 0, 255};
                break;
            case internal::FaceColorT::YELLOW:
                fc = FaceColor{255, 255, 0};
                break;
            case internal::FaceColorT::BLACK:
                fc = FaceColor{0, 0, 0};
                break;
            case internal::FaceColorT::WHITE:
                fc = FaceColor{255, 255, 255};
                break;
            case internal::FaceColorT::GRAY:
                fc = FaceColor{127, 127, 127};
                break;
            case internal::FaceColorT::NONE:
                fc = FaceColor{0U};
                break;
        }

        red = fc.red;
        green = fc.green;
        blue = fc.blue;
        use_color = fc.use_color;
    }

    static constexpr internal::FaceColorT RED = internal::FaceColorT::RED;
    static constexpr internal::FaceColorT GREEN = internal::FaceColorT::GREEN;
    static constexpr internal::FaceColorT BLUE = internal::FaceColorT::BLUE;
    static constexpr internal::FaceColorT CYAN = internal::FaceColorT::CYAN;
    static constexpr internal::FaceColorT MAGENTA = internal::FaceColorT::MAGENTA;
    static constexpr internal::FaceColorT YELLOW = internal::FaceColorT::YELLOW;
    static constexpr internal::FaceColorT BLACK = internal::FaceColorT::BLACK;
    static constexpr internal::FaceColorT WHITE = internal::FaceColorT::WHITE;
    static constexpr internal::FaceColorT GRAY = internal::FaceColorT::GRAY;
    static constexpr internal::FaceColorT NONE = internal::FaceColorT::NONE;
};

struct Silhouette : internal::PropertyBase
{
    uint8_t red, green, blue;
    float percentage;

    Silhouette() : internal::PropertyBase{internal::PropertyType::SILHOUETTE} {}

    explicit Silhouette(const uint8_t red_, const uint8_t green_, const uint8_t blue_)
        : internal::PropertyBase{internal::PropertyType::SILHOUETTE},
          red{red_},
          green{green_},
          blue{blue_},
          percentage{0.1f}
    {
    }

    // Percentage is a number in the range [0.0, 1.0]
    explicit Silhouette(const uint8_t red_, const uint8_t green_, const uint8_t blue_, const float percentage_)
        : internal::PropertyBase{internal::PropertyType::SILHOUETTE},
          red{red_},
          green{green_},
          blue{blue_},
          percentage{percentage_}
    {
    }

    Silhouette(const internal::SilhouetteT silhouette)
    {
        Silhouette s{};

        switch (silhouette)
        {
            case internal::SilhouetteT::RED:
                s = Silhouette{255, 0, 0};
                break;
            case internal::SilhouetteT::GREEN:
                s = Silhouette{0, 255, 0};
                break;
            case internal::SilhouetteT::BLUE:
                s = Silhouette{0, 0, 255};
                break;
            case internal::SilhouetteT::CYAN:
                s = Silhouette{0, 255, 255};
                break;
            case internal::SilhouetteT::MAGENTA:
                s = Silhouette{255, 0, 255};
                break;
            case internal::SilhouetteT::YELLOW:
                s = Silhouette{255, 255, 0};
                break;
            case internal::SilhouetteT::BLACK:
                s = Silhouette{0, 0, 0};
                break;
            case internal::SilhouetteT::WHITE:
                s = Silhouette{255, 255, 255};
                break;
            case internal::SilhouetteT::GRAY:
                s = Silhouette{127, 127, 127};
                break;
        }

        red = s.red;
        green = s.green;
        blue = s.blue;
    }

    Silhouette& operator=(const internal::SilhouetteT silhouette)
    {
        Silhouette s{silhouette};

        red = s.red;
        green = s.green;
        blue = s.blue;

        percentage = s.percentage;

        return *this;
    }

    static constexpr internal::SilhouetteT RED = internal::SilhouetteT::RED;
    static constexpr internal::SilhouetteT GREEN = internal::SilhouetteT::GREEN;
    static constexpr internal::SilhouetteT BLUE = internal::SilhouetteT::BLUE;
    static constexpr internal::SilhouetteT CYAN = internal::SilhouetteT::CYAN;
    static constexpr internal::SilhouetteT MAGENTA = internal::SilhouetteT::MAGENTA;
    static constexpr internal::SilhouetteT YELLOW = internal::SilhouetteT::YELLOW;
    static constexpr internal::SilhouetteT BLACK = internal::SilhouetteT::BLACK;
    static constexpr internal::SilhouetteT WHITE = internal::SilhouetteT::WHITE;
    static constexpr internal::SilhouetteT GRAY = internal::SilhouetteT::GRAY;
};

enum class ColorMap : uint8_t
{
    JET,
    HSV,
    MAGMA,
    VIRIDIS,
    PASTEL,
    JET_SOFT,
    JET_BRIGHT,
    UNKNOWN
};

struct PointSize : internal::PropertyBase
{
    uint8_t data;

    PointSize() : internal::PropertyBase{internal::PropertyType::POINT_SIZE} {}

    explicit PointSize(const uint8_t point_size)
        : internal::PropertyBase{internal::PropertyType::POINT_SIZE}, data{point_size}
    {
    }
};

class DistanceFrom : public internal::PropertyBase
{
private:
    Vec3<double> pt_;
    double min_dist_;
    double max_dist_;
    DistanceFromType dist_from_type_;

    DistanceFrom(const Vec3<double>& pt,
                 const double min_dist,
                 const double max_dist,
                 const DistanceFromType dist_from_type)
        : internal::PropertyBase{internal::PropertyType::DISTANCE_FROM},
          pt_{pt},
          min_dist_{min_dist},
          max_dist_{max_dist},
          dist_from_type_{dist_from_type}
    {
    }

public:
    DistanceFrom() : internal::PropertyBase{internal::PropertyType::DISTANCE_FROM} {}

    Vec3<double> getPoint() const
    {
        return pt_;
    }

    double getMinDist() const
    {
        return min_dist_;
    }

    double getMaxDist() const
    {
        return max_dist_;
    }

    DistanceFromType getDistFromType() const
    {
        return dist_from_type_;
    }

    void setPoint(const Vec3<double>& pt)
    {
        pt_ = pt;
    }

    void setMinDist(const double min_dist)
    {
        min_dist_ = min_dist;
    }

    void setMaxDist(const double max_dist)
    {
        max_dist_ = max_dist;
    }

    void setDistFromType(const DistanceFromType dist_from_type)
    {
        dist_from_type_ = dist_from_type;
    }

    static DistanceFrom x(const double x_val, const double min_dist, const double max_dist)
    {
        return DistanceFrom(Vec3<double>{x_val, 0.0, 0.0}, min_dist, max_dist, DistanceFromType::X);
    }

    static DistanceFrom y(const double y_val, const double min_dist, const double max_dist)
    {
        return DistanceFrom(Vec3<double>{0.0, y_val, 0.0}, min_dist, max_dist, DistanceFromType::Y);
    }

    static DistanceFrom z(const double z_val, const double min_dist, const double max_dist)
    {
        return DistanceFrom(Vec3<double>{0.0, 0.0, z_val}, min_dist, max_dist, DistanceFromType::Z);
    }

    static DistanceFrom xy(const PointXY<double>& p, const double min_dist, const double max_dist)
    {
        return DistanceFrom(Vec3<double>{p.x, p.y, 0.0}, min_dist, max_dist, DistanceFromType::XY);
    }

    static DistanceFrom xz(const PointXZ<double>& p, const double min_dist, const double max_dist)
    {
        return DistanceFrom(Vec3<double>{p.x, 0.0, p.z}, min_dist, max_dist, DistanceFromType::XZ);
    }

    static DistanceFrom yz(const PointYZ<double>& p, const double min_dist, const double max_dist)
    {
        return DistanceFrom(Vec3<double>{0.0, p.y, p.z}, min_dist, max_dist, DistanceFromType::YZ);
    }

    static DistanceFrom xyz(const Point3<double>& p, const double min_dist, const double max_dist)
    {
        return DistanceFrom(p, min_dist, max_dist, DistanceFromType::XYZ);
    }
};

struct BufferSize : internal::PropertyBase
{
    uint16_t data;

    BufferSize() : internal::PropertyBase{internal::PropertyType::BUFFER_SIZE} {}

    explicit BufferSize(const uint16_t buffer_size)
        : internal::PropertyBase{internal::PropertyType::BUFFER_SIZE}, data{buffer_size}
    {
    }
};

constexpr internal::PropertyFlag PERSISTENT = internal::PropertyFlag::PERSISTENT;
constexpr internal::PropertyFlag INTERPOLATE_COLORMAP = internal::PropertyFlag::INTERPOLATE_COLORMAP;
constexpr internal::PropertyFlag EXCLUDE_FROM_SELECTION = internal::PropertyFlag::EXCLUDE_FROM_SELECTION;
namespace not_ready
{
constexpr internal::PropertyFlag APPENDABLE = internal::PropertyFlag::APPENDABLE;
constexpr internal::PropertyFlag UPDATABLE = internal::PropertyFlag::UPDATABLE;
constexpr internal::PropertyFlag SELECTABLE = internal::PropertyFlag::SELECTABLE;
constexpr internal::PropertyFlag FAST_PLOT = internal::PropertyFlag::FAST_PLOT;
}  // namespace not_ready

}  // namespace properties

namespace internal
{

struct ColorInternal : PropertyBase
{
    uint8_t red, green, blue;

    ColorInternal() : PropertyBase{PropertyType::COLOR} {}
    ColorInternal(const properties::Color& col)
        : PropertyBase{PropertyType::COLOR}, red{col.red}, green{col.green}, blue{col.blue}
    {
    }

    explicit ColorInternal(const uint8_t red_, const uint8_t green_, const uint8_t blue_)
        : internal::PropertyBase{internal::PropertyType::COLOR}, red{red_}, green{green_}, blue{blue_}
    {
    }

    ColorInternal(const internal::ColorT color) : internal::PropertyBase{internal::PropertyType::COLOR}
    {
        ColorInternal c{};

        switch (color)
        {
            case internal::ColorT::RED:
                c = ColorInternal{255, 0, 0};
                break;
            case internal::ColorT::GREEN:
                c = ColorInternal{0, 255, 0};
                break;
            case internal::ColorT::BLUE:
                c = ColorInternal{0, 0, 255};
                break;
            case internal::ColorT::CYAN:
                c = ColorInternal{0, 255, 255};
                break;
            case internal::ColorT::MAGENTA:
                c = ColorInternal{255, 0, 255};
                break;
            case internal::ColorT::YELLOW:
                c = ColorInternal{255, 255, 0};
                break;
            case internal::ColorT::BLACK:
                c = ColorInternal{0, 0, 0};
                break;
            case internal::ColorT::WHITE:
                c = ColorInternal{255, 255, 255};
                break;
            case internal::ColorT::GRAY:
                c = ColorInternal{127, 127, 127};
                break;
        }

        red = c.red;
        green = c.green;
        blue = c.blue;
    }
};

struct Dimension2D
{
    uint32_t rows;
    uint32_t cols;

    Dimension2D() : rows(0), cols(0) {}
    Dimension2D(const size_t r, const size_t c) : rows(r), cols(c) {}
};

}  // namespace internal

}  // namespace duoplot

#endif  // DUOPLOT_PLOT_PROPERTIES_H_
