#version 330

uniform sampler2D tex0;

in vec4 fragColor;
in vec2 fragTexCoord;

out vec4 color;

void main() {
    //color = fragColor;
    color = texture(tex0, fragTexCoord);
}