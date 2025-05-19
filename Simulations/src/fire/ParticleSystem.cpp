#include "ParticleSystem.h"
#include "Random.h";
#include "GLM/trigonometric.hpp"

#include <iostream>
#include <functional>

ParticleSystem::ParticleSystem(int count)
{
	particles.resize(count);
	InitParticles();
}

void ParticleSystem::InitParticles()
{
	float offset = 0;
	float radiansPerVertex = glm::radians(360.0f / particles.size());
	float radius = 0.5f;
	for (int i = 0; i < particles.size(); i++)
	{
		Particle& particle = particles[i];

		// Position
		particle.position.x = glm::cos(radiansPerVertex * (i + 1)) * radius;
		particle.position.y = 0.0f;
		particle.position.z = glm::sin(radiansPerVertex * (i + 1)) * radius;

		// Velocity
		particle.velocity.x = 0.0f;
		particle.velocity.y = (Random::Float() * 6 + 0.5f);
		particle.velocity.z = 0.0f;

		// Max lifetime
		particle.maxLifetimeSeconds = Random::Float() + 0.5f;

		// Initial size
		particle.size = Random::Float() + 0.5f;

		particle.rotation = ((Random::Float() * 2) - 1) * 45.0f;

		std::cout << "Particle: " << i << " Velocity: " << particle.velocity.x << ", " << particle.velocity.y << ", " << particle.velocity.z << "\n";
		std::cout << "Particle: " << i << " Max lifetime: " << particle.maxLifetimeSeconds << "\n\n";
	}

	std::sort(particles.begin(), particles.end(), std::bind(&ParticleSystem::Sort, this, std::placeholders::_1, std::placeholders::_2));
}

bool ParticleSystem::Sort(Particle i, Particle j)
{
	return i.position.z < j.position.z;
}

const std::vector<Particle>& ParticleSystem::GetParticles()
{
	return particles;
}