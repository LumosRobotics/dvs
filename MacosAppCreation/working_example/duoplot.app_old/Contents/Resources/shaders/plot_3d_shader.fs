#version 330

out vec4 color;
in vec3 fragment_color;
in vec4 coord_out;
flat in vec3 p1_out;
in vec3 vert_pos;
flat in int triangle_id;
uniform int use_dash;
uniform float half_line_width;

uniform int use_clip_plane;
uniform vec4 clip_plane0;
uniform vec4 clip_plane1;
uniform vec4 clip_plane2;
uniform vec4 clip_plane3;
uniform vec4 clip_plane4;
uniform vec4 clip_plane5;

uniform float axes_width;
uniform float axes_height;
uniform float dash_size;
uniform float gap_size;

void main()
{
   if(use_clip_plane == 1)
   {
      if(dot(clip_plane0, coord_out) < 0.0)
      {
         discard;
      }
      else if(dot(clip_plane1, coord_out) < 0.0)
      {
         discard;
      }
      else if(dot(clip_plane2, coord_out) < 0.0)
      {
         discard;
      }
      else if(dot(clip_plane3, coord_out) < 0.0)
      {
         discard;
      }
      else if(dot(clip_plane4, coord_out) < 0.0)
      {
         discard;
      }
      else if(dot(clip_plane5, coord_out) < 0.0)
      {
         discard;
      }
   }

   if(triangle_id == 3)
   {
      float dx = vert_pos.x - p1_out.x;
      float dy = vert_pos.y - p1_out.y;

      float rx = half_line_width / axes_width;
      float ry = half_line_width / axes_height;

      float d = dx * dx / (rx * rx) + dy * dy / (ry * ry);

      if(d > 1.0)
      {
         discard;
      }
   }

   color = vec4(fragment_color, 1.0);

   /* TODO: Finish to enable dashed line
   vec3 vec_to_fragment;
   vec3 vec_along_line;

   if(triangle_id_int == 0)
   {
      vec_to_fragment = vert_pos - p0_out;
      vec_along_line = p1_out - p0_out;
   }
   else
   {
      vec_to_fragment = vert_pos - p1_out;
      vec_along_line = p2_out - p1_out;
   }

   vec3 projected_vec = (dot(vec_to_fragment, vec_along_line) / dot(vec_along_line, vec_along_line)) * vec_along_line;

   vec2 dir = (projected_vec.xy);

   float dist = length(dir) + length_along_fs;

   float gap_size_ = 0.05;
   float dash_size_ = 0.05;

   if ((use_dash == 1) && (fract(dist / (dash_size_ + gap_size_)) > dash_size_ / (dash_size_ + gap_size_)) )
   {
      discard;
   }*/

}
