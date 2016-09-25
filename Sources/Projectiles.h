#pragma once

#include "Engine/Particles.h"
#include "Engine/PhysicsObject.h"
#include "Engine/PhysicsWorld.h"

using namespace Kore;

#define PROJECTILE_SIZE 0.1f
#define PROJECTILE_LIFETIME 3;

class Projectiles {
public:
	Projectiles(int maxProjectiles, Texture* particleTex, MeshObject* mesh, VertexStructure** structures, PhysicsWorld* physics);
	void fire(vec3 pos, vec3 dir, float s);
	void update(float deltaT);
	void render(ConstantLocation vLocation, ConstantLocation tintLocation, TextureUnit tex, mat4 view);
private:
	int maxProj;
	int currProj;
	MeshObject* sharedMesh;
	VertexBuffer** vertexBuffers;

	// Projectiles
	float* timeToLife;
	PhysicsObject** physicsObject;
	ParticleSystem** particles;
	void kill(int projectile);
};