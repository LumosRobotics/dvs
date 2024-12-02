#include "opengl_primitives.h"

#include <cmath>

#include "rgbtriplet.h"

using namespace duoplot;

void PutTriangleIntoBuffer(BufferedVector& buffered_vector, const Vec2f& p0, const Vec2f& p1, const Vec2f& p2)
{
    const size_t idx = buffered_vector.idx();

    buffered_vector.data()[idx] = p0.x;
    buffered_vector.data()[idx + 1U] = p0.y;

    buffered_vector.data()[idx + 2U] = p1.x;
    buffered_vector.data()[idx + 3U] = p1.y;

    buffered_vector.data()[idx + 4U] = p2.x;
    buffered_vector.data()[idx + 5U] = p2.y;

    buffered_vector.incrementIdx(6U);
}

void Put4PointPolygonIntoBuffer(
    BufferedVector& buffered_vector, const Vec2f& p0, const Vec2f& p1, const Vec2f& p2, const Vec2f& p3)
{
    const size_t idx = buffered_vector.idx();

    buffered_vector.data()[idx] = p0.x;
    buffered_vector.data()[idx + 1U] = p0.y;

    buffered_vector.data()[idx + 2U] = p1.x;
    buffered_vector.data()[idx + 3U] = p1.y;

    buffered_vector.data()[idx + 4U] = p2.x;
    buffered_vector.data()[idx + 5U] = p2.y;

    buffered_vector.data()[idx + 6U] = p2.x;
    buffered_vector.data()[idx + 7U] = p2.y;

    buffered_vector.incrementIdx(8U);
}

void PutRoundedBarIntoBuffer(BufferedVector& buffered_vector,
                             const float x_top_left,
                             const float y_top_left,
                             const float bar_length,
                             const float bar_thickness,
                             const bool horizontal,
                             const size_t num_triangles_per_edge)
{
    const float edge_radius = bar_thickness / 2.0f;

    if (horizontal)
    {
        PutRectangleIntoBuffer(
            buffered_vector, x_top_left + edge_radius, y_top_left, bar_length - 2.0f * edge_radius, bar_thickness);

        PutCircleSegmentIntoBuffer(buffered_vector,
                                   x_top_left + bar_length - edge_radius,
                                   y_top_left + bar_thickness / 2.0f,
                                   edge_radius,
                                   -M_PI / 2.0f,
                                   M_PI / 2.0f,
                                   num_triangles_per_edge);

        PutCircleSegmentIntoBuffer(buffered_vector,
                                   x_top_left + edge_radius,
                                   y_top_left + bar_thickness / 2.0f,
                                   edge_radius,
                                   M_PI / 2.0f,
                                   3.0f * M_PI / 2.0f,
                                   num_triangles_per_edge);
    }
    else
    {
        // Vertical
        PutRectangleIntoBuffer(
            buffered_vector, x_top_left, y_top_left + edge_radius, bar_thickness, bar_length - 2.0f * edge_radius);

        PutCircleSegmentIntoBuffer(buffered_vector,
                                   x_top_left + bar_thickness / 2.0f,
                                   y_top_left + bar_length - edge_radius,
                                   edge_radius,
                                   0.0f,
                                   M_PI,
                                   num_triangles_per_edge);

        PutCircleSegmentIntoBuffer(buffered_vector,
                                   x_top_left + bar_thickness / 2.0f,
                                   y_top_left + edge_radius,
                                   edge_radius,
                                   M_PI,
                                   2.0f * M_PI,
                                   num_triangles_per_edge);
    }
}

