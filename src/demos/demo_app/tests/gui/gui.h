#ifndef DEMOS_TESTS_GUI_H
#define DEMOS_TESTS_GUI_H

#include <Box2D/Box2D.h>
#include <Box2D/Particle/b2Particle.h>

#include <fstream>
#include <nlohmann/json.hpp>
#include <tuple>

#include "lumos/plotting/duoplot.h"

using namespace lumos;

namespace gui_test
{

void testBasic();
void testDynamicSystem();

}  // namespace gui_test

#endif  // DEMOS_TESTS_GUI_H
