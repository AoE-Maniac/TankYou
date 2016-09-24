#include "Engine/pch.h"
#include "Projectile.h"

using namespace Kore;

Projectile::Projectile(Texture* particleTex, MeshObject* mesh, const VertexStructure& particleStructure, PhysicsWorld* physics) {
	physicsObject = new PhysicsObject(false, 0.001f);
	physicsObject->Collider.radius = 0.5f;
	physicsObject->Mesh = mesh;
	physics->AddDynamicObject(physicsObject);
	
	particles = new ParticleSystem(physicsObject->GetPosition(), vec3(0, 10, 0), 3.0f, vec4(2.5f, 0, 0, 1), vec4(0, 0, 0, 0), 0, 100, particleStructure, particleTex);

	active = false;
}

void Projectile::fire(vec3 pos, vec3 dir, float s) {
	physicsObject->SetPosition(pos);
	physicsObject->Velocity = dir.normalize() * s;
	active = true;
}

void Projectile::update(float deltaT) {
	physicsObject->UpdateMatrix();

	particles->setPosition(physicsObject->GetPosition());
	particles->setDirection(vec3(0, 1, 0));
	particles->update(deltaT);
}

void Projectile::render(ConstantLocation mLocation, ConstantLocation nLocation, ConstantLocation vLocation, ConstantLocation tintLocation, TextureUnit tex, mat4 view) {
	physicsObject->Mesh->render(mLocation, nLocation, tex);
	particles->render(tex, vLocation, mLocation, nLocation, tintLocation, view);
}