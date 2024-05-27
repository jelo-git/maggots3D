#version 330

//uniform sampler2D tex0;

in vec4 n;
in vec4 l;
in vec4 v;
//in vec4 fragColor;
//in vec2 fragTexCoord;

out vec4 color;

void main() {

	//Znormalizowane interpolowane wektory
	vec4 ml = normalize(l);
	vec4 mn = normalize(n);
	vec4 mv = normalize(v);
	//Wektor odbity
	vec4 mr = reflect(-ml, mn);

    //color = fragColor;
    //color = texture(tex0, fragTexCoord);
    vec4 fragColor = vec4(1.0, 0.5, 0.0, 1.0);

	//Obliczenie modelu oœwietlenia
	float nl = clamp(dot(mn, ml), 0, 1);
	float rv = pow(clamp(dot(mr, mv), 0, 1),25);
	color= vec4(fragColor.rgb * nl, fragColor.a) + vec4(fragColor.rgb*rv, 0);

}