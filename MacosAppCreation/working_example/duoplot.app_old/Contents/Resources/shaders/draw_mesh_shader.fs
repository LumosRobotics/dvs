#version 330

out vec4 color;
in vec3 colormap_color;
flat in vec3 flat_colormap_color;
uniform int color_map_selection;
uniform int has_color_vec;

uniform vec3 edge_color;
uniform vec3 face_color;
flat in vec3 individual_color;
in vec4 coord_out;
in vec3 frag_pos;
in vec3 frag_normal;

uniform int use_color;

uniform vec3 light_pos;
uniform int has_face_color;
uniform int has_edge_color;
uniform int is_edge;
uniform int interpolate_colormap;
uniform int global_illumination_active;

uniform float alpha;

uniform int use_clip_plane;
uniform vec4 clip_plane0;
uniform vec4 clip_plane1;
uniform vec4 clip_plane2;
uniform vec4 clip_plane3;
uniform vec4 clip_plane4;
uniform vec4 clip_plane5;

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

   vec3 object_color;
   if(is_edge == int(1))
   {
      object_color = edge_color;
   }
   else
   {
      if(has_color_vec == int(1))
      {
         object_color = individual_color;
      }
      else if(color_map_selection > 0)
      {
         if(interpolate_colormap == int(1))
         {
            object_color = colormap_color;
         }
         else
         {
            object_color = flat_colormap_color;
         }
      }
      else
      {
         object_color = face_color;
      }
   }

   if(global_illumination_active == int(1))
   {
      vec3 light_color = vec3(1.0, 1.0, 1.0);
      float ambient_strength = 1.0;
      vec3 ambient = ambient_strength * light_color;

      vec3 light_dir = normalize(light_pos - frag_pos);
      vec3 norm = normalize(frag_normal);
      float diff = max(dot(norm, light_dir), 0.0);

      vec3 diffuse = diff * light_color;

      vec3 view_pos = vec3(3.0, 3.0, 3.0);
      float specular_strength = 0.5;

      vec3 view_dir = normalize(view_pos - frag_pos);
      vec3 reflect_dir = reflect(-light_dir, norm);

      float spec = pow(max(dot(view_dir, reflect_dir), 0.0), 2);
      vec3 specular = specular_strength * spec * light_color;  

      // color = vec4((ambient + diffuse + specular) * object_color, 1.0);
      color = vec4((ambient + specular) * object_color, 1.0);
      color.r = min(color.r, 1.0);
      color.g = min(color.g, 1.0);
      color.b = min(color.b, 1.0);
      color.a = alpha;
   }
   else
   {
      color = vec4(object_color, alpha);
   }
}
