#version 330

in vec3 vertex_pos;

// uniform float axes_width;
// uniform float axes_height;
// uniform float radius;
// uniform vec3 vertex_color;

// uniform vec2 element_positions[20];
// uniform vec2 element_dimensions[20];
uniform int elp;

uniform int num_elements;

out vec3 color;

void main()
{
   color = vec3(1.0, 0.0, 0.0);

   for(int i = 0; i < num_elements; i++)
   {
      /*if(vertex_pos.x >= element_positions[i].x && vertex_pos.x <= element_positions[i].x + element_dimensions[i].x &&
         vertex_pos.y >= element_positions[i].y && vertex_pos.y <= element_positions[i].y + element_dimensions[i].y)
      {
         color = vec3(0.0, 0.0, 1.0);
         // color = vertex_color;
      }*/
   }
   
   
}
