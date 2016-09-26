#include "Engine/pch.h"
#include "Projectiles.h"
#include "Tank.h"
#include <cassert>

using namespace Kore;

namespace {
	int maxProjectiles = 0;
}

Projectiles::Projectiles(int maxProjectiles, float hitDistance, Texture* particleTex, MeshObject* mesh, VertexStructure** structures, PhysicsWorld* physics) : maxProj(maxProjectiles), sharedMesh(mesh) {
	::maxProjectiles = maxProjectiles;
	timeToLife = new float[maxProjectiles];
	collision_data = new projectile_collision_data[maxProjectiles];
	physicsObject = new PhysicsObject*[maxProjectiles];
	targets = new PhysicsObject*[maxProjectiles];
	particles = new ParticleSystem*[maxProjectiles];
	shooters = new Tank*[maxProjectiles];
	hitDist = hitDistance;
    shooters = new Tank*[maxProjectiles];
	
	for (int i = 0; i < maxProjectiles; i++) {
        inactiveProjectiles.insert(i);
		timeToLife[i] = 0;

		physicsObject[i] = new PhysicsObject(PROJECTILE, 0.01f, true, true, false);
		physicsObject[i]->Collider.radius = 0.05f * PROJECTILE_SIZE;
		physicsObject[i]->Mesh = mesh;
        physicsObject[i]->callback = [=](COLLIDING_OBJECT other, void* collisionData) {
            if( (Tank*)collisionData == targets[i]  )
            {
                kill(i, true);
            }
        };
		physicsObject[i]->collisionData = &collision_data[i];
		physicsObject[i]->active = false;
		
		physics->AddDynamicObject(physicsObject[i]);
	
		particles[i] = new ParticleSystem(physicsObject[i]->GetPosition(), vec3(0, 10, 0), 10.f, 0.3f, vec4(0.5, 0.5, 0.5, 1), vec4(0.5, 0.5, 0.5, 0), 0, 100, structures, particleTex);
	}
	
	vertexBuffers = new VertexBuffer*[2];
	vertexBuffers[0] = mesh->vertexBuffers[0];
	vertexBuffers[1] = new VertexBuffer(maxProjectiles, *structures[1], 1);
}

int Projectiles::fire(vec3 pos, PhysicsObject* target, float s, int dmg, Tank* shooter) {
    assert(inactiveProjectiles.size() > 0);

	if (inactiveProjectiles.size() <= 0) {
		return -1;
	}
    
    int projectile = *(inactiveProjectiles.begin());
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
    collision_data[projectile].damage = dmg;
    collision_data[projectile].source = shooter;
    collision_data[projectile].dest = target;

    targets[projectile] = target;
    inactiveProjectiles.erase(projectile);
    shooters[projectile] = shooter;
	return projectile;
}

void Projectiles::update(float deltaT) {
	for (int i = 0; i < maxProj; i++) {
        if (physicsObject[i]->active) {
            if (timeToLife[i] > 0) {
                particles[i]->setPosition(physicsObject[i]->GetPosition());
                particles[i]->setDirection(vec3(0, 1, 0));
                particles[i]->update(deltaT);

			// TODO: Scheint nicht zu funktionieren
               // physicsObject[i]->Velocity = (targets[i]->GetPosition() - physicsObject[i]->GetPosition()).normalize() * physicsObject[i]->Velocity.getLength();
                
                vec3 position = physicsObject[i]->GetPosition();
				vec3 target;
				if (targets[i] != nullptr) {
					target = targets[i]->GetPosition();
				}
                vec3 direction = (target - physicsObject[i]->GetPosition()).normalize()*20.f;
                
                physicsObject[i]->Velocity = direction;
                physicsObject[i]->Integrate(deltaT);
                timeToLife[i] -= deltaT;
            }
            else if (targets[i] != nullptr) {
                if(physicsObject[i]->GetPosition().distance(targets[i]->GetPosition()) > hitDist)
                    log(Info, "Hit, killing");
                else
                    log(Info, "ttl over, killing.");

                kill(i, physicsObject[i]->GetPosition().distance(targets[i]->GetPosition()) > hitDist);
            }
		}
	}
}

void Projectiles::remove(Tank* tank) {
	for (int i = 0; i < maxProjectiles; ++i) {
		if (shooters[i] == tank) {
			shooters[i] = nullptr;
		}
		if (physicsObject[i] == tank) {
			physicsObject[i] = nullptr;
		}
		if (targets[i] == tank) {
			targets[i] = nullptr;
		}
	}
}

void Projectiles::kill(int projectile, bool kill) {
    physicsObject[projectile]->active = false;
    inactiveProjectiles.insert(projectile);

	if (shooters[projectile] != nullptr) {
		shooters[projectile]->myProjectileID = -1;
		if (kill) {
			shooters[projectile]->score();
		}
	}
}

void Projectiles::onShooterDeath(int projectileID) {
	shooters[projectileID] = nullptr;
}

void Projectiles::render(ConstantLocation vLocation, TextureUnit tex, mat4 view) {
	float* data = vertexBuffers[1]->lock();
	int c = 0;
	for (int i = 0; i < maxProj; i++) {
		if (physicsObject[i]->active) {
			mat4 M = physicsObject[i]->GetMatrix();
			setMatrix(data, i, 0, 36, M);
			setMatrix(data, i, 16, 36, calculateN(M));
			setVec4(data, i, 32, 36, vec4(1, 1, 1, 1));
			c++;
		}
	}
	vertexBuffers[1]->unlock();
	
	Graphics::setTexture(tex, sharedMesh->image);
	Graphics::setVertexBuffers(vertexBuffers, 2);
	Graphics::setIndexBuffer(*sharedMesh->indexBuffer);
	Graphics::drawIndexedVerticesInstanced(c);

	for (int i = 0; i < maxProj; i++) {
		if (physicsObject[i]->active) {
			particles[i]->render(tex, vLocation, view);
		}
	}
}