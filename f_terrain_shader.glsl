#version 330

uniform vec4 lightColor;
uniform vec3 viewPos;

uniform vec3 lightPos;
uniform float expStr;

uniform sampler2D textureBase;

in vec3 iPosition;
in vec3 iNormal;
in vec2 iTexCoord;

out vec4 color;

vec4 pointLight(){
	// diffuse
	vec3 lightSource = normalize(lightPos - iPosition);
	vec3 normal = normalize(iNormal);
	float diff = max(dot(normal, lightSource), 0.0);

	// specular
	float spec = 0.0;
	if(diff != 0.0){
		float specularStrength = 0.6;
		vec3 viewDir = normalize(viewPos-iPosition);
		vec3 halfdir = normalize(viewDir+lightSource);
		float spec = pow(max(dot(viewDir, halfdir), 0.0), 32) * specularStrength;
	}

	// intensity
	float distance = length(lightPos - iPosition);
	float inten = 1.0 / (1.0 + 1.2 * distance + 0.5 * distance * distance);

	// combine results
	return (texture(textureBase, iTexCoord) * vec4(0.85, 0.8, 0.01, 1.0) * (diff + spec) * inten);
}

vec4 ambientnight(){
	// ambient
	float ambient = 0.1;
	// diffuse
	vec3 lightSource = normalize(vec3(1.0,3.0,1.0));
	vec3 normal = normalize(iNormal);
	float diff = max(dot(normal, lightSource), 0.0);

	// specular
	float specularStrength = 0.1;
	vec3 viewDir = normalize(viewPos-iPosition);
	vec3 halfdir = reflect(-lightSource, normal);
	float spec = pow(max(dot(viewDir, halfdir), 0.0), 2) * specularStrength;

	// combine results
	return (texture(textureBase, iTexCoord) * vec4(0.3,0.3,0.3,1.0) * (diff + ambient + spec));
}

vec4 sloncze(){
	// diffuse
	vec3 lightSource = normalize(vec3(-1.0,0.3,-1.0));
	vec3 normal = normalize(iNormal);
	float diff = max(dot(normal, lightSource), 0.0);

	// specular
	float specularStrength = 0.3;
	vec3 viewDir = normalize(viewPos-iPosition);
	vec3 halfdir = reflect(-lightSource, normal);
	float spec = pow(max(dot(viewDir, halfdir), 0.0), 16) * specularStrength;

	// combine results
	return (texture(textureBase, iTexCoord) * vec4(0.9,0.8,0.6,1.0) * (diff + spec));
}


void main() {
	color = (ambientnight()*0.4 + sloncze()*0.7) + pointLight() * expStr;
}