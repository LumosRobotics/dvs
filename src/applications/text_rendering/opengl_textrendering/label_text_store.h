#pragma once
#include <string>
#include <vector>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include "buffers/gBuffers.h"
#include "font_atlas.h"

struct label_text
{
    std::string label;
    glm::vec2   label_loc;
    glm::vec3   label_color;
    float       label_angle;
    float       label_size;
};

class label_text_store
{
public:
    font_atlas main_font;

    label_text_store() = default;
    ~label_text_store() = default;

    // Load font and build the glyph atlas.
    void init(const std::string& font_path, uint32_t pixel_size = 128);

    void add_text(const std::string& label, glm::vec2 text_loc, glm::vec3 text_color,
                  float geom_scale, float font_angle, float font_size);

    // Build GPU buffers from all added labels (call after all add_text calls).
    void set_buffers();

    void paint_text();

private:
    gBuffers             label_buffers;
    std::vector<label_text> labels;
    uint32_t             total_glyph_count = 0; // actual shaped glyph count after set_buffers

    // Fill vertex/index arrays for one label using HarfBuzz shaping.
    // Returns the number of glyphs actually written.
    uint32_t get_buffer(const label_text& lb,
                        float* vertices, uint32_t& vertex_index,
                        uint32_t* indices, uint32_t& indices_index);

    glm::vec2 rotate_pt(const glm::vec2& rotate_about, const glm::vec2& pt, float rotation_angle);
};
