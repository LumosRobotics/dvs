#version 330

uniform mat4 model_view_proj_mat;
uniform int use_global_alpha;
uniform int has_custom_transform;
uniform mat4 custom_translation_mat;
uniform mat4 custom_rotation_mat;
uniform mat4 custom_scale_mat;
uniform float global_alpha;

layout (location = 0) in vec3 vertex_in;
layout (location = 1) in vec2 text_coord_in;

out vec4 coord_out;
out vec2 tex_coord_out;

void main()
{
    if(has_custom_transform == int(1))
    {
        vec4 transformed_pos = custom_translation_mat * custom_rotation_mat * custom_scale_mat * vec4(vertex_in.x, vertex_in.y, vertex_in.z, 1.0);
        coord_out = transformed_pos;
        gl_Position = model_view_proj_mat * transformed_pos;
    }
    else
    {
        gl_Position = model_view_proj_mat * vec4(vertex_in.x, vertex_in.y, vertex_in.z, 1.0);
        coord_out = vec4(vertex_in.x, vertex_in.y, vertex_in.z, 1.0);
    }

    tex_coord_out = text_coord_in;
}
