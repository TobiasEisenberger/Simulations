#version 330 core

// color accumulation buffer
uniform sampler2D accum;

// revealage threshold buffer
uniform sampler2D reveal;

// epsilon number
const float EPSILON = 0.00001f;

out vec4 frag;

// calculate floating point numbers equality accurately
bool isApproximatelyEqual(float a, float b)
{
	return abs(a - b) <= (abs(a) < abs(b) ? abs(b) : abs(a)) * EPSILON;
}

// get the max value between three values
float max3(vec3 v) 
{
	return max(max(v.x, v.y), v.z);
}

void main()
{
	ivec2 coords = ivec2(gl_FragCoord.xy);
	
	float revealage = texelFetch(reveal, coords, 0).r;
	if (isApproximatelyEqual(revealage, 1.0f)) 
		discard;

	vec4 accumulation = texelFetch(accum, coords, 0);
//	if (isinf(max3(abs(accumulation.rgb)))) 
//		accumulation.rgb = vec3(accumulation.a);

	accumulation.a = revealage;
	frag = vec4(accumulation.rgb / max(accumulation.a, EPSILON), 1.0 - revealage);
}