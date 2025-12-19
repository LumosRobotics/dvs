#ifndef DEMOS_TESTS_BOXES_BOXES_H_
#define DEMOS_TESTS_BOXES_BOXES_H_

#include <reactphysics3d/reactphysics3d.h>

#include <array>

#include "debug_value_reader.h"
#include "lumos/plotting/duoplot.h"
// #include <box2d/b2_prismatic_joint.h>
// src/externals/box2d/include/box2d/b2_prismatic_joint.h

// Don't use 'using namespace lumos' to avoid Quaternion collision with reactphysics3d
// Instead, bring in all needed types and functions individually

// Basic types
using lumos::Vec2;
using lumos::Vec3;
using lumos::Vec3d;
using lumos::Vec3f;
using lumos::Point3d;
using lumos::Point3f;
using lumos::Vector;
using lumos::VectorConstView;
using lumos::VectorInitializer;
using lumos::IndexTriplet;
using lumos::Matrix;
using lumos::ItemId;
using lumos::PropertySet;

// Math functions
using lumos::diagMatrix;
using lumos::rotationMatrixX;
using lumos::rotationMatrixY;
using lumos::rotationMatrixZ;

// Plotting functions
using lumos::openProjectFile;
using lumos::setActiveView;
using lumos::clearView;
using lumos::axis;
using lumos::plot;
using lumos::scatter;
using lumos::drawMesh;
using lumos::clearViewOnUpdate;
using lumos::globalIllumination;
using lumos::disableScaleOnRotation;
using lumos::axesSquare;
using lumos::waitForFlush;
using lumos::viewAngles;
using lumos::flushCurrentElement;

// Properties namespace alias
namespace properties = lumos::properties;

namespace boxes
{

void testBasic();

}  // namespace boxes

#endif  // DEMOS_TESTS_BOXES_BOXES_H_