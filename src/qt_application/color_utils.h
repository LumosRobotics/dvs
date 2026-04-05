#ifndef QT_APPLICATION_COLOR_UTILS_H_
#define QT_APPLICATION_COLOR_UTILS_H_

#include <QColor>

#include "color.h"

inline QColor colorToQColor(Color_t col)
{
    switch (col)
    {
        case Color_t::RED:          return QColor(255,   0,   0);
        case Color_t::GREEN:        return QColor(  0, 200,   0);
        case Color_t::BLUE:         return QColor(  0,   0, 255);
        case Color_t::YELLOW:       return QColor(200, 200,   0);
        case Color_t::ORANGE:       return QColor(255, 128,   0);
        case Color_t::PURPLE:       return QColor(128,   0, 255);
        case Color_t::PINK:         return QColor(255,   0, 255);
        case Color_t::CYAN:         return QColor(  0, 200, 200);
        case Color_t::WHITE:        return QColor(255, 255, 255);
        case Color_t::BLACK:        return QColor(  0,   0,   0);
        case Color_t::GRAY:         return QColor(128, 128, 128);
        case Color_t::BROWN:        return QColor(128,  64,   0);
        case Color_t::LIGHT_GREEN:  return QColor(128, 255, 128);
        case Color_t::LIGHT_ORANGE: return QColor(255, 192, 128);
        default:                    return QColor(  0,   0,   0);
    }
}

#endif  // QT_APPLICATION_COLOR_UTILS_H_
