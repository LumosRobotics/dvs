#ifndef DUOPLOT_CONTROL_FUNCTIONS_H
#define DUOPLOT_CONTROL_FUNCTIONS_H

#include "duoplot/internal.h"
#include "duoplot/math/math.h"

namespace duoplot
{

template <typename... Us> void setProperties(const ItemId id, const Us&... settings)
{
    internal::CommunicationHeader hdr{internal::Function::PROPERTIES_EXTENSION};
    hdr.append(internal::CommunicationHeaderObjectType::ITEM_ID, id);
    hdr.extend(settings...);

    if (hdr.hasPropertyFlag(internal::PropertyFlag::APPENDABLE))
    {
        DUOPLOT_LOG_WARNING()
            << "Flag APPENDABLE is not allowed for setProperties()! Set in function call instead, e.g. "
               "scatter(x, y, properties::APPENDABLE).";
        return;
    }

    if (hdr.hasPropertyFlag(internal::PropertyFlag::FAST_PLOT))
    {
        DUOPLOT_LOG_WARNING()
            << "Flag FAST_PLOT is not allowed for setProperties()! Set in function call instead, e.g. "
               "plot(x, y, properties::FAST_PLOT).";
        return;
    }

    internal::sendHeaderOnly(internal::getSendFunction(), hdr);
}

template <typename... Us> void setProperties(const std::vector<PropertySet>& property_sets)
{
    internal::CommunicationHeader hdr{internal::Function::PROPERTIES_EXTENSION_MULTIPLE};

    std::size_t total_size{0U};
    for (size_t k = 0; k < property_sets.size(); k++)
    {
        total_size += property_sets[k].getTotalSize();
    }

    // +1 for number of property sets
    Vector<std::uint8_t> data_vec(total_size + 1U);
    data_vec(0) = static_cast<std::uint8_t>(property_sets.size());

    std::size_t idx{1U};
    for (size_t k = 0; k < property_sets.size(); k++)
    {
        property_sets[k].fillBuffer(data_vec.data() + idx);

        idx += property_sets[k].getTotalSize();
    }

    internal::sendHeaderAndData(internal::getSendFunction(), hdr, data_vec);
}

inline void setActiveView(const std::string& name)
{
    if (name.length() == 0)
    {
        DUOPLOT_LOG_ERROR() << "Cannot set element name with string length 0!";
        return;
    }

    internal::CommunicationHeader hdr{internal::Function::SET_CURRENT_ELEMENT};
    hdr.append(internal::CommunicationHeaderObjectType::ELEMENT_NAME, properties::Label(name.c_str()));

    internal::sendHeaderOnly(internal::getSendFunction(), hdr);
}

inline void deletePlotObject(const ItemId id)
{
    internal::CommunicationHeader hdr{internal::Function::DELETE_PLOT_OBJECT};
    hdr.append(internal::CommunicationHeaderObjectType::ITEM_ID, id);

    internal::sendHeaderOnly(internal::getSendFunction(), hdr);
}

inline void setActiveViewToImageView()
{
    internal::CommunicationHeader hdr{internal::Function::CURRENT_ELEMENT_AS_IMAGE_VIEW};

    internal::sendHeaderOnly(internal::getSendFunction(), hdr);
}

inline void waitForFlush()
{
    internal::CommunicationHeader hdr{internal::Function::WAIT_FOR_FLUSH};

    internal::sendHeaderOnly(internal::getSendFunction(), hdr);
}

inline void flushCurrentElement()
{
    internal::CommunicationHeader hdr{internal::Function::FLUSH_ELEMENT};

    internal::sendHeaderOnly(internal::getSendFunction(), hdr);
}

namespace internal
{

inline void flushMultipleElementsInternal(std::vector<std::string>& extracted_elements, const std::string& elem)
{
    extracted_elements.push_back(elem);
}

template <typename... Us>
void flushMultipleElementsInternal(std::vector<std::string>& extracted_elements,
                                   const std::string& elem,
                                   const Us&... elements)
{
    extracted_elements.push_back(elem);

    internal::flushMultipleElementsInternal(extracted_elements, elements...);
}

}  // namespace internal

template <typename... Us> void flushMultipleElements(const Us&... elements)
{
    internal::CommunicationHeader hdr{internal::Function::FLUSH_MULTIPLE_ELEMENTS};

    std::vector<std::string> extracted_elements;

    internal::flushMultipleElementsInternal(extracted_elements, elements...);

    Vector<uint8_t> name_lengths{extracted_elements.size()};

    std::vector<char> concatenated_names;

    for (size_t k = 0; k < extracted_elements.size(); k++)
    {
        name_lengths(k) = static_cast<uint8_t>(extracted_elements[k].length());
        for (size_t i = 0; i < extracted_elements[k].length(); i++)
        {
            concatenated_names.push_back(extracted_elements[k][i]);
        }
    }

    Vector<char> concatenated_names_local = concatenated_names;

    hdr.append(internal::CommunicationHeaderObjectType::NUM_NAMES, internal::toUInt8(extracted_elements.size()));

    internal::sendHeaderAndData(internal::getSendFunction(), hdr, name_lengths, concatenated_names_local);
}

inline void viewAngles(const float azimuth_deg, const float elevation_deg)
{
    internal::CommunicationHeader hdr{internal::Function::VIEW};
    hdr.append(internal::CommunicationHeaderObjectType::AZIMUTH, azimuth_deg);
    hdr.append(internal::CommunicationHeaderObjectType::ELEVATION, elevation_deg);

    internal::sendHeaderOnly(internal::getSendFunction(), hdr);
}

inline void axis(const Vec3<double>& min_bound, const Vec3<double>& max_bound)
{
    internal::CommunicationHeader hdr{internal::Function::AXES_3D};
    hdr.append(internal::CommunicationHeaderObjectType::AXIS_MIN_MAX_VEC,
               std::pair<Vec3<double>, Vec3<double>>(min_bound, max_bound));

    internal::sendHeaderOnly(internal::getSendFunction(), hdr);
}

inline void axis(const Vec2<double>& min_bound, const Vec2<double>& max_bound)
{
    const Vec3<double> min_bound_3d(min_bound.x, min_bound.y, -1.0);
    const Vec3<double> max_bound_3d(max_bound.x, max_bound.y, 1.0);

    internal::CommunicationHeader hdr{internal::Function::AXES_2D};
    hdr.append(internal::CommunicationHeaderObjectType::AXIS_MIN_MAX_VEC,
               std::pair<Vec3<double>, Vec3<double>>(min_bound_3d, max_bound_3d));

    internal::sendHeaderOnly(internal::getSendFunction(), hdr);
}

inline void globalIllumination(const Vec3<double>& light_position)
{
    internal::CommunicationHeader hdr{internal::Function::GLOBAL_ILLUMINATION};
    hdr.append(internal::CommunicationHeaderObjectType::VEC3, light_position);

    internal::sendHeaderOnly(internal::getSendFunction(), hdr);
}

inline void showLegend()
{
    internal::CommunicationHeader hdr{internal::Function::SHOW_LEGEND};

    internal::sendHeaderOnly(internal::getSendFunction(), hdr);
}

inline void clearViewOnUpdate()
{
    internal::CommunicationHeader hdr{internal::Function::SOFT_CLEAR};

    internal::sendHeaderOnly(internal::getSendFunction(), hdr);
}

inline void clearView()
{
    internal::CommunicationHeader hdr{internal::Function::CLEAR};

    internal::sendHeaderOnly(internal::getSendFunction(), hdr);
}

inline void disableAutomaticAxesAdjustment()
{
    internal::CommunicationHeader hdr{internal::Function::DISABLE_AXES_FROM_MIN_MAX};

    internal::sendHeaderOnly(internal::getSendFunction(), hdr);
}

inline void disableScaleOnRotation()
{
    internal::CommunicationHeader hdr{internal::Function::DISABLE_SCALE_ON_ROTATION};

    internal::sendHeaderOnly(internal::getSendFunction(), hdr);
}

inline void axesSquare()
{
    internal::CommunicationHeader hdr{internal::Function::AXES_SQUARE};

    internal::sendHeaderOnly(internal::getSendFunction(), hdr);
}

inline void setAxesBoxScaleFactor(const Vec3<double>& scale_vector)
{
    internal::CommunicationHeader hdr{internal::Function::SET_AXES_BOX_SCALE_FACTOR};
    hdr.append(internal::CommunicationHeaderObjectType::VEC3, scale_vector);

    internal::sendHeaderOnly(internal::getSendFunction(), hdr);
}

inline void setTitle(const std::string& title)
{
    if (title.length() == 0)
    {
        DUOPLOT_LOG_ERROR() << "Cannot set title with string length 0!";
        return;
    }

    internal::CommunicationHeader hdr{internal::Function::SET_TITLE};
    hdr.append(internal::CommunicationHeaderObjectType::TITLE_STRING, properties::Label(title.c_str()));

    internal::sendHeaderOnly(internal::getSendFunction(), hdr);
}

inline void setTransform(const ItemId id,
                         const Matrix<double>& scale,
                         const Matrix<double>& rotation,
                         const Vec3<double>& translation)
{
    DUOPLOT_ASSERT(rotation.numRows() == 3) << "Number of rows should be 3!";
    DUOPLOT_ASSERT(rotation.numCols() == 3) << "Number of columns should be 3!";

    DUOPLOT_ASSERT(scale.numRows() == 3) << "Number of rows should be 3!";
    DUOPLOT_ASSERT(scale.numCols() == 3) << "Number of columns should be 3!";

    MatrixFixed<double, 3, 3> r_mat, scale_mat;

    for (size_t r = 0; r < 3; r++)
    {
        for (size_t c = 0; c < 3; c++)
        {
            r_mat(r, c) = rotation(r, c);
            scale_mat(r, c) = scale(r, c);
        }
    }

    internal::CommunicationHeader hdr{internal::Function::SET_OBJECT_TRANSFORM};
    hdr.append(internal::CommunicationHeaderObjectType::ROTATION_MATRIX, r_mat);
    hdr.append(internal::CommunicationHeaderObjectType::TRANSLATION_VECTOR, translation);
    hdr.append(internal::CommunicationHeaderObjectType::SCALE_MATRIX, scale_mat);
    hdr.append(internal::CommunicationHeaderObjectType::ITEM_ID, id);

    internal::sendHeaderOnly(internal::getSendFunction(), hdr);
}

inline void openProjectFile(const std::string& file_path)
{
    internal::CommunicationHeader hdr{internal::Function::OPEN_PROJECT_FILE};
    hdr.append(internal::CommunicationHeaderObjectType::PROJECT_FILE_NAME, properties::Label(file_path.c_str()));

    internal::sendHeaderOnly(internal::getSendFunction(), hdr);
}

// inline void screenshot(const std::string& base_path, const std::string& filename)
inline void screenshot(const std::string& base_path)
{
    internal::CommunicationHeader hdr{internal::Function::SCREENSHOT};
    hdr.append(internal::CommunicationHeaderObjectType::SCREENSHOT_BASE_PATH, properties::Label(base_path.c_str()));

    internal::sendHeaderOnly(internal::getSendFunction(), hdr);
}

inline void setTransform(const ItemId id,
                         const MatrixFixed<double, 3, 3>& scale,
                         const MatrixFixed<double, 3, 3>& rotation,
                         const Vec3<double>& translation)
{
    internal::CommunicationHeader hdr{internal::Function::SET_OBJECT_TRANSFORM};
    hdr.append(internal::CommunicationHeaderObjectType::ROTATION_MATRIX, rotation);
    hdr.append(internal::CommunicationHeaderObjectType::TRANSLATION_VECTOR, translation);
    hdr.append(internal::CommunicationHeaderObjectType::SCALE_MATRIX, scale);
    hdr.append(internal::CommunicationHeaderObjectType::ITEM_ID, id);

    internal::sendHeaderOnly(internal::getSendFunction(), hdr);
}

inline void spawn()
{
    if (!internal::isDuoplotRunning())
    {
        DUOPLOT_LOG_INFO() << "Starting duoplot";
        system("./main_application/duoplot &");
    }
}

}  // namespace duoplot

#endif  // DUOPLOT_CONTROL_FUNCTIONS_H
