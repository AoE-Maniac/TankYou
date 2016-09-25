#include "Engine/pch.h"
#include "Projectile.h"

using namespace Kore;

Projectile::Projectile(Texture* particleTex, MeshObject* mesh, VertexStructure** particleStructures, PhysicsWorld* physics) {
	physicsObject = new PhysicsObject(0.001f, true, true);
	physicsObject->Collider.radius = 0.5f * PROJECTILE_SIZE;
	physicsObject->Mesh = mesh;
	physics->AddDynamicObject(physicsObject);
	
	particles = new ParticleSystem(physicsObject->GetPosition(), vec3(0, 10, 0), 10 * PROJECTILE_SIZE, 3.0f, vec4(0.5, 0.5, 0.5, 1), vec4(0.5, 0.5, 0.5, 0), 0, 100, particleStructures, particleTex);

	timeToLife = 0;
}

void Projectile::fire(vec3 pos, vec3 dir, float s) {
	vec3 direction = dir.normalize();
	physicsObject->SetPosition(pos);
	physicsObject->Velocity = direction * s;
	
	vec3 zneg = vec3(1, 0, 0);
	vec3 a = zneg.cross(direction).normalize();
	float ang = Kore::acos(zneg.dot(direction));
	vec3 b = a.cross(zneg);
	if (b.dot(direction) < 0) ang = -ang;
	vec3 q = Kore::sin(ang/2) * a;
	physicsObject->SetRotation(Quat(Kore::cos(ang/2), q.x(), q.y(), q.z()));

	timeToLife = PROJECTILE_LIFETIME;
}

void Projectile::update(float deltaT) {
	physicsObject->UpdateMatrix();

	particles->setPosition(physicsObject->GetPosition());
	particles->setDirection(vec3(0, 1, 0));
	particles->update(deltaT);

	timeToLife -= deltaT;
}
