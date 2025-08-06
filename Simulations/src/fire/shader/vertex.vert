#version 330 core

layout (location = 0) in vec3 startPos;
layout (location = 1) in vec3 vel;
layout (location = 2) in float maxLifeTime;
layout (location = 3) in float startSize;
layout (location = 4) in float rotation;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform float elapsedTime;

out VS_OUT {
    vec4 color;
    float size;
    float rotation;
} vs_out;

float interpolateAlpha(float t);

void main()
{	
	float currentTime = elapsedTime;
	if (elapsedTime > maxLifeTime)
		currentTime = mod(elapsedTime, maxLifeTime);
	
	vec3 position = startPos + vel * currentTime;
	gl_Position = vec4(position, 1.0f);
	vs_out.size = max(0.1f, startSize - currentTime);
	vs_out.color = vec4(interpolateAlpha(currentTime));
	
	float rotPerTimestep = rotation / maxLifeTime;
	vs_out.rotation = rotPerTimestep * currentTime;
}

float interpolateAlpha(float t)
{
	vec2 alphaControlPoints[4] = vec2[](
		vec2(0.0f, 0.0f),
		vec2(0.1f, 0.2f),
		vec2(0.6f, 1.0f),
		vec2(1.0f, 0.0f)
	);

	float normalizedT = t / maxLifeTime;
	int firstPointIndex = 0;
	for (int i = 0; i < alphaControlPoints.length; i++)
	{
		if (alphaControlPoints[i].x >= normalizedT)
			break;
		firstPointIndex = i;
	}

	vec2 a = alphaControlPoints[firstPointIndex];
	vec2 b = alphaControlPoints[min(alphaControlPoints.length -1, firstPointIndex+1)];

	float factor = (normalizedT - a.x) / (b.x - a.x);
    return mix(a.y, b.y, factor);
}