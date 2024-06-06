#version 330

layout(location = 0) in vec3 positon;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;

out vec3 iPosition;
out vec3 iNormal;
out vec2 iTexCoord;


void main(){
    vec4 vertex = vec4(positon, 1.0);

    iPosition = vec3(M * vertex);

    mat3 normalMatrix = transpose(inverse(mat3(M)));
    iNormal = normalMatrix * normal;

    iTexCoord = texCoord;

    gl_Position = P * V * M * vertex;
}