void PutRoundedRectangleEdgeIntoBuffer(BufferedVector& buffered_vector,
                                       const float x_top_left,
                                       const float y_top_left,
                                       const float width,
                                       const float height,
                                       const float edge_thickness,
                                       const float corner_radius,
                                       const size_t num_triangles_per_corner)
{
    // Top rectangle
    PutRectangleIntoBuffer(
        buffered_vector, x_top_left + corner_radius, y_top_left, width - 2.0f * corner_radius, edge_thickness);

    // Left rectangle
    PutRectangleIntoBuffer(
        buffered_vector, x_top_left, y_top_left + corner_radius, edge_thickness, height - 2.0f * corner_radius);

    // Right rectangle
    PutRectangleIntoBuffer(buffered_vector,
                           x_top_left + width - edge_thickness,
                           y_top_left + corner_radius,
                           edge_thickness,
                           height - 2.0f * corner_radius);

    // Bottom rectangle
    PutRectangleIntoBuffer(buffered_vector,
                           x_top_left + corner_radius,
                           y_top_left + height - edge_thickness,
                           width - 2.0f * corner_radius,
                           edge_thickness);

    // Top left corner
    PutCircularCurveSegmentIntoBuffer(buffered_vector,
                                      x_top_left + corner_radius,
                                      y_top_left + corner_radius,
                                      corner_radius - edge_thickness,
                                      corner_radius,
                                      M_PI / 2.0f,
                                      M_PI,
                                      num_triangles_per_corner);

    // Top right corner
    PutCircularCurveSegmentIntoBuffer(buffered_vector,
                                      x_top_left + width - corner_radius,
                                      y_top_left + corner_radius,
                                      corner_radius - edge_thickness,
                                      corner_radius,
                                      0.0f,
                                      M_PI / 2.0f,
                                      num_triangles_per_corner);

    // Bottom right corner
    PutCircularCurveSegmentIntoBuffer(buffered_vector,
                                      x_top_left + width - corner_radius,
                                      y_top_left + height - corner_radius,
                                      corner_radius - edge_thickness,
                                      corner_radius,
                                      0.0f,
                                      -M_PI / 2.0f,
                                      num_triangles_per_corner);

    // Bottom left corner
    PutCircularCurveSegmentIntoBuffer(buffered_vector,
                                      x_top_left + corner_radius,
                                      y_top_left + height - corner_radius,
                                      corner_radius - edge_thickness,
                                      corner_radius,
                                      M_PI,
                                      3.0f * M_PI / 2.0f,
                                      num_triangles_per_corner);
}

void PutRoundedRectangleIntoBuffer(BufferedVector& buffered_vector,
                                   const float x_top_left,
                                   const float y_top_left,
                                   const float width,
                                   const float height,
                                   const float corner_radius,
                                   const size_t num_triangles_per_corner)
{
    // Center rectangle
    PutRectangleIntoBuffer(
        buffered_vector, x_top_left + corner_radius, y_top_left, width - 2.0f * corner_radius, height);

    // Left rectangle
    PutRectangleIntoBuffer(
        buffered_vector, x_top_left, y_top_left + corner_radius, corner_radius, height - 2.0f * corner_radius);

    // Right rectangle
    PutRectangleIntoBuffer(buffered_vector,
                           x_top_left + width - corner_radius,
                           y_top_left + corner_radius,
                           corner_radius,
                           height - 2.0f * corner_radius);

    // Top left corner
    PutCircleSegmentIntoBuffer(buffered_vector,
                               x_top_left + corner_radius,
                               y_top_left + corner_radius,
                               corner_radius,
                               M_PI / 2.0f,
                               M_PI,
                               num_triangles_per_corner);

    // Top right corner
    PutCircleSegmentIntoBuffer(buffered_vector,
                               x_top_left + width - corner_radius,
                               y_top_left + corner_radius,
                               corner_radius,
                               0.0f,
                               M_PI / 2.0f,
                               num_triangles_per_corner);

    // Bottom right corner
    PutCircleSegmentIntoBuffer(buffered_vector,
                               x_top_left + width - corner_radius,
                               y_top_left + height - corner_radius,
                               corner_radius,
                               3.0f * M_PI / 2.0f,
                               2.0f * M_PI,
                               num_triangles_per_corner);

    // Bottom left corner
    PutCircleSegmentIntoBuffer(buffered_vector,
                               x_top_left + corner_radius,
                               y_top_left + height - corner_radius,
                               corner_radius,
                               M_PI,
                               3.0f * M_PI / 2.0f,
                               num_triangles_per_corner);
}

