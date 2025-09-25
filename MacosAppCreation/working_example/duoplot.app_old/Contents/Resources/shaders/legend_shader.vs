#version 330

layout(location = 0) in vec3 in_vertex;
layout(location = 1) in vec3 in_color;

uniform mat4 model_view_proj_mat;
uniform float point_size;

out vec3 fragment_color;

void main()
{
    gl_PointSize = point_size;
    gl_Position = model_view_proj_mat * vec4(in_vertex.xyz, 1.0);
    fragment_color = in_color;
}
