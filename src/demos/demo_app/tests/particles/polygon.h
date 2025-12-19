#ifndef DEMOS_TESTS_PARTICLES_POLYGON_H_
#define DEMOS_TESTS_PARTICLES_POLYGON_H_

#include <Box2D/Box2D.h>
#include <Box2D/Particle/b2Particle.h>

#include <fstream>
#include <nlohmann/json.hpp>

#include "lumos/plotting/duoplot.h"
#include "line2d.h"
#include "misc/rgb_triplet.h"
#include "rapidxml.hpp"

using namespace lumos;

namespace particles
{

class Polygon
{
private:
    struct PointPair
    {
        Point2f p0;
        Point2f p1;
    };

    Vector<PointPair> point_pairs_;
    Vec2f min_vec_;
    Vec2f max_vec_;
    Vec2f random_vec_;
    Vec2f perp_random_vec_;

    enum class IntersectionType
    {
        Intersects,
        DoesNotIntersect,
        Collinear
    };

    IntersectionType areIntersecting(const float v1x1,
                                     const float v1y1,
                                     const float v1x2,
                                     const float v1y2,
                                     const float v2x1,
                                     const float v2y1,
                                     const float v2x2,
                                     const float v2y2) const
    {
        float d1, d2;
        float a1, a2, b1, b2, c1, c2;

        // Convert vector 1 to a line (line 1) of infinite length.
        // We want the line in linear equation standard form: A*x + B*y + C = 0
        // See: http://en.wikipedia.org/wiki/Linear_equation
        a1 = v1y2 - v1y1;
        b1 = v1x1 - v1x2;
        c1 = (v1x2 * v1y1) - (v1x1 * v1y2);

        // Every point (x,y), that solves the equation above, is on the line,
        // every point that does not solve it, is not. The equation will have a
        // positive result if it is on one side of the line and a negative one
        // if is on the other side of it. We insert (x1,y1) and (x2,y2) of vector
        // 2 into the equation above.
        d1 = (a1 * v2x1) + (b1 * v2y1) + c1;
        d2 = (a1 * v2x2) + (b1 * v2y2) + c1;

        // If d1 and d2 both have the same sign, they are both on the same side
        // of our line 1 and in that case no intersection is possible. Careful,
        // 0 is a special case, that's why we don't test ">=" and "<=",
        // but "<" and ">".
        if ((d1 > 0.0) && (d2 > 0.0))
        {
            return IntersectionType::DoesNotIntersect;
        }
        if ((d1 < 0.0) && (d2 < 0.0))
        {
            return IntersectionType::DoesNotIntersect;
        }

        // The fact that vector 2 intersected the infinite line 1 above doesn't
        // mean it also intersects the vector 1. Vector 1 is only a subset of that
        // infinite line 1, so it may have intersected that line before the vector
        // started or after it ended. To know for sure, we have to repeat the
        // the same test the other way round. We start by calculating the
        // infinite line 2 in linear equation standard form.
        a2 = v2y2 - v2y1;
        b2 = v2x1 - v2x2;
        c2 = (v2x2 * v2y1) - (v2x1 * v2y2);

        // Calculate d1 and d2 again, this time using points of vector 1.
        d1 = (a2 * v1x1) + (b2 * v1y1) + c2;
        d2 = (a2 * v1x2) + (b2 * v1y2) + c2;

        // Again, if both have the same sign (and neither one is 0),
        // no intersection is possible.
        if ((d1 > 0) && (d2 > 0))
        {
            return IntersectionType::DoesNotIntersect;
        }
        if ((d1 < 0) && (d2 < 0))
        {
            return IntersectionType::DoesNotIntersect;
        }

        // If we get here, only two possibilities are left. Either the two
        // vectors intersect in exactly one point or they are collinear, which
        // means they intersect in any number of points from zero to infinite.
        if (std::fabs((a1 * b2) - (a2 * b1)) < 1e-7)
        {
            return IntersectionType::Collinear;
        }

        // If they are not collinear, they must intersect in exactly one point.
        return IntersectionType::Intersects;
    }

public:
    Polygon() {}

    Polygon(const Vector<Point2f>& points)
    {
        point_pairs_.resize(points.size());

        min_vec_ = points(0);
        max_vec_ = points(0);

        for (size_t k = 0; k < points.size(); k++)
        {
            min_vec_.x = std::min(min_vec_.x, points(k).x);
            min_vec_.y = std::min(min_vec_.y, points(k).y);

            max_vec_.x = std::max(max_vec_.x, points(k).x);
            max_vec_.y = std::max(max_vec_.y, points(k).y);
        }

        random_vec_ = Vec2f(std::sqrt(2.0), std::sqrt(7.0));
        perp_random_vec_ = Vec2f{-random_vec_.y, random_vec_.x};

        for (size_t k = 0; k < (points.size() - 1U); k++)
        {
            point_pairs_(k).p0 = points(k);
            point_pairs_(k).p1 = points(k + 1);
        }

        point_pairs_(point_pairs_.size() - 1U).p0 = points(points.size() - 1U);
        point_pairs_(point_pairs_.size() - 1U).p1 = points(0U);
    }

    Vec2f getMinVec() const
    {
        return min_vec_;
    }

    Vec2f getMaxVec() const
    {
        return max_vec_;
    }

    void visualize() const
    {
        Vector<float> x{point_pairs_.size() + 1}, y{point_pairs_.size() + 1};

        for (size_t k = 0; k < point_pairs_.size(); k++)
        {
            x(k) = point_pairs_(k).p0.x;
            y(k) = point_pairs_(k).p0.y;
        }
        x(x.size() - 1U) = point_pairs_(0).p0.x;
        y(y.size() - 1U) = point_pairs_(0).p0.y;

        scatter(x, y);
        plot(x, y);
    }

    bool pointIsInPolygon(const Point2f& pt) const
    {
        if ((pt.x < min_vec_.x) || (pt.x > max_vec_.x) || (pt.y < min_vec_.y) || (pt.y > max_vec_.x))
        {
            return false;
        }

        int num_crossings = 0;

        for (size_t k = 0; k < point_pairs_.size(); k++)
        {
            const Point2f p0 = point_pairs_(k).p0;
            const Point2f p1 = point_pairs_(k).p1;

            const Point2f q0 = pt;
            const Point2f q1 = pt + random_vec_;

            IntersectionType intersection_type = areIntersecting(p0.x, p0.y, p1.x, p1.y, q0.x, q0.y, q1.x, q1.y);

            if (intersection_type == IntersectionType::Collinear)
            {
                const Point2f q0 = pt;
                const Point2f q1 = pt + perp_random_vec_;

                intersection_type = areIntersecting(p0.x, p0.y, p1.x, p1.y, q0.x, q0.y, q1.x, q1.y);
            }

            if (intersection_type == IntersectionType::Intersects)
            {
                num_crossings++;
            }
        }

        return (num_crossings % 2) == 1;
    }
};

}  // namespace particles

#endif
