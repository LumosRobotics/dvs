#version 330 core

uniform float zoomscale;

layout(location = 0) in vec2 position;
layout(location = 1) in vec2 origin;
layout(location = 2) in vec2 textureCoord;
layout(location = 3) in vec3 color;

out vec3 v_textureColor;
out vec2 v_textureCoord;

void main()
{
	mat4 scalingMatrix = mat4(1.0) * zoomscale;
	scalingMatrix[3][3] = 1.0;

	vec4 finalPosition   = scalingMatrix * vec4(position, 0.0, 1.0);
	vec4 finalTextOrigin = scalingMatrix * vec4(origin,   0.0, 1.0);

	// Offset from origin, un-zoomed, so text size stays fixed in screen space.
	vec2 scaled_pt = vec2(finalPosition.x - finalTextOrigin.x,
	                      finalPosition.y - finalTextOrigin.y) / zoomscale;

	gl_Position    = vec4(finalPosition.x + scaled_pt.x,
	                      finalPosition.y + scaled_pt.y, 0.0, 1.0);
	v_textureCoord = textureCoord;
	v_textureColor = color;
}
