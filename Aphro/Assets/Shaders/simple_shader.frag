#version 450

layout(location = 1) in vec3 iworldPosition;
layout(location = 2) in vec3 iworldNormal;
layout(location = 0) in vec3 icolor;

layout (location = 0) out vec4 ocolor;

layout(set = 0, binding = 0) uniform GlobalUbo {
	mat4 projectionViewMatrix;
//	vec3 directionToLight;
	vec4 ambientLightColor;
	vec3 lightPosition;
	vec4 lightColor;
} ubo;

layout(push_constant) uniform Push {
	mat4 modelMatrix;
	mat4 normalMatrix;
} push;

void main() {
	vec3 lightDirection = ubo.lightPosition - iworldPosition;
	float attenuation = 1.0f / dot(lightDirection, lightDirection);

	vec3 lightColor = ubo.lightColor.xyz * ubo.lightColor.w * attenuation;
	vec3 ambientLight = ubo.ambientLightColor.xyz * ubo.ambientLightColor.w;

	vec3 diffuseLight = lightColor * max(dot(normalize(iworldNormal), normalize(lightDirection)), 0);

	ocolor = vec4((diffuseLight + ambientLight) * icolor, 1.0);
}