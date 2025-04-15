#version 330

out vec4 output_color;
in vec4 fragment_color;
uniform int shader_mode;

void main()
{
    if (shader_mode == int(0))  // Normal
    {
        output_color = fragment_color;
    }
    else if (shader_mode == int(1))  // Button
    {
        output_color = fragment_color;
    }
    else if (shader_mode == int(2))  // Slider
    {
        output_color = fragment_color;
    }
    else if (shader_mode == int(3))  // Checkbox
    {
        output_color = fragment_color;
    }
    else if (shader_mode == int(4))  // Sizing Rectangles
    {
        output_color = fragment_color;
    }
}
