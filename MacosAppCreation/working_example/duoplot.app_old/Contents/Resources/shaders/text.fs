#version 330 core

// layout (binding = 0) uniform sampler2D text
in vec2 TexCoords;
out vec4 color;

uniform sampler2D text_sampler;
uniform vec3 text_color;

void main()
{
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text_sampler, TexCoords).r);
    color = vec4(text_color, 1.0) * sampled;
}
