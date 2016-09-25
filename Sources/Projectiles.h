#pragma once

#include "Engine/Particles.h"
#include "Engine/PhysicsObject.h"
#include "Engine/PhysicsWorld.h"

#define PROJECTILE_SIZE 0.1f
#define PROJECTILE_LIFETIME 3;

class Projectiles {
public:
	Projectiles(int maxProjectiles, Kore::Texture* particleTex, MeshObject* mesh, Kore::VertexStructure** structures, PhysicsWorld* physics);
	void fire(vec3 pos, vec3 dir, float s, int dmg);
	void update(float deltaT);
	void render(Kore::ConstantLocation vLocation, Kore::ConstantLocation tintLocation, Kore::TextureUnit tex, Kore::mat4 view);
private:
	int maxProj;
	int currProj;
	MeshObject* sharedMesh;
	Kore::VertexBuffer** vertexBuffers;

	// Projectiles
	float* timeToLife;
	int* damage;
	PhysicsObject** physicsObject;
	ParticleSystem** particles;
	void kill(int projectile);
};
