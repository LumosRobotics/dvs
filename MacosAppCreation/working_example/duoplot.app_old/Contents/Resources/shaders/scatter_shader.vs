#version 330

uniform mat4 model_view_proj_mat;
layout(location = 0) in vec3 in_vertex;
layout(location = 1) in vec3 in_color;
layout(location = 2) in float in_point_sizes;
uniform vec3 vertex_color;
uniform vec3 distance_from_point;
uniform float point_size;
uniform float min_dist;
uniform float max_dist;
uniform int has_color_vec;
uniform int has_point_sizes_vec;
uniform int distance_from_type;
uniform int color_map_selection;
uniform int has_distance_from;

out vec3 fragment_color;
out vec4 coord_out;

vec3 calculateColormapRainbow(float value)
{
    if(value < 0.0)
    {
        value = 0.0;
    }
    else if(value >= 1.0)
    {
        value = 0.99999;
    }

    float full_range_value = value * 5.0;
    float integer_part = floor(full_range_value);
    float fraction_part = full_range_value - integer_part;

    float r = 0.0, g = 0.0, b = 0.0;

    switch(int(integer_part))
    {
    case 0:
        r = 1.0 - fraction_part;
        g = 1.0;
        b = 0.0;
        break;
    case 1:
        r = 0.0;
        g = 1.0;
        b = fraction_part;
        break;
    case 2:
        r = 0.0;
        g = 1.0 - fraction_part;
        b = 1.0;
        break;
    case 3:
        r = fraction_part;
        g = 0.0;
        b = 1.0;
        break;
    case 4:
        r = 1.0;
        g = 0.0;
        b = 1.0 - fraction_part;
        break;
    }

    return vec3(r, g, b);
}

vec3 calculateColormapMagma(float value)
{
    if(value < 0.0)
    {
        value = 0.0;
    }
    else if(value >= 1.0)
    {
        value = 0.99999;
    }

    float full_range_value = value * 3.0;
    float integer_part = floor(full_range_value);
    float fraction_part = full_range_value - integer_part;

    float r = 0.0, g = 0.0, b = 0.0;

    switch(int(integer_part))
    {
    case 0:
        r = 0.023529411764705882 + fraction_part * 0.403921568627451;
        g = 0.011764705882352941 + fraction_part * 0.047058823529411764;
        b = 0.09019607843137255 + fraction_part * 0.4117647058823529;
        break;
    case 1:
        r = 0.42745098039215684 + fraction_part * 0.5019607843137255;
        g = 0.058823529411764705 + fraction_part * 0.3372549019607843;
        b = 0.5019607843137255 - fraction_part * 0.13725490196078433;
        break;
    case 2:
        r = 0.9294117647058824 + fraction_part * 0.050980392156862675;
        g = 0.396078431372549 + fraction_part * 0.5529411764705883;
        b = 0.36470588235294116 + fraction_part * 0.34117647058823536;
        break;
    }

    return vec3(r, g, b);
}

vec3 calculateColormapViridis(float value)
{
    if(value < 0.0)
    {
        value = 0.0;
    }
    else if(value >= 1.0)
    {
        value = 0.99999;
    }

    float full_range_value = value * 8.0;
    float integer_part = floor(full_range_value);
    float fraction_part = full_range_value - integer_part;

    float r = 0.0, g = 0.0, b = 0.0;

    switch(int(integer_part))
    {
    case 0:
        r = 0.2627450980392157 + fraction_part * 0.01568627450980392;
        g = fraction_part * 0.16470588235294117;
        b = 0.32941176470588235 + fraction_part * 0.1568627450980392;
        break;
    case 1:
        r = 0.2784313725490196 - fraction_part * 0.03529411764705884;
        g = 0.16470588235294117 + fraction_part * 0.16470588235294117;
        b = 0.48627450980392156 + fraction_part * 0.05882352941176466;
        break;
    case 2:
        r = 0.24313725490196078 - fraction_part * 0.03137254901960784;
        g = 0.32941176470588235 + fraction_part * 0.1215686274509804;
        b = 0.5450980392156862 + fraction_part * 0.007843137254902044;
        break;
    case 3:
        r = 0.21176470588235294 - fraction_part * 0.01568627450980392;
        g = 0.45098039215686275 + fraction_part * 0.10980392156862745;
        b = 0.5529411764705883 - fraction_part * 0.007843137254902044;
        break;
    case 4:
        r = 0.19607843137254902 + fraction_part * 0.023529411764705882;
        g = 0.5607843137254902 + fraction_part * 0.11372549019607847;
        b = 0.5450980392156862 - fraction_part * 0.039215686274509776;
        break;
    case 5:
        r = 0.2196078431372549 + fraction_part * 0.1215686274509804;
        g = 0.6745098039215687 + fraction_part * 0.09411764705882353;
        b = 0.5058823529411764 - fraction_part * 0.0784313725490196;
        break;
    case 6:
        r = 0.3411764705882353 + fraction_part * 0.2823529411764706;
        g = 0.7686274509803922 + fraction_part * 0.09411764705882353;
        b = 0.42745098039215684 - fraction_part * 0.17254901960784313;
        break;
    case 7:
        r = 0.6235294117647059 + fraction_part * 0.3411764705882353;
        g = 0.8627450980392157 + fraction_part * 0.050980392156862675;
        b = 0.2549019607843137 - fraction_part * 0.0549019607843137;
        break;
    }

    return vec3(r, g, b);
}

