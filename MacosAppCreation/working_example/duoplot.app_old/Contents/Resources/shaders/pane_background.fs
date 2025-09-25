#version 330

in vec3 vertex_pos;

uniform float axes_width;
uniform float axes_height;
uniform float radius;
uniform vec3 vertex_color;

out vec3 color;

void main()
{
   float rx = 2.0 * 3.0 * radius / axes_width;
   float ry = 2.0 * 3.0 * radius / axes_height;

   float d, dx, dy;

   if(vertex_pos.x < 0.0)
   {
      if(vertex_pos.y < 0.0)
      {
         // Bottom left corner
         dx = (vertex_pos.x + (3.0 - rx)) / rx;
         dy = (vertex_pos.y + (3.0 - ry)) / ry;
      }
      else
      {
         // Top left corner
         dx = (vertex_pos.x + (3.0 - rx)) / rx;
         dy = (vertex_pos.y - (3.0 - ry)) / ry;
      }
   }
   else
   {
      if(vertex_pos.y < 0.0)
      {
         // Bottom right corner
         dx = (vertex_pos.x - (3.0 - rx)) / rx;
         dy = (vertex_pos.y + (3.0 - ry)) / ry;
      }
      else
      {
         // Top right corner
         dx = (vertex_pos.x - (3.0 - rx)) / rx;
         dy = (vertex_pos.y - (3.0 - ry)) / ry;
      }
   }

   d = dx * dx + dy * dy;

   if(d < 1.0)
   {
      // color = vec3(1.0, 0.0, 0.0);
      discard;
   }
   else
   {
      color = vertex_color;
   }
   
}
