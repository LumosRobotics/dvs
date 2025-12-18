#ifndef USER_SUPPLIED_PROPERTIES_H
#define USER_SUPPLIED_PROPERTIES_H


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <map>
#include <memory>
#include <string>
#include <string_view>
#include <vector>
#include <optional>

#include "axes/legend_properties.h"
#include "color_picker.h"
#include "communication/received_data.h"
#include "duoplot/enumerations.h"
#include "duoplot/math/math.h"
#include "duoplot/plot_properties.h"
#include "misc/rgb_triplet.h"
#include "opengl_low_level/opengl_header.h"
#include "plot_objects/utils.h"
#include "duoplot/internal.h"
#include "shader.h"

using namespace duoplot;
using namespace duoplot::internal;
using namespace duoplot::properties;


constexpr size_t kDefaultBufferSize = 500U;
constexpr char* const kDefaultLabel = "";
constexpr RGBTripletf kDefaultEdgeColor{0.0f, 0.0f, 0.0f};
constexpr RGBTripletf kDefaultSilhouette{0.0f, 0.0f, 0.0f};
constexpr bool kDefaultHasSilhouette{false};
constexpr float kDefaultSilhouettePercentage{0.01f};
constexpr float kDefaultZOffset{0.0f};
constexpr ScatterStyle kDefaultScatterStyle{ScatterStyle::CIRCLE};
constexpr LineStyle kDefaultLineStyle{LineStyle::SOLID};
constexpr float kDefaultAlpha{1.0f};
constexpr float kDefaultLineWidth{2.0f};
constexpr float kDefaultPointSize{10.0f};
constexpr bool kDefaultIsPersistent{false};
constexpr bool kDefaultIsAppendable{false};
constexpr bool kDefaultExcludeFromSelection{false};
constexpr bool kDefaultIsUpdateable{false};
constexpr bool kDefaultInterpolateColormap{false};
constexpr ItemId kDefaultId{ItemId::UNKNOWN};
constexpr GLenum kDefaultDynamicOrStaticUsage{GL_STATIC_DRAW};
constexpr bool kDefaultNoEdges{false};
constexpr bool kDefaultNoFaces{false};

template <typename T> duoplot::internal::PropertyType templateToPropertyType()
{
    if (std::is_same<T, duoplot::properties::Alpha>::value)
    {
        return duoplot::internal::PropertyType::ALPHA;
    }
    else if (std::is_same<T, duoplot::properties::Label>::value)
    {
        return duoplot::internal::PropertyType::NAME;
    }
    else if (std::is_same<T, duoplot::properties::LineWidth>::value)
    {
        return duoplot::internal::PropertyType::LINE_WIDTH;
    }
    else if (std::is_same<T, duoplot::properties::LineStyle>::value)
    {
        return duoplot::internal::PropertyType::LINE_STYLE;
    }
    else if (std::is_same<T, duoplot::internal::ColorInternal>::value)
    {
        return duoplot::internal::PropertyType::COLOR;
    }
    else if (std::is_same<T, duoplot::properties::EdgeColor>::value)
    {
        return duoplot::internal::PropertyType::EDGE_COLOR;
    }
    else if (std::is_same<T, duoplot::properties::FaceColor>::value)
    {
        return duoplot::internal::PropertyType::FACE_COLOR;
    }
    else if (std::is_same<T, duoplot::properties::ColorMap>::value)
    {
        return duoplot::internal::PropertyType::COLOR_MAP;
    }
    else if (std::is_same<T, duoplot::properties::ScatterStyle>::value)
    {
        return duoplot::internal::PropertyType::SCATTER_STYLE;
    }
    else if (std::is_same<T, duoplot::properties::PointSize>::value)
    {
        return duoplot::internal::PropertyType::POINT_SIZE;
    }
    else if (std::is_same<T, duoplot::properties::BufferSize>::value)
    {
        return duoplot::internal::PropertyType::BUFFER_SIZE;
    }
    else if (std::is_same<T, duoplot::properties::DistanceFrom>::value)
    {
        return duoplot::internal::PropertyType::DISTANCE_FROM;
    }
    else if (std::is_same<T, duoplot::properties::ZOffset>::value)
    {
        return duoplot::internal::PropertyType::Z_OFFSET;
    }
    else if (std::is_same<T, duoplot::properties::Transform>::value)
    {
        return duoplot::internal::PropertyType::TRANSFORM;
    }
    else if (std::is_same<T, duoplot::properties::Silhouette>::value)
    {
        return duoplot::internal::PropertyType::SILHOUETTE;
    }
    else
    {
        throw std::runtime_error("Invalid property template!");
    }
}

class UserSuppliedProperties
{
private:
    bool has_properties_;

    duoplot::internal::CommunicationHeader::PropertiesArray props_;
    duoplot::internal::PropertyLookupTable props_lut_;
    duoplot::internal::CommunicationHeader::FlagsArray flags_;

    template <typename T>
    void overwritePropertyFromOtherIfPresent(std::optional<T>& local, const std::optional<T> other)
    {
        if (other.has_value())
        {
            local = other.value();
            has_properties_ = true;
        }
    }

    size_t numProperties() const
    {
        return props_.usedSize();
    }

public:

    UserSuppliedProperties();
    UserSuppliedProperties(const CommunicationHeader& hdr);
    void appendProperties(const UserSuppliedProperties& props);
    bool hasProperties() const;
    void clear();

    bool hasProperty(const duoplot::internal::PropertyType tp) const;
    bool hasFlag(const duoplot::internal::PropertyFlag f) const;

    template <typename T> T getProperty() const
    {
        const duoplot::internal::PropertyType tp = templateToPropertyType<T>();

        const uint8_t idx = props_lut_.data[static_cast<uint8_t>(tp)];

        if (idx == 255U)
        {
            throw std::runtime_error("Property not present!");
        }

        return props_[idx].as<T>();
    }

    bool isEmpty() const
    {
        if (props_.usedSize() > 0)
        {
            return false;
        }

        for (size_t k = 0; k < flags_.size(); k++)
        {
            if (flags_[k] == static_cast<uint8_t>(1U))
            {
                return false;
            }
        }

        return true;
    }


    // Properties
    std::optional<std::string> label{std::nullopt};

    std::optional<ScatterStyle> scatter_style{std::nullopt};
    std::optional<LineStyle> line_style{std::nullopt};

    std::optional<float> z_offset{std::nullopt};
    std::optional<float> alpha{std::nullopt};
    std::optional<float> line_width{std::nullopt};
    std::optional<float> point_size{std::nullopt};

    std::optional<uint16_t> buffer_size{std::nullopt};

    std::optional<RGBTripletf> color{std::nullopt};

    std::optional<RGBTripletf> edge_color{std::nullopt};
    bool no_edges{kDefaultNoEdges};

    std::optional<RGBTripletf> face_color;
    bool no_faces{kDefaultNoFaces};

    std::optional<RGBTripletf> silhouette{kDefaultSilhouette};
    float silhouette_percentage{kDefaultSilhouettePercentage};
    bool has_silhouette;

    std::optional<ColorMap> color_map;

    std::optional<DistanceFrom> distance_from;

    std::optional<Transform> custom_transform;

    bool is_persistent{kDefaultIsPersistent};
    bool is_updateable{kDefaultIsUpdateable};
    bool is_appendable{kDefaultIsAppendable};
    bool exclude_from_selection{kDefaultExcludeFromSelection};
    bool interpolate_colormap{kDefaultInterpolateColormap};
    GLenum dynamic_or_static_usage{kDefaultDynamicOrStaticUsage};
};

#endif // USER_SUPPLIED_PROPERTIES_H
