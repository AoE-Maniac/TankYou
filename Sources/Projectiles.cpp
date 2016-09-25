#include "Engine/pch.h"
#include "Projectiles.h"
#include <cassert>

using namespace Kore;

Projectiles::Projectiles(int maxProjectiles, Texture* particleTex, MeshObject* mesh, VertexStructure** structures, PhysicsWorld* physics) : maxProj(maxProjectiles), sharedMesh(mesh) {
	timeToLife = new float[maxProjectiles];
	physicsObject = new PhysicsObject*[maxProjectiles];
	particles = new ParticleSystem*[maxProjectiles];
	
	for (int i = 0; i < maxProjectiles; i++) {
		timeToLife[i] = 0;

		physicsObject[i] = new PhysicsObject(PROJECTILE, 0.001f, true, true);
		physicsObject[i]->Collider.radius = 0.5f * PROJECTILE_SIZE;
		physicsObject[i]->Mesh = mesh;
		physicsObject[i]->callback = [=](COLLIDING_OBJECT other) { kill(i); };
		physics->AddDynamicObject(physicsObject[i]);
	
		particles[i] = new ParticleSystem(physicsObject[i]->GetPosition(), vec3(0, 10, 0), 10 * PROJECTILE_SIZE, 3.0f, vec4(0.5, 0.5, 0.5, 1), vec4(0.5, 0.5, 0.5, 0), 0, 100, structures, particleTex);
	}
	
	vertexBuffers = new VertexBuffer*[2];
	vertexBuffers[0] = mesh->vertexBuffers[0];
	vertexBuffers[1] = new VertexBuffer(maxProjectiles, *structures[1], 1);

	currProj = 0;
}

void Projectiles::fire(vec3 pos, vec3 dir, float s) {
	assert(currProj + 1 < maxProj);

	vec3 direction = dir.normalize();
	physicsObject[currProj]->SetPosition(pos);
	physicsObject[currProj]->Velocity = direction * s;
	
	vec3 zneg = vec3(1, 0, 0);
	vec3 a = zneg.cross(direction).normalize();
	float ang = Kore::acos(zneg.dot(direction));
	vec3 b = a.cross(zneg);
	if (b.dot(direction) < 0) ang = -ang;
	vec3 q = Kore::sin(ang/2) * a;
	physicsObject[currProj]->SetRotation(Quat(Kore::cos(ang/2), q.x(), q.y(), q.z()));

	timeToLife[currProj] = PROJECTILE_LIFETIME;

	currProj++;
}

void Projectiles::update(float deltaT) {
	for (int i = 0; i < currProj; i++) {
		if (timeToLife[i] > 0) {
			particles[i]->setPosition(physicsObject[i]->GetPosition());
			particles[i]->setDirection(vec3(0, 1, 0));
			particles[i]->update(deltaT);

			timeToLife[i] -= deltaT;
		}
		else {
			kill(i);
			i--;
		}
	}
}

void Projectiles::kill(int projectile) {
	timeToLife[projectile] = timeToLife[currProj - 1];
	timeToLife[currProj - 1] = -1;
			
	PhysicsObject* physicsObjectTemp = physicsObject[projectile];
	physicsObject[projectile] = physicsObject[currProj - 1];
	physicsObject[currProj - 1] = physicsObjectTemp;

	ParticleSystem* temp = particles[projectile];
	particles[projectile] = particles[currProj - 1];
	particles[currProj - 1] = temp;

	--currProj;
}

void Projectiles::render(ConstantLocation vLocation, ConstantLocation tintLocation, TextureUnit tex, mat4 view) {
	Graphics::setFloat4(tintLocation, vec4(1, 1, 1, 1));
	float* data = vertexBuffers[1]->lock();
	for (int i = 0; i < currProj; i++) {
		mat4 M = physicsObject[i]->GetMatrix();
		setMatrix(data, i, 0, M);
		setMatrix(data, i, 1, calculateN(M));
	}
	vertexBuffers[1]->unlock();
	
	Graphics::setTexture(tex, sharedMesh->image);
	Graphics::setVertexBuffers(vertexBuffers, 2);
	Graphics::setIndexBuffer(*sharedMesh->indexBuffer);
	Graphics::drawIndexedVerticesInstanced(currProj);

	for (int i = 0; i < currProj; i++) {
		particles[i]->render(tex, vLocation, tintLocation, view);
	}
}