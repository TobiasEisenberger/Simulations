#pragma once

#include "Particle.h"
#include <vector>
#include <random>

class ParticleSystem
{
public:
	ParticleSystem(int count);
	const std::vector<Particle>& GetParticles();

private:
	std::vector<Particle> particles;
	void InitParticles();
	bool Sort(Particle i, Particle j);
};