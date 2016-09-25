#pragma once

#include <Kore/Graphics/Graphics.h>

using namespace Kore;

class Particle;

class ParticleSystem {
public:
	ParticleSystem(vec3 pos, vec3 dir, float size, float timeToLive, vec4 colorS, vec4 colorE, float grav, int maxParticles, VertexStructure** structures, Texture* image);

	void setPosition(vec3 position);
	void setDirection(vec3 direction);
	void update(float deltaTime);
	void render(TextureUnit tex, ConstantLocation vLocation, ConstantLocation tintLocation, mat4 V);

private:
	VertexBuffer** vbs;
	IndexBuffer* ib;
	Texture* texture;

	// The coordinates of the emitter box
	vec3 emitMin;
	vec3 emitMax;

	// The direction of the emission
	vec3 emitDir;

	// Particle data
	vec3* particlePos; // The current position
	vec3* particleVel; // The current velocity
	float* particleTTL; // The remaining time to live

	// The number of particles
	int numParticles;

	// The spawn rate
	float spawnRate;

	// When should the next particle be spawned?
	float nextSpawn;

	// The total time time to live
	float totalTimeToLive;

	// The beginning color
	vec4 colorStart;

	// The end color
	vec4 colorEnd;

	// The number of particles
	float gravity;

	void init(float halfSize, int maxParticles, VertexStructure** structures);
	void emitParticle(int index);
	float getRandom(float minValue, float maxValue);
};