#version 330

layout(location = 0) in vec3 in_vertex;

uniform mat4 model_view_proj_mat;
uniform vec3 vertex_color;

out vec3 fragment_color;

void main()
{
    gl_Position = model_view_proj_mat * vec4(in_vertex.xyz, 1.0);
    fragment_color = vertex_color;
}
