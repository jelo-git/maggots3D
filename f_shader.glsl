#version 330

uniform vec4 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;

uniform sampler2D tex0;
uniform sampler2D tex1;

in vec3 texCoord;
in vec3 normal;
in vec3 position;

out vec4 color;

vec4 pointLight(){
	//ambient
	float ambient = 0.2;

	//light vec
	vec3 lightvec = lightPos - position;

	//diffuse
	vec3 n = normalize(normal);
	vec3 lightDir = normalize(lightvec);
	float diff = max(dot(n, lightDir), 0.0);

	//specular
	float specularStrength = 0.5;
	vec3 viewDir = normalize(viewPos - position);
	vec3 reflectDir = reflect(-lightDir, n);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 4) * specularStrength;

	//intensity
	float distance = length(lightvec);
	float a = 2.5;
	float b = 0.6;
	float intensity = 1.0 / (length(lightvec) * length(lightvec));

	//combine results
	return (texture(tex0, texCoord) * (diff*intensity+ambient) + texture(tex1,texCoord).r * spec*intensity) * lightColor;
}

vec4 directLight(){
	//ambient
	float ambient = 0.2;

	//diffuse
	vec3 n = normalize(normal);
	vec3 posOfLight = vec3(9999.0f,9999.0f,0.0f);
	vec3 lightDir = normalize(vec3(posOfLight));
	float diff = max(dot(n, lightDir), 0.0);

	//specular
	float specularStrength = 0.5;
	vec3 viewDir = normalize(viewPos - position);
	vec3 reflectDir = reflect(-lightDir, n);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 2) * specularStrength;

	//combine results
	return (texture(tex0, texCoord) * (diff+ambient) + texture(tex1,texCoord).r * spec) * lightColor;
}

void main() {
	color = directLight();
}