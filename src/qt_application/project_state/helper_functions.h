#ifndef PROJECT_STATE_HELPER_FUNCTIONS_H
#define PROJECT_STATE_HELPER_FUNCTIONS_H

#include <fstream>
#include <memory>
#include <nlohmann/json.hpp>
#include <stdexcept>

#include "duoplot/enumerations.h"
#include "duoplot/logging.h"
#include "misc/rgb_triplet.h"

constexpr RGBTriplet<float> hexToRgbTripletf(const std::uint32_t hex_val)
{
    const std::uint8_t r = (hex_val & 0xFF0000) >> 16;
    const std::uint8_t g = (hex_val & 0x00FF00) >> 8;
    const std::uint8_t b = (hex_val & 0x0000FF);

    return RGBTriplet<float>{
        static_cast<float>(r) / 255.0f, static_cast<float>(g) / 255.0f, static_cast<float>(b) / 255.0f};
}

inline duoplot::GuiElementType parseGuiElementType(const nlohmann::json& j)
{
    const std::string type_string = j["type"];

    if (type_string == "BUTTON")
    {
        return duoplot::GuiElementType::Button;
    }
    else if (type_string == "SLIDER")
    {
        return duoplot::GuiElementType::Slider;
    }
    else if (type_string == "CHECKBOX")
    {
        return duoplot::GuiElementType::Checkbox;
    }
    else if (type_string == "EDITABLE_TEXT")
    {
        return duoplot::GuiElementType::EditableText;
    }
    else if (type_string == "DROPDOWN_MENU")
    {
        return duoplot::GuiElementType::DropdownMenu;
    }
    else if (type_string == "LISTBOX")
    {
        return duoplot::GuiElementType::ListBox;
    }
    else if (type_string == "RADIO_BUTTON_GROUP")
    {
        return duoplot::GuiElementType::RadioButtonGroup;
    }
    else if (type_string == "TEXT_LABEL")
    {
        return duoplot::GuiElementType::TextLabel;
    }
    else if (type_string == "STATIC_BOX")
    {
        return duoplot::GuiElementType::StaticBox;
    }
    else if (type_string == "PLOT_PANE")
    {
        return duoplot::GuiElementType::PlotPane;
    }
    else if (type_string == "SCROLLING_TEXT")
    {
        return duoplot::GuiElementType::ScrollingText;
    }
    else
    {
        return duoplot::GuiElementType::Unknown;
    }
}

inline std::string guiElementTypeToString(const duoplot::GuiElementType& type)
{
    std::string res;

    switch (type)
    {
        case duoplot::GuiElementType::Button: {
            res = "BUTTON";
            break;
        }
        case duoplot::GuiElementType::Slider: {
            res = "SLIDER";
            break;
        }
        case duoplot::GuiElementType::Checkbox: {
            res = "CHECKBOX";
            break;
        }
        case duoplot::GuiElementType::EditableText: {
            res = "EDITABLE_TEXT";
            break;
        }
        case duoplot::GuiElementType::DropdownMenu: {
            res = "DROPDOWN_MENU";
            break;
        }
        case duoplot::GuiElementType::ListBox: {
            res = "LISTBOX";
            break;
        }
        case duoplot::GuiElementType::RadioButtonGroup: {
            res = "RADIO_BUTTON_GROUP";
            break;
        }
        case duoplot::GuiElementType::TextLabel: {
            res = "TEXT_LABEL";
            break;
        }
        case duoplot::GuiElementType::StaticBox: {
            res = "STATIC_BOX";
            break;
        }
        case duoplot::GuiElementType::PlotPane: {
            res = "PLOT_PANE";
            break;
        }
        case duoplot::GuiElementType::ScrollingText: {
            res = "SCROLLING_TEXT";
            break;
        }
        default: {
            res = "UNKNOWN";
            break;
        }
    }

    return res;
}

inline void throwIfMissing(const nlohmann::json& j, const std::string& field_name, const std::string& exception_string)
{
    if (j.count(field_name) == 0)
    {
        throw std::runtime_error(exception_string);
    }
}

inline RGBTripletf jsonObjToColor(const nlohmann::json& j)
{
    return RGBTripletf{j["r"], j["g"], j["b"]};
}

inline nlohmann::json colorToJsonObj(const RGBTripletf& c)
{
    nlohmann::json j;
    j["r"] = c.red;
    j["g"] = c.green;
    j["b"] = c.blue;
    return j;
}

template <typename T>
void assignIfNotDefault(nlohmann::json& j, const std::string& key, const T& val, const T& default_value)
{
    if (val != default_value)
    {
        j[key] = val;
    }
}

template <typename T>
void assignIfNotDefault(
    nlohmann::json& j, const std::string& key, const std::string& sub_key, const T& val, const T& default_value)
{
    if (val != default_value)
    {
        j[key][sub_key] = val;
    }
}

template <typename T> T getOptionalValue(const nlohmann::json& j_data, const std::string& key, const T& default_value)
{
    if (j_data.contains(key))
    {
        return j_data[key];
    }
    else
    {
        return default_value;
    }
}

#endif  // PROJECT_STATE_HELPER_FUNCTIONS_H