vec3 calculateColormapJet(float value)
{
    if(value < 0.0)
    {
        value = 0.0;
    }
    else if(value >= 1.0)
    {
        value = 0.99999;
    }

    float full_range_value = value * 6.0;
    float integer_part = floor(full_range_value);
    float fraction_part = full_range_value - integer_part;

    float r = 0.0, g = 0.0, b = 0.0;

    switch(int(integer_part))
    {
    case 0:
        r = 0.07450980392156863 + fraction_part * 0.11372549019607843;
        g = fraction_part * 0.047058823529411764;
        b = 0.51 + fraction_part * 0.48235294117647065;
        break;
    case 1:
        r = 0.18823529411764706 + fraction_part * 0.0784313725490196;
        g = 0.047058823529411764 + fraction_part * 0.8509803921568628;
        b = 0.9923529411764707 - fraction_part * 0.03137254901960784;
        break;
    case 2:
        r = 0.26666666666666666 + fraction_part * 0.3137254901960785;
        g = 0.8980392156862745 + fraction_part * 0.10196078431372546;
        b = 0.9609803921568628 - fraction_part * 0.5372549019607844;
        break;
    case 3:
        r = 0.5803921568627451 + fraction_part * 0.3999999999999999;
        g = 1.0 - fraction_part * 0.196078431372549;
        b = 0.4237254901960784 - fraction_part * 0.2901960784313725;
        break;
    case 4:
        r = 0.9803921568627451 - fraction_part * 0.0117647058823529;
        g = 0.803921568627451 - fraction_part * 0.611764705882353;
        b = 0.13352941176470587 - fraction_part * 0.05098039215686273;
        break;
    case 5:
        r = 0.9686274509803922 - fraction_part * 0.47058823529411764;
        g = 0.19215686274509805 - fraction_part * 0.19215686274509805;
        b = 0.08254901960784314 - fraction_part * 0.06274509803921569;
        break;
    }

    return vec3(r, g, b);
}

vec3 calculateColormapRainbowPastel(float value)
{
    if(value < 0.0)
    {
        value = 0.0;
    }
    else if(value >= 1.0)
    {
        value = 0.99999;
    }

    float full_range_value = value * 5.0;
    float integer_part = floor(full_range_value);
    float fraction_part = full_range_value - integer_part;

    float r = 0.0, g = 0.0, b = 0.0;

    switch(int(integer_part))
    {
    case 0:
        r = 1.0;
        g = 0.6039215686274509 + fraction_part * 0.11372549019607847;
        b = 0.6352941176470588 + fraction_part * 0.06274509803921569;
        break;
    case 1:
        r = 1.0;
        g = 0.7176470588235294 + fraction_part * 0.13725490196078427;
        b = 0.6980392156862745 + fraction_part * 0.06274509803921569;
        break;
    case 2:
        r = 1.0 - fraction_part * 0.11372549019607847;
        g = 0.8549019607843137 + fraction_part * 0.0862745098039216;
        b = 0.7607843137254902 + fraction_part * 0.04313725490196085;
        break;
    case 3:
        r = 0.8862745098039215 - fraction_part * 0.180392156862745;
        g = 0.9411764705882353 - fraction_part * 0.02352941176470591;
        b = 0.803921568627451 + fraction_part * 0.04313725490196074;
        break;
    case 4:
        r = 0.7058823529411765 + fraction_part * 0.07450980392156858;
        g = 0.9176470588235294 - fraction_part * 0.1098039215686274;
        b = 0.8470588235294118 + fraction_part * 0.06666666666666665;
        break;
    }

    return vec3(r, g, b);
}

void main()
{
    gl_Position = model_view_proj_mat * vec4(in_vertex.x, in_vertex.y, in_vertex.z, 1.0);
    coord_out = vec4(in_vertex.x, in_vertex.y, in_vertex.z, 1.0);

    if(has_color_vec == int(1))
    {
        fragment_color = in_color;
    }
    else if(has_distance_from == int(1))
    {
        float d;

        if(distance_from_type == int(0)) // X
        {
            d = length(in_vertex.x - distance_from_point.x);
        }
        else if(distance_from_type == int(1)) // Y
        {
            d = length(in_vertex.y - distance_from_point.y);
        }
        else if(distance_from_type == int(2)) // Z
        {
            d = length(in_vertex.z - distance_from_point.z);
        }
        else if(distance_from_type == int(3)) // XY
        {
            d = length(in_vertex.xy - distance_from_point.xy);
        }
        else if(distance_from_type == int(4)) // XZ
        {
            d = length(in_vertex.xz - distance_from_point.xz);
        }
        else if(distance_from_type == int(5)) // YZ
        {
            d = length(in_vertex.yz - distance_from_point.yz);
        }
        else if(distance_from_type == int(6)) // XYZ
        {
            d = length(in_vertex - distance_from_point);
        }

        float delta = max_dist - min_dist;

        if(color_map_selection == 1)
        {
            fragment_color = calculateColormapJet((d - min_dist) / delta);
        }
        else if(color_map_selection == 2)
        {
            fragment_color = calculateColormapRainbow((d - min_dist) / delta);
        }
        else if(color_map_selection == 3)
        {
            fragment_color = calculateColormapMagma((d - min_dist) / delta);
        }
        else if(color_map_selection == 4)
        {
            fragment_color = calculateColormapViridis((d - min_dist) / delta);
        }
        else if(color_map_selection == 5)
        {
            fragment_color = calculateColormapRainbowPastel((d - min_dist) / delta);
        }
        else
        {
            fragment_color = calculateColormapJet((d - min_dist) / delta);
        }
    }
    else
    {
        fragment_color = vertex_color;
    }

    if(has_point_sizes_vec == int(1))
    {
        // gl_PointSize = point_size * in_point_sizes;
        gl_PointSize = in_point_sizes;
    }
    else
    {
        gl_PointSize = point_size;
    }
}