void PutCircularCurveSegmentIntoBuffer(BufferedVector& buffered_vector,
                                       const float x_center,
                                       const float y_center,
                                       const float inner_radius,
                                       const float outer_radius,
                                       const float theta_start,
                                       const float theta_end,
                                       const size_t num_triangles)
{
    const float angle_inc = (theta_end - theta_start) / static_cast<float>(num_triangles);

    float theta = theta_start;

    const size_t start_idx = buffered_vector.idx();

    for (size_t i = 0; i < num_triangles; ++i)
    {
        size_t idx = start_idx + i * 12U;

        const float x_pt = outer_radius * std::cos(theta);
        const float y_pt = -outer_radius * std::sin(theta);

        const float x_pt_inner = x_pt * inner_radius / outer_radius;
        const float y_pt_inner = y_pt * inner_radius / outer_radius;

        theta += angle_inc;
        const float x_pt_next = outer_radius * std::cos(theta);
        const float y_pt_next = -outer_radius * std::sin(theta);

        const float x_pt_next_inner = x_pt_next * inner_radius / outer_radius;
        const float y_pt_next_inner = y_pt_next * inner_radius / outer_radius;

        // First triangle
        buffered_vector.data()[idx] = x_pt_inner + x_center;
        buffered_vector.data()[idx + 1U] = y_pt_inner + y_center;

        buffered_vector.data()[idx + 2U] = x_pt + x_center;
        buffered_vector.data()[idx + 3U] = y_pt + y_center;

        buffered_vector.data()[idx + 4U] = x_pt_next_inner + x_center;
        buffered_vector.data()[idx + 5U] = y_pt_next_inner + y_center;

        // Second triangle
        buffered_vector.data()[idx + 6U] = x_pt_next_inner + x_center;
        buffered_vector.data()[idx + 7U] = y_pt_next_inner + y_center;

        buffered_vector.data()[idx + 8U] = x_pt + x_center;
        buffered_vector.data()[idx + 9U] = y_pt + y_center;

        buffered_vector.data()[idx + 10U] = x_pt_next + x_center;
        buffered_vector.data()[idx + 11U] = y_pt_next + y_center;

        buffered_vector.incrementIdx(12U);
    }
}

void PutCircleIntoBuffer(BufferedVector& buffered_vector,
                         const float x_center,
                         const float y_center,
                         const float radius,
                         const size_t num_triangles)
{
    size_t start_idx = buffered_vector.idx();

    for (size_t i = 0; i < num_triangles; ++i)
    {
        const size_t idx = start_idx + i * 6;

        const float theta0 = 2.0f * M_PI * static_cast<float>(i) / static_cast<float>(num_triangles);
        const float theta1 = 2.0f * M_PI * static_cast<float>(i + 1) / static_cast<float>(num_triangles);

        const float x0 = radius * std::cos(theta0);
        const float y0 = radius * std::sin(theta0);

        const float x1 = radius * std::cos(theta1);
        const float y1 = radius * std::sin(theta1);

        // First outer point
        buffered_vector.data()[idx] = x_center + x0;
        buffered_vector.data()[idx + 1] = y_center + y0;

        // Center point
        buffered_vector.data()[idx + 2] = x_center;
        buffered_vector.data()[idx + 3] = y_center;

        // Second outer point
        buffered_vector.data()[idx + 4] = x_center + x1;
        buffered_vector.data()[idx + 5] = y_center + y1;

        buffered_vector.incrementIdx(6);
    }
}

void PutRectangleIntoBuffer(BufferedVector& buffered_vector,
                            const float x_top_left,
                            const float y_top_left,
                            const float width,
                            const float height)
{
    const size_t start_idx = buffered_vector.idx();

    const Vec2f bottom_left{x_top_left, y_top_left + height};
    const Vec2f bottom_right{x_top_left + width, y_top_left + height};

    const Vec2f top_left{x_top_left, y_top_left};
    const Vec2f top_right{x_top_left + width, y_top_left};

    using Triangle = std::array<Vec2f, 3>;

    const std::array<Triangle, 2> triangles{Triangle{top_left, top_right, bottom_left},
                                            Triangle{bottom_left, top_right, bottom_right}};

    size_t idx = start_idx;

    for (const auto& corner_points : triangles)
    {
        for (const auto& corner_point : corner_points)
        {
            buffered_vector.data()[idx] = corner_point.x;
            buffered_vector.data()[idx + 1U] = corner_point.y;
            buffered_vector.incrementIdx(2U);

            idx += 2U;
        }
    }
}

