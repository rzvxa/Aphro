#version 450

layout (location = 0) in vec3 icolor;
layout (location = 0) out vec4 ocolor;

void main() {
	ocolor = vec4(icolor, 1.0);
}