#version 330

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 normal;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;

out vec3 fragNormal;
out vec3 fragPos;


void main(){
    vec4 vertex = vec4(pos, 1.0);

    fragNormal = normal;
    fragPos = vec3(M * vertex);

    gl_Position = P * V * M * vertex;
}