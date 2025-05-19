#version 330 core

in vec4 ageColor;
in vec2 TexCoords;
in float rotation;

uniform sampler2D particleTexture;

out vec4 color;

void main()
{
	// color = vec4(1.0f, 0.5f, 0.2f, 1.0f);
	// color = ageColor;
	
	// rotation
	// float degreesToRad = radians(90.0f);
	float degreesToRad = radians(rotation);
	float cosine = cos(degreesToRad);
	float sine = sin(degreesToRad);
	vec2 rotatedCoords = (TexCoords - 0.5) * mat2(cosine, sine, -sine, cosine) + 0.5;

	// color = texture(particleTexture, TexCoords) * ageColor;
	color = texture(particleTexture, rotatedCoords) * ageColor;
}