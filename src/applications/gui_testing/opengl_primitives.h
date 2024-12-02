#ifndef DVS_OPENGL_PRIMITIVES_H
#define DVS_OPENGL_PRIMITIVES_H

#include <stddef.h>
#include <stdint.h>

#include <cstring>
#include <vector>

#include "duoplot/internal.h"
#include "duoplot/math/math.h"

class BufferedVector
{
public:
    BufferedVector() = delete;
    BufferedVector(size_t size) : data_(new float[size]), size_(size), idx_(0) {}
    ~BufferedVector()
    {
        delete[] data_;
    }

    float* data()
    {
        return data_;
    }

    size_t size()
    {
        return size_;
    }

    size_t idx()
    {
        return idx_;
    }

    void reset()
    {
        idx_ = 0;
    }

    void setAllValuesToZero()
    {
        std::memset(data_, 0, size_ * sizeof(float));
    }

    void incrementIdx(size_t increment)
    {
        idx_ += increment;
    }

private:
    float* data_;
    size_t size_;
    size_t idx_;
};

void PutCircleSegmentIntoBuffer(BufferedVector& buffered_vector,
                                const float x_center,
                                const float y_center,
                                const float radius,
                                const float theta_start,
                                const float theta_end,
                                const size_t num_triangles);

void PutCircularCurveSegmentIntoBuffer(BufferedVector& buffered_vector,
                                       const float x_center,
                                       const float y_center,
                                       const float inner_radius,
                                       const float outer_radius,
                                       const float theta_start,
                                       const float theta_end,
                                       const size_t num_triangles);

void PutCircleIntoBuffer(BufferedVector& buffered_vector,
                         const float x_center,
                         const float y_center,
                         const float radius,
                         const size_t num_triangles);
void PutRectangleIntoBuffer(BufferedVector& buffered_vector,
                            const float x_top_left,
                            const float y_top_left,
                            const float width,
                            const float height);

void PutRoundedRectangleIntoBuffer(BufferedVector& buffered_vector,
                                   const float x_top_left,
                                   const float y_top_left,
                                   const float width,
                                   const float height,
                                   const float corner_radius,
                                   const size_t num_triangles_per_corner);

void PutRoundedRectangleEdgeIntoBuffer(BufferedVector& buffered_vector,
                                       const float x_top_left,
                                       const float y_top_left,
                                       const float width,
                                       const float height,
                                       const float edge_thickness,
                                       const float corner_radius,
                                       const size_t num_triangles_per_corner);

void PutRoundedBarIntoBuffer(BufferedVector& buffered_vector,
                             const float x_top_left,
                             const float y_top_left,
                             const float bar_length,
                             const float bar_thickness,
                             const bool horizontal,
                             const size_t num_triangles_per_edge);

void PutTriangleIntoBuffer(BufferedVector& buffered_vector,
                           const duoplot::Vec2f& p0,
                           const duoplot::Vec2f& p1,
                           const duoplot::Vec2f& p2);
void Put4PointPolygonIntoBuffer(BufferedVector& buffered_vector,
                                const duoplot::Vec2f& p0,
                                const duoplot::Vec2f& p1,
                                const duoplot::Vec2f& p2,
                                const duoplot::Vec2f& p3);

void PutRotatedRectangleIntoBuffer(BufferedVector& buffered_vector,
                                   const float x_center,
                                   const float y_center,
                                   const float width,
                                   const float height,
                                   const float angle);

void PutHexagonIntoBuffer(BufferedVector& buffered_vector,
                          const float x_center,
                          const float y_center,
                          const float angle_offset,
                          const float radius);

void PutRoundedHexagonIntoBuffer(BufferedVector& buffered_vector,
                                 const float x_center,
                                 const float y_center,
                                 const float angle_offset,
                                 const float radius,
                                 const float corner_angle,
                                 const float corner_radius,
                                 const size_t num_triangles_per_corner);
void PutRotatedRectangleIntoBuffer(BufferedVector& buffered_vector,
                            const float x_top_left,
                            const float y_top_left,
                            const float width,
                            const float height,
                            const float angle);
// TODO: Should all of these functions just be part of BufferedVector?

#endif  // DVS_OPENGL_PRIMITIVES_H
