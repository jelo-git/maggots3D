#version 330

layout(location = 0) in vec4 pos;
layout(location = 1) in vec4 color;
layout(location = 2) in vec2 texCoord;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;

out vec4 fragColor;
out vec2 fragTexCoord;

void main(){
    gl_Position = P * V * M * pos;
    //gl_Position = pos;
    fragColor = color;
    fragTexCoord = texCoord;
}