void PutRotatedRectangleIntoBuffer(BufferedVector& buffered_vector,
                            const float x_top_left,
                            const float y_top_left,
                            const float width,
                            const float height,
                            const float angle)
{
    const size_t start_idx = buffered_vector.idx();

    const float cos_angle = std::cos(-angle);
    const float sin_angle = std::sin(-angle);

    const auto rotate_pt = [cos_angle, sin_angle](const float x, const float y) -> Vec2f {
        return Vec2f{x * cos_angle - y * sin_angle, x * sin_angle + y * cos_angle};
    };

    const Vec2f pos{x_top_left, y_top_left};

    const Vec2f bottom_left = rotate_pt(0.0f, height) + pos;
    const Vec2f bottom_right = rotate_pt(width, height) + pos;

    const Vec2f top_left = rotate_pt(0.0f, 0.0f) + pos;
    const Vec2f top_right = rotate_pt(width, 0.0f) + pos;

    /*const Vec2f bottom_left{x_top_left, y_top_left + height};
    const Vec2f bottom_right{x_top_left + width, y_top_left + height};

    const Vec2f top_left{x_top_left, y_top_left};
    const Vec2f top_right{x_top_left + width, y_top_left};*/

    using Triangle = std::array<Vec2f, 3>;

    const std::array<Triangle, 2> triangles{Triangle{top_left, top_right, bottom_left},
                                            Triangle{bottom_left, top_right, bottom_right}};

    size_t idx = start_idx;

    for (const auto& corner_points : triangles)
    {
        for (const auto& corner_point : corner_points)
        {
            buffered_vector.data()[idx] = corner_point.x;
            buffered_vector.data()[idx + 1U] = corner_point.y;
            buffered_vector.incrementIdx(2U);

            idx += 2U;
        }
    }
}

void PutCircleSegmentIntoBuffer(BufferedVector& buffered_vector,
                                const float x_center,
                                const float y_center,
                                const float radius,
                                const float theta_start,
                                const float theta_end,
                                const size_t num_triangles)
{
    const size_t start_idx = buffered_vector.idx();

    const float delta_angle = (theta_end - theta_start) / static_cast<float>(num_triangles);

    for (size_t k = 0; k < num_triangles; ++k)
    {
        const size_t idx = start_idx + k * 6;

        const float theta0 = theta_start + static_cast<float>(k) * delta_angle;
        const float theta1 = theta_start + static_cast<float>(k + 1) * delta_angle;

        const float x0 = radius * std::cos(theta0);
        const float y0 = -radius * std::sin(theta0);

        const float x1 = radius * std::cos(theta1);
        const float y1 = -radius * std::sin(theta1);

        // First outer point
        buffered_vector.data()[idx] = x_center + x0;
        buffered_vector.data()[idx + 1] = y_center + y0;

        // Center point
        buffered_vector.data()[idx + 2] = x_center;
        buffered_vector.data()[idx + 3] = y_center;

        // Second outer point
        buffered_vector.data()[idx + 4] = x_center + x1;
        buffered_vector.data()[idx + 5] = y_center + y1;

        buffered_vector.incrementIdx(6);
    }

    /*
    float theta = theta0;
        for (size_t k = 0; k < kNumTrianglesPerEdge; ++k)
        {
            line_buffer_.data()[idx] = x_center;
            line_buffer_.data()[idx + 1U] = y_center;

            const float x_pt = kEdgeRadius * std::cos(theta);
            const float y_pt = kEdgeRadius * std::sin(theta);

            line_buffer_.data()[idx + 2U] = x_center + x_pt;
            line_buffer_.data()[idx + 3U] = y_center + y_pt;

            theta += angle_inc;
            const float x_pt_next = kEdgeRadius * std::cos(theta);
            const float y_pt_next = kEdgeRadius * std::sin(theta);

            line_buffer_.data()[idx + 4U] = x_center + x_pt_next;
            line_buffer_.data()[idx + 5U] = y_center + y_pt_next;

            idx += 6U;
        }
    */
}