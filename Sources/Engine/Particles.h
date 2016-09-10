#pragma once

#include <Kore/Graphics/Graphics.h>

using namespace Kore;

class Particle;

class ParticleSystem {
public:
	ParticleSystem(vec3 pos, int maxParticles, const VertexStructure& structure);

	void SetPosition(vec3 pos) {
		position = pos;
		float b = 0.1f;
		emitMin = position + vec3(-b, -b, -b);
		emitMax = position + vec3(b, b, b);
	}

	void update(float deltaTime);
	void render(TextureUnit tex, Texture* image, ConstantLocation vLocation, ConstantLocation mLocation, ConstantLocation nLocation, ConstantLocation tintLocation, Kore::mat4 V);

private:
	// The center of the particle system
	vec3 position;

	// The minimum coordinates of the emitter box
	vec3 emitMin;

	// The maximal coordinates of the emitter box
	vec3 emitMax;

	// The list of particles
	Particle* particles;

	// The number of particles
	int numParticles;

	// The spawn rate
	float spawnRate;

	// When should the next particle be spawned?
	float nextSpawn;

	float getRandom(float minValue, float maxValue);
	void EmitParticle(int index);
};