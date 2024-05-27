#version 330

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 normal;
//layout(location = 2) in vec2 texCoord;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;

//out vec4 fragColor;
//out vec2 fragTexCoord;

out vec4 l;
out vec4 n;
out vec4 v;

void main(){
    vec4 vertex = vec4(pos, 1.0);

    vec4 lp = vec4(0, 0, -6, 1); //przestrzeñ œwiata
    l = normalize(V * lp - V*M*vertex); //wektor do œwiat³a w przestrzeni oka
    v = normalize(vec4(0, 0, 0, 1) - V * M * vertex); //wektor do obserwatora w przestrzeni oka
    n = normalize(V * M * vec4(normal, 1.0)); //wektor normalny w przestrzeni oka

    gl_Position = P * V * M * vertex;


    //gl_Position = pos;
    //fragColor = color;
    //fragTexCoord = texCoord;
}