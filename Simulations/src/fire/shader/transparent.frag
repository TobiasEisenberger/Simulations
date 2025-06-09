#version 330 core

layout (location = 0) out vec4 accum;
layout (location = 1) out float reveal;

uniform vec4 color;

void main()
{
	// weight function
	// float weight = clamp(pow(min(1.0, color.a * 10.0) + 0.01, 3.0) * 1e8 * pow(1.0 - gl_FragCoord.z * 0.9, 3.0), 1e-2, 3e3);
	float weight = color.a * 0.5 + 0.1;
	
	// store pixel color accumulation
	accum = vec4(color.rgb * color.a, color.a) * weight;
	
	// store pixel revealage threshold
	reveal = color.a;
}