#version 330

uniform mat4 model_view_proj_mat;
uniform mat4 inverse_model_view_proj_mat;
layout(location = 0) in vec3 p0;
layout(location = 1) in vec3 p1;
layout(location = 2) in vec3 p2;
layout(location = 3) in int idx;
layout(location = 4) in vec3 in_color;
uniform vec3 vertex_color;
uniform float half_line_width;
uniform float axes_width;
uniform int has_color_vec;
uniform float axes_height;

out vec3 fragment_color;
out vec4 coord_out;

flat out vec3 p1_out;
out vec3 vert_pos;
flat out int triangle_id;

struct Line2D
{
    float a;
    float b;
    float c;
};

vec2 rotate90Deg(vec2 vec_in)
{
    return vec2(-vec_in.y, vec_in.x);
}

Line2D lineFromTwoPoints(vec2 p0, vec2 p1)
{
    Line2D line;

    vec2 v = p1 - p0;

    line.a = -v.y;
    line.b = v.x;
    line.c = -(line.a * p0.x + line.b * p0.y);

    return line;
}

vec2 intersectionOfTwoLines(Line2D line0, Line2D line1)
{
    vec2 p;

    float n0 = (line0.a * line1.b - line1.a * line0.b);
    p.x = (line0.b * line1.c - line1.b * line0.c) / n0;
    p.y = -(line0.a * line1.c - line1.a * line0.c) / n0;

    return p;
}

Line2D lineFromPointAndNormalVec(vec2 p, vec2 n)
{
    // Construct a 2d line from a point on the line 'p', and
    // line normal 'n'
    Line2D line;
    line.a = n.x;
    line.b = n.y; 
    line.c = -(line.a * p.x + line.b * p.y);

    return line;
}

float evalLine(Line2D line, vec2 point)
{
   return line.a * point.x + line.b * point.y + line.c;
}

