#include "Engine/pch.h"
#include "Projectiles.h"

#include <cassert>

using namespace Kore;

Projectiles::Projectiles(int maxProjectiles, Texture* particleTex, MeshObject* mesh, VertexStructure** particleStructures, PhysicsWorld* physics) : maxProj(maxProjectiles), sharedMesh(mesh) {
	projectiles = new Projectile*[maxProjectiles];
	for (int i = 0; i < maxProjectiles; i++) {
		projectiles[i] = new Projectile(particleTex, mesh, particleStructures, physics);
	}
	currProj = 0;
}

void Projectiles::fire(vec3 pos, vec3 dir, float s) {
	assert(currProj + 1 < maxProj);
	projectiles[currProj]->fire(pos, dir, s);
	currProj++;
}

void Projectiles::update(float deltaT) {
	for (int i = 0; i < currProj; i++) {
		if (projectiles[i]->timeToLife > 0) {
			projectiles[i]->update(deltaT);
		}
		else {
			Projectile* temp = projectiles[i];
			projectiles[i] = projectiles[currProj - 1];
			projectiles[currProj - 1] = temp;

			--currProj;
			--i;
		}
	}
}

void Projectiles::render(ConstantLocation vLocation, ConstantLocation tintLocation, TextureUnit tex, mat4 view) {
	/*float* data = vbs[1]->lock();
	for (int i = 0; i < currProj; i++) {
		mat4 M = projectiles[i]->physicsObject->GetMatrix();
		setMatrix(data, i, 0, M);
		setMatrix(data, i, 1, calculateN(M));
	}
	vbs[1]->unlock();
	
	Graphics::setTexture(tex, sharedMesh->image);
	Graphics::setVertexBuffers(vbs, 2);
	Graphics::setIndexBuffer(*sharedMesh->indexBuffer);
	Graphics::drawIndexedVerticesInstanced(currProj);*/

	for (int i = 0; i < currProj; i++) {
		projectiles[i]->particles->render(tex, vLocation, tintLocation, view);
	}
}