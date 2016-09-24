#pragma once

#include "Engine/Particles.h"
#include "Engine/PhysicsObject.h"
#include "Engine/PhysicsWorld.h"

using namespace Kore;

#define PROJECTILE_SIZE 0.1f
#define PROJECTILE_LIFETIME 3;

class Projectile {
public:
	float timeToLife;

	Projectile(Texture* particleTex, MeshObject* mesh, VertexStructure** particleStructures, PhysicsWorld* physics);
	void fire(vec3 pos, vec3 dir, float s);
	void update(float deltaT);
	void render(ConstantLocation mLocation, ConstantLocation nLocation, ConstantLocation vLocation, ConstantLocation tintLocation, TextureUnit tex, mat4 view);
private:
	PhysicsObject* physicsObject;
	ParticleSystem* particles;
};