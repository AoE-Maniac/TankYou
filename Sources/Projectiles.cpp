#include "Engine/pch.h"
#include "Projectiles.h"
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
        inactiveProjectiles.insert(i);
		timeToLife[i] = 0;

		physicsObject[i] = new PhysicsObject(PROJECTILE, 0.0f, true, true, false);
		physicsObject[i]->Collider.radius = 0.5f * PROJECTILE_SIZE;
		physicsObject[i]->Mesh = mesh;
		physicsObject[i]->callback = [=](COLLIDING_OBJECT other, void* collisionData) { kill(i); };
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

void Projectiles::fire(vec3 pos, PhysicsObject* target, float s, int dmg) {
    assert(inactiveProjectiles.size() > 0);
    
    int projectile = *(inactiveProjectiles.begin());
    log(Info, "projectile number: %d", projectile);
    vec3 direction = (target->GetPosition() - pos).normalize();
    physicsObject[projectile]->SetPosition(pos);
    physicsObject[projectile]->Velocity = direction * s;
    physicsObject[projectile]->active = true;
	
    vec3 zneg = vec3(1, 0, 0);
    vec3 a = zneg.cross(direction).normalize();
    float ang = Kore::acos(zneg.dot(direction));
    vec3 b = a.cross(zneg);
    if (b.dot(direction) < 0) ang = -ang;
    vec3 q = Kore::sin(ang/2) * a;
    physicsObject[projectile]->SetRotation(Quat(Kore::cos(ang/2), q.x(), q.y(), q.z()));

    timeToLife[projectile] = PROJECTILE_LIFETIME;
    damage[projectile] = dmg;

    targets[projectile] = target;
    inactiveProjectiles.erase(projectile);
}

void Projectiles::update(float deltaT) {
	for (int i = 0; i < currProj; i++) {
        if( inactiveProjectiles.find(i) != inactiveProjectiles.end() )
        {
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

                log(Info, "kill");
                kill(i);
                log(Info, "kill end");
                i--;
            }
		}
	}
}

void Projectiles::kill(int projectile) {
    physicsObject[projectile]->active = false;
    inactiveProjectiles.insert(projectile);
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