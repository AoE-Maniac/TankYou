#pragma once

#include <Kore/Graphics/Graphics.h>

class Particle;

class ParticleSystem {
public:
	ParticleSystem(Kore::vec3 pos, Kore::vec3 dir, float size, float timeToLive, Kore::vec4 colorS, Kore::vec4 colorE, float grav, int maxParticles, Kore::VertexStructure** structures, Kore::Texture* image);

    ~ParticleSystem();
	void setPosition(Kore::vec3 position);
	void setDirection(Kore::vec3 direction);
	void update(float deltaTime);
	void render(Kore::TextureUnit tex, Kore::ConstantLocation vLocation, Kore::ConstantLocation tintLocation, Kore::mat4 V);

//private:
	Kore::VertexBuffer** vbs;
	Kore::IndexBuffer* ib;
	Kore::Texture* texture;

	// The coordinates of the emitter box
	Kore::vec3 emitMin;
	Kore::vec3 emitMax;

	// The direction of the emission
	Kore::vec3 emitDir;

	// Particle data
	Kore::vec3* particlePos; // The current position
	Kore::vec3* particleVel; // The current velocity
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
	Kore::vec4 colorStart;

	// The end color
	Kore::vec4 colorEnd;

	// The number of particles
	float gravity;
    float spawnArea;

	void init(float halfSize, int maxParticles, Kore::VertexStructure** structures);
	void emitParticle(int index);
	float getRandom(float minValue, float maxValue);
};