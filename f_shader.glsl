#version 330

uniform vec4 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;

in vec3 fragNormal;
in vec3 fragPos;

out vec4 color;

vec4 directLight(){
	//ambient
	float ambient = 0.2;

	//diffuse
	vec3 normal = normalize(fragNormal);
	vec3 posOfLight = vec3(9999.0f,9999.0f,0.0f);
	vec3 lightDir = normalize(vec3(posOfLight));
float diff = max(dot(normal, lightDir), 0.0);

	//specular
	float specularStrength = 0.5;
	vec3 viewDir = normalize(viewPos - fragPos);
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 2) * specularStrength;

	//combine results
	return (texture(tex0, texCoord) * (diff+ambient) + texture(tex1,texCoord).r * spec) * lightColor;
}

void main() {
	// diffuse
	vec3 normal = normalize(fragNormal);
	vec3 lightDir = normalize(lightPos - fragPos);

	float diff = max(dot(normal, lightDir), 0.0);

	// specular
	float specularStrength = 0.5;
	vec3 viewDir = normalize(viewPos - fragPos);
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 4) * specularStrength;

	// combine results
	color = vec4(1.0, 1.0, 1.0, 1.0) * lightColor * (diff + spec);
}