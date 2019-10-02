#version 450
#extension GL_ARB_separate_shader_objects : enable


layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragUVCoord;
layout(location = 2) in vec3 fragNormal;
layout(location = 3) in vec3 fragViewVector;
layout(location = 4) in vec3 fragLightVector;

layout(location = 0) out vec4 outColor;

layout(binding = 1) uniform sampler2D tex;

void main() 
{
	//outColor = vec4(fragColor, 1.0);
	//outColor = texture(tex, fragUVCoord);

	vec3 N = normalize(fragNormal);
	vec3 L = normalize(fragLightVector);
	vec3 V = normalize(fragViewVector);
	vec3 R = reflect(L, N);

	vec3 ambient = fragColor * 0.1;
	vec3 diffuse = max(dot(N, L), 0.0) * fragColor;
	vec3 specular = pow(max(dot(R, V), 0.0), 16.0) * vec3(1.35);

	outColor = vec4(ambient + diffuse + specular, 1.0);
}