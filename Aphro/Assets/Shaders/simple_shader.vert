#version 450

layout(location = 0) in vec2 iposition;
layout(location = 1) in vec3 icolor;

//layout(location = 0) out vec3 ocolor;

layout(push_constant) uniform Push {
	mat2 transform;
	vec2 offset;
	vec3 color;
} push;

void main() {
	gl_Position = vec4(push.transform * iposition + push.offset, 0.0, 1.0);
}