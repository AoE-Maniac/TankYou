#pragma once

#include "Engine/Particles.h"
#include "Engine/PhysicsObject.h"
#include "Engine/PhysicsWorld.h"

using namespace Kore;

class Projectile {
public:
	Projectile(Texture* particleTex, MeshObject* mesh, const VertexStructure& particleStructure, PhysicsWorld* physics);
	void fire(vec3 pos, vec3 dir, float s);
	void update(float deltaT);
	void render(ConstantLocation mLocation, ConstantLocation nLocation, ConstantLocation vLocation, ConstantLocation tintLocation, TextureUnit tex, mat4 view);
private:
	bool active;
	PhysicsObject* physicsObject;
	ParticleSystem* particles;
};