void main()
{
    bool should_flip = false;

    vec4 p0_transformed = model_view_proj_mat * vec4(p0, 1.0);
    vec4 p1_transformed = model_view_proj_mat * vec4(p1, 1.0);
    vec4 p2_transformed = model_view_proj_mat * vec4(p2, 1.0);

    p0_transformed = p0_transformed / abs(p0_transformed.w);
    p1_transformed = p1_transformed / abs(p1_transformed.w);
    p2_transformed = p2_transformed / abs(p2_transformed.w);

    // vec_along01 points from point p0 to point p1
    vec2 vec_along01 = normalize(p1_transformed.xy - p0_transformed.xy);
    // vec_along12 points from point p1 to point p2
    vec2 vec_along12 = normalize(p2_transformed.xy - p1_transformed.xy);

    // Rotate the two vectors so that they are perpendicular to
    // their original direction, thereby reaching the edge of the
    // line.
    vec2 vec_on_line_edge01 = rotate90Deg(vec_along01) * half_line_width;
    vec2 vec_on_line_edge12 = rotate90Deg(vec_along12) * half_line_width;

    float mul_x = 1.0 / axes_width;
    float mul_y = 1.0 / axes_height;

    vec_on_line_edge01 = vec2(vec_on_line_edge01.x * mul_x, vec_on_line_edge01.y * mul_y);
    vec_on_line_edge12 = vec2(vec_on_line_edge12.x * mul_x, vec_on_line_edge12.y * mul_y);

    // Form two points on the edge, two points per segment
    vec2 point0_on_edge01 = vec_on_line_edge01;
    vec2 point1_on_edge01 = vec_on_line_edge01 - vec_along01;

    vec2 point0_on_edge12 = vec_on_line_edge12;
    vec2 point1_on_edge12 = vec_on_line_edge12 + vec_along12;

    // Fit lines from those two points that are on the edges of each segment
    Line2D line0 = lineFromTwoPoints(point0_on_edge01, point1_on_edge01);
    Line2D line1 = lineFromTwoPoints(point0_on_edge12, point1_on_edge12);

    vec2 intersection_point = intersectionOfTwoLines(line0, line1);

    // The sign of the z component of the cross product of
    // the two vectors depends on the mirroring of the view
    float which_side_vec_z = vec_along01.x * vec_along12.y - vec_along01.y * vec_along12.x;

    if(which_side_vec_z > 0.0)
    {
        should_flip = true;
    }
    else
    {
        should_flip = false;
    }

    switch(idx)
    {
    // 1st triangle
    case 0:
        gl_Position = vec4(p1_transformed.xy + vec_on_line_edge12, p1_transformed.z, p1_transformed.w);
        triangle_id = 0;
        break;
    case 1:
        gl_Position = vec4(p1_transformed.xy - vec_on_line_edge01, p1_transformed.z, p1_transformed.w);
        triangle_id = 0;
        break;
    case 2:
        gl_Position = vec4(p1_transformed.xy + vec_on_line_edge01, p1_transformed.z, p1_transformed.w);
        triangle_id = 0;
        break;
    // 2nd triangle
    case 3:
        gl_Position = vec4(p1_transformed.xy + vec_on_line_edge12, p1_transformed.z, p1_transformed.w);
        triangle_id = 1;
        break;
    case 4:
        gl_Position = vec4(p1_transformed.xy - vec_on_line_edge01, p1_transformed.z, p1_transformed.w);
        triangle_id = 1;
        break;
    case 5:
        gl_Position = vec4(p1_transformed.xy - vec_on_line_edge12, p1_transformed.z, p1_transformed.w);
        triangle_id = 1;
        break;
    // 3rd triangle
    case 6:
        gl_Position = vec4(p1_transformed.xy, p1_transformed.z, p1_transformed.w);
        triangle_id = 2;
        break;
    case 7:
        triangle_id = 2;

        if(should_flip)
        {
            gl_Position = vec4(p1_transformed.xy - vec_on_line_edge01, p1_transformed.z, p1_transformed.w);
        }
        else
        {
            gl_Position = vec4(p1_transformed.xy + vec_on_line_edge01, p1_transformed.z, p1_transformed.w);
        }
        
        break;
    case 8:
        triangle_id = 2;
        if(should_flip)
        {
            gl_Position = vec4(p1_transformed.xy - vec_on_line_edge12, p1_transformed.z, p1_transformed.w);
        }
        else
        {
            gl_Position = vec4(p1_transformed.xy + vec_on_line_edge12, p1_transformed.z, p1_transformed.w);
        }

        break;
    // 4th triangle
    case 9:
        triangle_id = 3;
        if(should_flip)
        {
            gl_Position = vec4(p1_transformed.xy - vec_on_line_edge01, p1_transformed.z, p1_transformed.w);
        }
        else
        {
            gl_Position = vec4(p1_transformed.xy + vec_on_line_edge01, p1_transformed.z, p1_transformed.w);
        }

        break;
    case 10:
        triangle_id = 3;
        if(should_flip)
        {
            gl_Position = vec4(p1_transformed.xy - vec_on_line_edge12, p1_transformed.z, p1_transformed.w);
        }
        else
        {
            gl_Position = vec4(p1_transformed.xy + vec_on_line_edge12, p1_transformed.z, p1_transformed.w);
        }

        break;
    case 11:
        triangle_id = 3;
        if(should_flip)
        {
            intersection_point = -intersection_point;
        }

        gl_Position = vec4(p1_transformed.xy + intersection_point, p1_transformed.z, p1_transformed.w);
        break;
    }

    vec4 op = inverse_model_view_proj_mat * gl_Position;

    coord_out = vec4(op.x, op.y, op.z, op.w);
    if(has_color_vec == int(1))
    {
        fragment_color = in_color;
    }
    else
    {
        fragment_color = vertex_color;
    }
    vert_pos     = gl_Position.xyz / gl_Position.w;
    p1_out = p1_transformed.xyz;
}
