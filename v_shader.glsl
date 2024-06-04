#version 330

layout(location = 0) in vec3 iPosition;
layout(location = 1) in vec3 iNormal;
layout(location = 2) in vec2 iTexCoord;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;

out vec3 normal;
out vec3 position;
out vec2 texCoord;


void main(){
    vec4 vertex = vec4(iPosition, 1.0);

    normal = iNormal;
    position = vec3(M * vertex);
    texCoord = iTexCoord;

    gl_Position = P * V * M * vertex;
}