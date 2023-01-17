#version 450

layout(location = 0) in vec3 iposition;
layout(location = 1) in vec3 icolor;

layout(location = 0) out vec3 ocolor;

layout(push_constant) uniform Push {
	mat4 transform;
	vec3 color;
} push;

void main() {
	gl_Position = push.transform * vec4(iposition, 1.0);
	ocolor = icolor;
}