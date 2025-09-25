#version 330

layout(location = 0) in vec3 in_vertex;

uniform mat4 model_view_proj_mat;


/*uniform vec2 element_position0;
uniform vec2 element_position1;
uniform vec2 element_position2;
uniform vec2 element_position3;
uniform vec2 element_position4;
uniform vec2 element_position5;
uniform vec2 element_position6;
uniform vec2 element_position7;
uniform vec2 element_position8;
uniform vec2 element_position9;

uniform vec2 element_dimension0;
uniform vec2 element_dimension1;
uniform vec2 element_dimension2;
uniform vec2 element_dimension3;
uniform vec2 element_dimension4;
uniform vec2 element_dimension5;
uniform vec2 element_dimension6;
uniform vec2 element_dimension7;
uniform vec2 element_dimension8;
uniform vec2 element_dimension9;*/

// uniform vec2 element_positions[20];
out vec3 vertex_pos;



void main()
{
    gl_Position = model_view_proj_mat * vec4(in_vertex.xyz, 1.0);
    vertex_pos = in_vertex.xyz;
}
