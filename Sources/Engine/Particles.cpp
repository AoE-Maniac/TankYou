#include "pch.h"

#include "Particles.h"

#include <Kore/Graphics/Graphics.h>
#include <Kore/Math/Random.h>

#include "Rendering.h"

using namespace Kore;

// A simple particle implementation
class Particle {
public:
	VertexBuffer* vb;
	IndexBuffer* ib;

	mat4 M;

	// The current position
	vec3 position;

	// The current velocity
	vec3 velocity;

	// The remaining time to live
	float timeToLive;

	// The total time time to live
	float totalTimeToLive;

	// Is the particle dead (= ready to be re-spawned?)
	bool dead;

	// The beginning color
	vec4 colorStart;

	// The end color
	vec4 colorEnd;

	void init(const VertexStructure& structure) {
		vb = new VertexBuffer(4, structure, 0);
		float* vertices = vb->lock();
		SetVertex(vertices, 0, -1, -1, 0, 0, 0);
		SetVertex(vertices, 1, -1, 1, 0, 0, 1);
		SetVertex(vertices, 2, 1, 1, 0, 1, 1);
		SetVertex(vertices, 3, 1, -1, 0, 1, 0);
		vb->unlock();

		// Set index buffer
		ib = new IndexBuffer(6);
		int* indices = ib->lock();
		indices[0] = 0;
		indices[1] = 1;
		indices[2] = 2;
		indices[3] = 0;
		indices[4] = 2;
		indices[5] = 3;
		ib->unlock();

		dead = true;
	}

	void Emit(vec3 pos, vec3 velocity, float timeToLive, vec4 colorStart, vec4 colorEnd) {
		position = pos;
		this->velocity = velocity;
		dead = false;
		this->timeToLive = timeToLive;
		totalTimeToLive = timeToLive;
		this->colorStart = colorStart;
		this->colorEnd = colorEnd;
	}

	Particle() {
	}

	void SetVertex(float* vertices, int index, float x, float y, float z, float u, float v) {
		vertices[index * 8 + 0] = x;
		vertices[index * 8 + 1] = y;
		vertices[index * 8 + 2] = z;
		vertices[index * 8 + 3] = u;
		vertices[index * 8 + 4] = v;
		vertices[index * 8 + 5] = 0.0f;
		vertices[index * 8 + 6] = 0.0f;
		vertices[index * 8 + 7] = -1.0f;
	}

	void render(TextureUnit tex, Texture* image) {
		Graphics::setTexture(tex, image);
		Graphics::setVertexBuffer(*vb);
		Graphics::setIndexBuffer(*ib);
		Graphics::drawIndexedVertices();
	}

	void Integrate(float deltaTime) {
		timeToLive -= deltaTime;

		if (timeToLive < 0.0f) {
			dead = true;
		}

		// Note: We are using no forces or gravity at the moment.

		position += velocity * deltaTime;

		// Build the matrix
		M = mat4::Translation(position.x(), position.y(), position.z()) * mat4::Scale(0.2f, 0.2f, 0.2f);
	}
};

ParticleSystem::ParticleSystem(vec3 pos, int maxParticles, const VertexStructure& structure) {
	particles = new Particle[maxParticles];
	numParticles = maxParticles;
	for (int i = 0; i < maxParticles; i++) {
		particles[i].init(structure);
	}
	spawnRate = 0.05f;
	nextSpawn = spawnRate;

	SetPosition(pos);
}

void ParticleSystem::update(float deltaTime) {
	// Do we need to spawn a particle?
	nextSpawn -= deltaTime;
	bool spawnParticle = false;
	if (nextSpawn < 0) {
		spawnParticle = true;
		nextSpawn = spawnRate;
	}

	for (int i = 0; i < numParticles; i++) {

		if (particles[i].dead) {
			if (spawnParticle) {
				EmitParticle(i);
				spawnParticle = false;
			}
		}

		particles[i].Integrate(deltaTime);
	}
}

void ParticleSystem::render(TextureUnit tex, Texture* image, ConstantLocation vLocation, ConstantLocation mLocation, ConstantLocation nLocation, ConstantLocation tintLocation, mat4 V) {
	Graphics::setBlendingMode(BlendingOperation::SourceAlpha, BlendingOperation::InverseSourceAlpha);
	Graphics::setRenderState(RenderState::DepthWrite, false);
	
	Graphics::setMatrix(vLocation, V);

	mat4 view = V.Invert();
	view.Set(0, 3, 0.0f);
	view.Set(1, 3, 0.0f);
	view.Set(2, 3, 0.0f);

	for (int i = 0; i < numParticles; i++) {
		// Skip dead particles
		if (particles[i].dead) continue;

		// Interpolate linearly between the two colors
		float interpolation = particles[i].timeToLive / particles[i].totalTimeToLive;
		Graphics::setFloat4(tintLocation, particles[i].colorStart * interpolation + particles[i].colorEnd * (1.0f - interpolation));

		Graphics::setMatrix(mLocation, particles[i].M * view);
		Graphics::setMatrix(nLocation, calculateN(particles[i].M * view));

		particles[i].render(tex, image);
	}
	Graphics::setRenderState(RenderState::DepthWrite, true);
}

float ParticleSystem::getRandom(float minValue, float maxValue) {
	int randMax = 1000000;
	int randInt = Random::get(0, randMax);
	float r = (float)randInt / (float)randMax;
	return minValue + r * (maxValue - minValue);
}

void ParticleSystem::EmitParticle(int index) {
	// Calculate a random position inside the box
	float x = getRandom(emitMin.x(), emitMax.x());
	float y = getRandom(emitMin.y(), emitMax.y());
	float z = getRandom(emitMin.z(), emitMax.z());

	vec3 pos;
	pos.set(x, y, z);

	vec3 velocity(0, 0.3f, 0);

	particles[index].Emit(pos, velocity, 3.0f, vec4(2.5f, 0, 0, 1), vec4(0, 0, 0, 0));
}