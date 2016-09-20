#include "pch.h"
#include "PhysicsWorld.h"

using namespace Kore;

PhysicsWorld::PhysicsWorld() {
	staticColliders = new TriangleMeshCollider*[100];
	for (int i = 0; i < 10; i++) {
		staticColliders[i] = nullptr;
	}

	dynamicObjects = new PhysicsObject*[100];
	for (int i = 0; i < 100; i++) {
		dynamicObjects[i] = nullptr;
	}
}

void PhysicsWorld::Update(float deltaT) {
	PhysicsObject** currentP = &dynamicObjects[0];
		while (*currentP != nullptr) {

		// Apply gravity (= constant accceleration, so we multiply with the mass and divide in the integration step.
		// The alternative would be to add gravity during the integration as a constant.

		(*currentP)->ApplyForceToCenter(vec3(0, (*currentP)->Mass * -9.81, 0));

		// Check for collisions with the other objects
		PhysicsObject** currentCollision = currentP + 1;
		while (*currentCollision != nullptr) {
			(*currentP)->HandleCollision(*currentCollision, deltaT);
			++currentCollision;
		}

		// Check for collisions with the static geometry
		TriangleMeshCollider** meshCollider = &staticColliders[0];
		while (*meshCollider != nullptr) {
			(*currentP)->HandleCollision(**meshCollider, deltaT);
			++meshCollider;
		}

		// Integrate the equations of motion
		(*currentP)->Integrate(deltaT);

		++currentP;
	}
}

void PhysicsWorld::AddStaticCollider(TriangleMeshCollider* tmc) {
	TriangleMeshCollider** current = &staticColliders[0];
	while (*current != nullptr) {
		++current;
	}
	*current = tmc;
}

void PhysicsWorld::AddDynamicObject(PhysicsObject* po) {
	PhysicsObject** current = &dynamicObjects[0];
	while (*current != nullptr) {
		++current;
	} 
	*current = po;
}
