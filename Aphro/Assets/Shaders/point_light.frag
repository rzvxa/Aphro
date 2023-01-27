#version 450

layout (location = 0) in vec2 ioffset;

layout (location = 0) out vec4 ocolor;

struct PointLight {
	vec4 position; // ignore w
	vec4 color; // w is intensity
};

layout(set = 0, binding = 0) uniform GlobalUbo {
	mat4 projection;
	mat4 view;
	vec4 ambientLightColor;
	PointLight pointLights[10 /* replace const with specialization constants */];
	int numLights;
} ubo;

layout(push_constant) uniform Push {
	vec4 position;
	vec4 color;
	float radius;
} push;

void main() {
	float dis = sqrt(dot(ioffset, ioffset));
//	float alpha = step(1, dis);
//	ocolor = vec4(ubo.lightColor.xyz, alpha);
	if (dis >= 1)
		discard;
	ocolor = vec4(push.color.xyz, 1.0);
}