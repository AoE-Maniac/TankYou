#include "pch.h"

#include "Particles.h"

#include <Kore/Graphics/Graphics.h>
#include <Kore/Math/Random.h>

#include "Rendering.h"

using namespace Kore;

ParticleSystem::ParticleSystem(vec3 pos, vec3 dir, float size, float timeToLive, vec4 colorS, vec4 colorE, float grav, int maxParticles, const VertexStructure& structure, Texture* image) :
	colorStart(colorS),
	colorEnd(colorE),
	gravity(grav),
	totalTimeToLive(timeToLive),
	numParticles(maxParticles),
	texture (image) {

	particlePos = new vec3[maxParticles];
	particleVel = new vec3[maxParticles];
	particleTTL = new float[maxParticles];
	
	spawnRate = 0.05f;
	nextSpawn = spawnRate;

	init(size / 2, structure);
	setPosition(pos);
	setDirection(dir);
}

void ParticleSystem::init(float halfSize, const VertexStructure& structure) {
	vb = new VertexBuffer(4, structure, 0);
	float* vertices = vb->lock();
	setVertex(vertices, 0, -1 * halfSize, -1 * halfSize, 0, 0, 0);
	setVertex(vertices, 1, -1 * halfSize, 1 * halfSize, 0, 0, 1);
	setVertex(vertices, 2, 1 * halfSize, 1 * halfSize, 0, 1, 1);
	setVertex(vertices, 3, 1 * halfSize, -1 * halfSize, 0, 1, 0);
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
}

void ParticleSystem::setPosition(vec3 position) {
	float b = 0.1f;
	emitMin = position + vec3(-b, -b, -b);
	emitMax = position + vec3(b, b, b);
}

void ParticleSystem::setDirection(vec3 direction) {
	emitDir = direction;
}

void ParticleSystem::setVertex(float* vertices, int index, float x, float y, float z, float u, float v) {
	vertices[index * 8 + 0] = x;
	vertices[index * 8 + 1] = y;
	vertices[index * 8 + 2] = z;
	vertices[index * 8 + 3] = u;
	vertices[index * 8 + 4] = v;
	vertices[index * 8 + 5] = 0.0f;
	vertices[index * 8 + 6] = 0.0f;
	vertices[index * 8 + 7] = -1.0f;
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

		if (particleTTL[i] < 0.0f) {
			if (spawnParticle) {
				emitParticle(i);
				spawnParticle = false;
			}
		}

		particleTTL[i] -= deltaTime;
		particleVel[i] += vec3(0, -gravity * deltaTime, 0);
		particlePos[i] += particleVel[i] * deltaTime;
	}
}

void ParticleSystem::render(TextureUnit tex, ConstantLocation vLocation, ConstantLocation mLocation, ConstantLocation nLocation, ConstantLocation tintLocation, mat4 V) {
	Graphics::setBlendingMode(BlendingOperation::SourceAlpha, BlendingOperation::InverseSourceAlpha);
	Graphics::setRenderState(RenderState::DepthWrite, false);
	
	Graphics::setMatrix(vLocation, V);

	mat4 view = V.Invert();
	view.Set(0, 3, 0.0f);
	view.Set(1, 3, 0.0f);
	view.Set(2, 3, 0.0f);

	for (int i = 0; i < numParticles; i++) {
		// Skip dead particles
		if (particleTTL[i] <= 0.0f) continue;

		// Interpolate linearly between the two colors
		float interpolation = particleTTL[i] / totalTimeToLive;
		Graphics::setFloat4(tintLocation, colorStart * interpolation + colorEnd * (1.0f - interpolation));

		mat4 M = mat4::Translation(particlePos[i].x(), particlePos[i].y(), particlePos[i].z()) * mat4::Scale(0.2f, 0.2f, 0.2f);
		Graphics::setMatrix(mLocation, M * view);
		Graphics::setMatrix(nLocation, calculateN(M * view));

		Graphics::setTexture(tex, texture);
		Graphics::setVertexBuffer(*vb);
		Graphics::setIndexBuffer(*ib);
		Graphics::drawIndexedVertices();
	}
	Graphics::setRenderState(RenderState::DepthWrite, true);
}

void ParticleSystem::emitParticle(int index) {
	// Calculate a random position inside the box
	float x = getRandom(emitMin.x(), emitMax.x());
	float y = getRandom(emitMin.y(), emitMax.y());
	float z = getRandom(emitMin.z(), emitMax.z());

	particlePos[index].set(x, y, z);
	particleVel[index] = emitDir;
	particleTTL[index] = totalTimeToLive;
}

float ParticleSystem::getRandom(float minValue, float maxValue) {
	int randMax = 1000000;
	int randInt = Random::get(0, randMax);
	float r = (float)randInt / (float)randMax;
	return minValue + r * (maxValue - minValue);
}