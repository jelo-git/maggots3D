#version 330

uniform vec4 lightColor;
uniform vec3 viewPos;

uniform sampler2D textureBase;

in vec3 iPosition;
in vec3 iNormal;
in vec2 iTexCoord;

out vec4 color;

void main() {

	// ambient
	float ambient = 0.10;

	// diffuse
	vec3 lightSource = normalize(vec3(1.0,1.0,1.0));
	vec3 normal = normalize(iNormal);
	float diff = max(dot(normal, lightSource), 0.0);

	// specular
	float spec = 0.0;
	if(diff != 0.0){
		float specularStrength = 0.4;
		vec3 viewDir = normalize(viewPos-iPosition);
		vec3 halfdir = normalize(viewDir+lightSource);
		float spec = pow(max(dot(viewDir, halfdir), 0.0), 32) * specularStrength;
	}

	// combine results
	color = texture(textureBase, iTexCoord) * lightColor * (diff + ambient + spec);
}