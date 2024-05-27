#version 330

uniform vec4 lightColor;

out vec4 color;

void main()
{
	color = lightColor;
}