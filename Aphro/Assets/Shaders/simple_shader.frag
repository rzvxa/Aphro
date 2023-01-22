#version 450

layout (location = 0) in vec3 icolor;

layout (location = 0) out vec4 ocolor;

layout(push_constant) uniform Push {
	mat4 transform; // projection * view * model
	mat4 normalMatrix;
} push;

void main() {
	ocolor = vec4(icolor, 1.0);
}