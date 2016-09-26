#include "Engine/pch.h"
#include "Projectiles.h"
#include "Tank.h"
#include <cassert>

using namespace Kore;

Projectiles::Projectiles(int maxProjectiles, float hitDistance, Texture* particleTex, MeshObject* mesh, VertexStructure** structures, PhysicsWorld* physics) : maxProj(maxProjectiles), sharedMesh(mesh) {
	timeToLife = new float[maxProjectiles];
	damage = new int[maxProjectiles];
	physicsObject = new PhysicsObject*[maxProjectiles];
	targets = new PhysicsObject*[maxProjectiles];
	particles = new ParticleSystem*[maxProjectiles];
	hitDist = hitDistance;
	
	for (int i = 0; i < maxProjectiles; i++) {
		timeToLife[i] = 0;

		physicsObject[i] = new PhysicsObject(PROJECTILE, 0.001f, true, true);
		physicsObject[i]->Collider.radius = 0.5f * PROJECTILE_SIZE;
		physicsObject[i]->Mesh = mesh;
		physicsObject[i]->callback = [=](COLLIDING_OBJECT other, void* collisionData) { kill(i, true); };
		physicsObject[i]->collisionData = &damage[i];
		physicsObject[i]->active = false;
		
		physics->AddDynamicObject(physicsObject[i]);
	
		particles[i] = new ParticleSystem(physicsObject[i]->GetPosition(), vec3(0, 10, 0), 10 * PROJECTILE_SIZE, 3.0f, vec4(0.5, 0.5, 0.5, 1), vec4(0.5, 0.5, 0.5, 0), 0, 100, structures, particleTex);
	}
	
	vertexBuffers = new VertexBuffer*[2];
	vertexBuffers[0] = mesh->vertexBuffers[0];
	vertexBuffers[1] = new VertexBuffer(maxProjectiles, *structures[1], 1);

	currProj = 0;
}

int Projectiles::fire(vec3 pos, PhysicsObject* target, float s, int dmg, Tank* shooter) {
	assert(currProj + 1 < maxProj);

	if (currProj + 1 < maxProj) {
		vec3 direction = (target->GetPosition() - pos).normalize();
		physicsObject[currProj]->SetPosition(pos);
		physicsObject[currProj]->Velocity = direction * s;
		physicsObject[currProj]->active = true;
	
		vec3 zneg = vec3(1, 0, 0);
		vec3 a = zneg.cross(direction).normalize();
		float ang = Kore::acos(zneg.dot(direction));
		vec3 b = a.cross(zneg);
		if (b.dot(direction) < 0) ang = -ang;
		vec3 q = Kore::sin(ang/2) * a;
		physicsObject[currProj]->SetRotation(Quat(Kore::cos(ang/2), q.x(), q.y(), q.z()));

		timeToLife[currProj] = PROJECTILE_LIFETIME;

		damage[currProj] = dmg;
		shooters[currProj] = shooter;
		targets[currProj] = target;

		currProj++;

		return currProj - 1;
	}
	return -1;
}

void Projectiles::update(float deltaT) {
	for (int i = 0; i < currProj; i++) {
		if (timeToLife[i] > 0 || physicsObject[i]->GetPosition().distance(targets[i]->GetPosition()) > hitDist) {
			particles[i]->setPosition(physicsObject[i]->GetPosition());
			particles[i]->setDirection(vec3(0, 1, 0));
			particles[i]->update(deltaT);

			// TODO: Scheint nicht zu funktionieren
			physicsObject[i]->Velocity = (targets[i]->GetPosition() - physicsObject[i]->GetPosition()).normalize() * physicsObject[i]->Velocity.getLength();

			timeToLife[i] -= deltaT;
		}
		else {
			if(physicsObject[i]->GetPosition().distance(targets[i]->GetPosition()) > hitDist)
				log(Info, "Hit, killing");
			else
				log(Info, "ttl over, killing.");

			kill(i, physicsObject[i]->GetPosition().distance(targets[i]->GetPosition()) > hitDist);
			i--;
		}
	}
}

void Projectiles::kill(int projectile, bool kill) {
	timeToLife[projectile] = timeToLife[currProj - 1];
	timeToLife[currProj - 1] = -1;

	damage[projectile] = damage[currProj - 1];
	damage[currProj - 1] = 1;
	auto ctemp = physicsObject[currProj - 1]->callback;
	PhysicsObject* physicsObjectTemp = physicsObject[projectile];
	physicsObject[projectile] = physicsObject[currProj - 1];
	physicsObject[projectile]->callback = physicsObjectTemp->callback;

	physicsObject[currProj - 1] = physicsObjectTemp;
	physicsObject[currProj - 1]->callback = ctemp;
	physicsObject[currProj - 1]->active = false;

	ParticleSystem* temp = particles[projectile];
	particles[projectile] = particles[currProj - 1];
	particles[currProj - 1] = temp;

	if (shooters[currProj] != nullptr) {
		shooters[currProj]->myProjectileID = -1;
		if (kill) {
			shooters[currProj]->score();
		}
	}

	--currProj;
}

void Projectiles::onShooterDeath(int projectileID) {
	shooters[projectileID] = nullptr;
}

void Projectiles::render(ConstantLocation vLocation, TextureUnit tex, mat4 view) {
	float* data = vertexBuffers[1]->lock();
	for (int i = 0; i < currProj; i++) {
		mat4 M = physicsObject[i]->GetMatrix();
		setMatrix(data, i, 0, 36, M);
		setMatrix(data, i, 16, 36, calculateN(M));
		setVec4(data, i, 32, 36, vec4(1, 1, 1, 1));
	}
	vertexBuffers[1]->unlock();
	
	Graphics::setTexture(tex, sharedMesh->image);
	Graphics::setVertexBuffers(vertexBuffers, 2);
	Graphics::setIndexBuffer(*sharedMesh->indexBuffer);
	Graphics::drawIndexedVerticesInstanced(currProj);

	for (int i = 0; i < currProj; i++) {
		particles[i]->render(tex, vLocation, view);
	}
}