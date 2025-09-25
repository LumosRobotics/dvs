#version 330

out vec4 color;
in vec3 fragment_color;
in vec4 coord_out;

uniform int use_clip_plane;
uniform vec4 clip_plane0;
uniform vec4 clip_plane1;
uniform vec4 clip_plane2;
uniform vec4 clip_plane3;
uniform vec4 clip_plane4;
uniform vec4 clip_plane5;

uniform int scatter_mode;

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

    float a = 1.0;
    // Default scatter_mode == 0 is square mode, which requires no modifications

    if(scatter_mode == 1) // Circle
    {
        vec2 circCoord = 2.0 * gl_PointCoord - 1.0;
        float squared_dist = dot(circCoord, circCoord);
        if ((squared_dist > 1.0) || (squared_dist < 0.4))
        {
            discard;
        }
    }
    else if(scatter_mode == 2) // Disc
    {
        vec2 circCoord = 2.0 * gl_PointCoord - 1.0;
        float squared_dist = dot(circCoord, circCoord);
        if (squared_dist > 1.0)
        {
            discard;
        }
        // else if((squared_dist <= 1.0) && (squared_dist > 0.85))
        // {
        //     a = 1.0 - squared_dist;
        // }
    }
    else if(scatter_mode == 3) // Plus
    {
        vec2 circCoord = 2.0 * gl_PointCoord - 1.0;
        float f = 0.2;

        if (((circCoord.x < -f) && (circCoord.y < -f)) ||
            ((circCoord.x > f) && (circCoord.y < -f)) ||
            ((circCoord.x > f) && (circCoord.y > f)) ||
            ((circCoord.x < -f) && (circCoord.y > f)))
        {
            discard;
        }
    }
    else if(scatter_mode == 4) // Cross
    {
        vec2 circCoord = 2.0 * gl_PointCoord - 1.0;
        float f = 0.2;

        if ( ((circCoord.x + circCoord.y) < -f) && ((circCoord.x - circCoord.y) < -f) ||
             ((circCoord.x + circCoord.y) > f) && ((circCoord.x - circCoord.y) < -f) ||
             ((circCoord.x + circCoord.y) > f) && ((circCoord.x - circCoord.y) > f) ||
             ((circCoord.x + circCoord.y) < -f) && ((circCoord.x - circCoord.y) > f) )
        {
            discard;
        }
    }

    color = vec4(fragment_color, a);
		
}
