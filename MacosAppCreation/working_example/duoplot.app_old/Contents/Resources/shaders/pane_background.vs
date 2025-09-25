#version 330

layout(location = 0) in vec3 in_vertex;

uniform mat4 model_view_proj_mat;

out vec3 vertex_pos;

void main()
{
    gl_Position = model_view_proj_mat * vec4(in_vertex.xyz, 1.0);
    vertex_pos = in_vertex.xyz;
}
