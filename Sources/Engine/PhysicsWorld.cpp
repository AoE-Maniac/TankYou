#include "pch.h"
#include "PhysicsWorld.h"

#include <cassert>

using namespace Kore;

PhysicsWorld::PhysicsWorld() {
	currentStaticColliders = 0;
	staticColliders = new TriangleMeshCollider*[MAX_STATIC_MESHES];

	currentDynamicObjects = 0;
	dynamicObjects = new PhysicsObject*[MAX_DYNAMIC_MESHES];
}

void PhysicsWorld::Update(float deltaT) {
	for (int i = 0; i < currentDynamicObjects; i++) {
		PhysicsObject** currentP = &dynamicObjects[i];

		// Apply gravity (= constant accceleration, so we multiply with the mass and divide in the integration step.
		// The alternative would be to add gravity during the integration as a constant.
		if (!(*currentP)->IgnoreGravity) {
			(*currentP)->ApplyForceToCenter(vec3(0, (*currentP)->Mass * -9.81, 0));
		}

		// Check for collisions with the other objects
		for (int j = i + 1; j < currentDynamicObjects; j++) {
			PhysicsObject** currentCollision = &dynamicObjects[j];
			(*currentP)->HandleCollision(*currentCollision, deltaT);
		}

		// Check for collisions with the static geometry
		for (int j = 0; j < currentStaticColliders; j++) {
			TriangleMeshCollider** meshCollider = &staticColliders[j];
			(*currentP)->HandleCollision(**meshCollider, deltaT);
		}

		// Integrate the equations of motion
		(*currentP)->Integrate(deltaT);
	}
}

void PhysicsWorld::AddStaticCollider(TriangleMeshCollider* tmc) {
	assert(currentStaticColliders < MAX_STATIC_MESHES);

	staticColliders[currentStaticColliders] = tmc;
	currentStaticColliders++;
}

void PhysicsWorld::AddDynamicObject(PhysicsObject* po) {
	assert(currentDynamicObjects < MAX_DYNAMIC_MESHES);

	dynamicObjects[currentDynamicObjects] = po;
	currentDynamicObjects++;
}
