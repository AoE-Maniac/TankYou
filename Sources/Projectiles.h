#pragma once

#include "Engine/Particles.h"
#include "Engine/PhysicsObject.h"
#include "Engine/PhysicsWorld.h"

#include "Projectile.h"

using namespace Kore;

class Projectiles {
public:
	Projectiles(int maxProjectiles, Texture* particleTex, MeshObject* mesh, VertexStructure** particleStructures, PhysicsWorld* physics);
	void fire(vec3 pos, vec3 dir, float s);
	void update(float deltaT);
	void render(ConstantLocation vLocation, ConstantLocation tintLocation, TextureUnit tex, mat4 view);
private:
	int maxProj;
	int currProj;
	MeshObject* sharedMesh;
	Projectile** projectiles;
};