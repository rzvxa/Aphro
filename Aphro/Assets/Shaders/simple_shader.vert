#version 450

layout(location = 0) in vec3 iposition;
layout(location = 1) in vec3 icolor;
layout(location = 2) in vec3 inormal;
layout(location = 3) in vec2 iuv;

layout(location = 0) out vec3 ocolor;

layout(push_constant) uniform Push {
	mat4 transform; // projection * view * model
	mat4 normalMatrix;
} push;

const vec3 DIRECTION_TO_LIGHT = normalize(vec3(1.0, -3.0, -1.0));
const float AMBIENT = 0.2;

void main() {
	gl_Position = push.transform * vec4(iposition, 1.0);

	// Uniform
	//	vec3 normalWorldSpace = normalize(mat3(push.modelMatrix) * inormal);

	//	mat3 normalMatrix = transpose(inverse(mat3(push.modelMatrix)));
	//	vec3 normalWorldSpace = normalize(normalMatrix * inormal);

	vec3 normalWorldSpace = normalize(mat3(push.normalMatrix) * inormal);

	float lightIntensity = AMBIENT + max(dot(normalWorldSpace, DIRECTION_TO_LIGHT), 0);

	ocolor = lightIntensity * icolor;
}