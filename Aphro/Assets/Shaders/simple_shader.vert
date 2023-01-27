#version 450

layout(location = 0) in vec3 iposition;
layout(location = 1) in vec3 icolor;
layout(location = 2) in vec3 inormal;
layout(location = 3) in vec2 iuv;

layout(location = 1) out vec3 oworldPosition;
layout(location = 2) out vec3 oworldNormal;
layout(location = 0) out vec3 ocolor;

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
	vec4 worldPosition = push.modelMatrix * vec4(iposition, 1.0f);
	gl_Position = ubo.projectionViewMatrix * worldPosition;
	oworldPosition = worldPosition.xyz;
	oworldNormal = normalize(mat3(push.normalMatrix) * inormal);
	ocolor = icolor;
}