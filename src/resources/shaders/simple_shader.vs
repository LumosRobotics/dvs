#version 400 core

layout(location = 0) in vec3 in_vertex;
layout(location = 1) in vec3 in_color;
uniform vec3 vertex_color;
uniform float pane_width;
uniform float pane_height;
uniform float alpha;
uniform int shader_mode;

out vec4 fragment_color;
// out vec4 coord_out;

void main()
{
    if (shader_mode == int(0))  // Normal
    {
        // Largest value that a point can have is width - 1, or height - 1
        gl_Position = vec4(2.0 * in_vertex.x / (pane_width - 1.0) - 1.0,
                           1.0 - 2.0 * in_vertex.y / (pane_height - 1.0),
                           in_vertex.z,
                           1.0);

        fragment_color = vec4(vertex_color, 1.0);
    }
    else if (shader_mode == int(1))  // Button
    {
        gl_Position = vec4(2.0 * in_vertex.x / (pane_width - 1.0) - 1.0,
                           1.0 - 2.0 * in_vertex.y / (pane_height - 1.0),
                           in_vertex.z,
                           1.0);

        fragment_color = vec4(vertex_color, 1.0);
    }
    else if (shader_mode == int(2))  // Slider
    {
        gl_Position = vec4(2.0 * in_vertex.x / (pane_width - 1.0) - 1.0,
                           1.0 - 2.0 * in_vertex.y / (pane_height - 1.0),
                           in_vertex.z,
                           1.0);

        fragment_color = vec4(vertex_color, 1.0);
    }
    else if (shader_mode == int(3))  // Checkbox
    {
        gl_Position = vec4(2.0 * in_vertex.x / (pane_width - 1.0) - 1.0,
                           1.0 - 2.0 * in_vertex.y / (pane_height - 1.0),
                           in_vertex.z,
                           1.0);

        fragment_color = vec4(vertex_color, 1.0);
    }
    else if(shader_mode == int(4))  // Sizing Rectangles
    {
        gl_Position = vec4(2.0 * in_vertex.x / (pane_width - 1.0) - 1.0,
                           1.0 - 2.0 * in_vertex.y / (pane_height - 1.0),
                           in_vertex.z,
                           1.0);

        fragment_color = vec4(in_color, alpha);
    }

}
