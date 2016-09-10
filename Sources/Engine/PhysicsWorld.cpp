#include "pch.h"
#include "PhysicsWorld.h"

using namespace Kore;

PhysicsWorld::PhysicsWorld() {
	physicsObjects = new PhysicsObject*[100];
		for (int i = 0; i < 100; i++) {
			physicsObjects[i] = nullptr;
		}

		plane.normal = vec3(0, 1, 0);
		plane.d = -1;

		float y = 1.0f;

		vec3 p1(0, 0, 0);
		vec3 p2(0, 0, 1);
		vec3 p3(1, 0, 1);
		vec3 p4(1, 0, 0);

		p1 *= 30.0f;
		p2 *= 30.0f;
		p3 *= 30.0f;
		p4 *= 30.0f;
		
		p1 += vec3(0, 1, 0);
		p2 += vec3(0, 1, 0);
		p3 += vec3(0, 1, 0);
		p4 += vec3(0, 1, 0);

		triangle1.A = p1;
		triangle1.B = p2;
		triangle1.C = p3;
		
		triangle2.A = p1;
		triangle2.B = p3;
		triangle2.C = p4;
}

void PhysicsWorld::Update(float deltaT) {
	PhysicsObject** currentP = &physicsObjects[0];
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

		// Check for collisions with the plane
		// (*currentP)->HandleCollision(plane, deltaT);
		// (*currentP)->HandleCollision(triangle1, deltaT);
		// (*currentP)->HandleCollision(triangle2, deltaT);
		(*currentP)->HandleCollision(meshCollider, deltaT);

		// Integrate the equations of motion
		(*currentP)->Integrate(deltaT);

		++currentP;
	}
}

void PhysicsWorld::AddObject(PhysicsObject* po) {
	PhysicsObject** current = &physicsObjects[0];
	while (*current != nullptr) {
		++current;
	} 
	*current = po;
}
