#version 330

uniform mat4 model_view_proj_mat;
layout(location = 0) in vec3 in_vertex;
uniform vec3 vertex_color;
uniform float point_size;

out vec3 fragment_color;
out vec4 coord_out;

mat4 unitMat()
{
    mat4 rot_mat;
    for(int r = 0; r < 4; r++)
    {
        for(int c = 0; c < 4; c++)
        {
            rot_mat[r][c] = 0.0;
        }
    }

    rot_mat[0][0] = 1.0; rot_mat[1][1] = 1.0; rot_mat[2][2] = 1.0; rot_mat[3][3] = 1.0;
    return rot_mat;
}

mat4 rotationMatrixX(float ax)
{
    mat4 r = unitMat();
    r[1][1] = cos(ax);  r[1][2] = -sin(ax);
    r[2][1] = sin(ax);  r[2][2] = cos(ax);
    return r;
}

mat4 rotationMatrixY(float ay)
{
    mat4 r = unitMat();
    r[0][0] = cos(ay);  r[0][2] = sin(ay);
    r[2][0] = -sin(ay);  r[2][2] = cos(ay);
    return r;
}

mat4 rotationMatrixZ(float az)
{
    mat4 r = unitMat();
    r[0][0] = cos(az);  r[0][1] = -sin(az);
    r[1][0] = sin(az);  r[1][1] = cos(az);
    return r;
}

void main()
{
    gl_Position = model_view_proj_mat * vec4(in_vertex.x, in_vertex.y, in_vertex.z, 1.0);
    coord_out = vec4(in_vertex.x, in_vertex.y, in_vertex.z, 1.0);
    
    fragment_color = vertex_color;

    gl_PointSize = point_size;
}
