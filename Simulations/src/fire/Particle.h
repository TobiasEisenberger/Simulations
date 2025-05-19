#pragma once

#include <GLM/vec3.hpp>

struct Particle
{
	glm::vec3 position;
	glm::vec3 velocity;
	float maxLifetimeSeconds;
	float size;
	float rotation;
};