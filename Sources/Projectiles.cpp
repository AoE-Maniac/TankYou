#include "Engine/pch.h"
#include "Projectiles.h"

#include <cassert>

using namespace Kore;

Projectiles::Projectiles(int maxProjectiles, Texture* particleTex, MeshObject* mesh, const VertexStructure& particleStructure, PhysicsWorld* physics) : maxProj(maxProjectiles) {
	projectiles = new Projectile[maxProjectiles];
	for (int i = 0; i < maxProjectiles; i++) {
		projectiles[i].init(particleTex, mesh, particleStructure, physics);
	}
	currProj = 0;
}

void Projectiles::fire(vec3 pos, vec3 dir, float s) {
	assert(currProj + 1 < maxProj);
	projectiles[currProj].fire(pos, dir, s);
	currProj++;
}

void Projectiles::update(float deltaT) {
	for (int i = 0; i < currProj; i++) {
		if (projectiles[i].timeToLife > 0) {
			projectiles[i].update(deltaT);
		}
	}
}

void Projectiles::render(ConstantLocation mLocation, ConstantLocation nLocation, ConstantLocation vLocation, ConstantLocation tintLocation, TextureUnit tex, mat4 view) {
	for (int i = 0; i < currProj; i++) {
		if (projectiles[i].timeToLife > 0) {
			projectiles[i].render(mLocation, nLocation, vLocation, tintLocation, tex, view);
		}
	}
}