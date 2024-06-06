#version 330

uniform vec3 partColor;

out vec4 color;

void main()
{
	color = vec4(partColor,1.0f);
}