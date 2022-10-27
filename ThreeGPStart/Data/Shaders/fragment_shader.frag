#version 330

uniform vec4 diffuse_colour;
uniform sampler2D sampler_tex;

uniform vec3 lightPosition;
uniform vec3 lightIntensity;

in vec3 varying_normal;
in vec2 varying_coord;
in vec3 varying_pos;

out vec4 fragment_colour;

void main(void)
{
	vec3 tex_colour = texture(sampler_tex, varying_coord).rgb;

	vec3 N = normalize(varying_normal);
	vec3 lightDirection = vec3(0,-1,-0.5);
	vec3 L = normalize(-lightDirection);
	
	float ambientIntensity = 0.05;
    vec3 ambientColour = tex_colour;

	float lightIntensity = max(dot(L,N),0);
	vec3 lightColour = vec3(1);

	vec3 pointLightdirec = lightPosition - varying_pos;
	L = normalize(pointLightdirec);
	float pointLightIntensity = max(dot(L,N),0);

    vec3 result = ambientIntensity * ambientColour + tex_colour * (lightIntensity + lightColour + pointLightIntensity * vec3(3,0,3));

	fragment_colour = vec4(result, 1.0);
}