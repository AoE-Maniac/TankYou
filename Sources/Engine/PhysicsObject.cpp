#include "pch.h"
#include "PhysicsObject.h"
#include "Kore/Log.h"

using namespace Kore;

PhysicsObject::PhysicsObject(bool isStatic, float mass) : IsStatic(isStatic), Mass(mass) {
	Accumulator = vec3(0, 0, 0);
	Velocity = vec3(0, 0, 0);
	Collider.radius = 0.5f;
	Rotation = Quat();//Quaternion(vec3(1, 1, 1), 0);
	float I = 2.0f/5.0f * Mass * Collider.radius * Collider.radius;
	MomentOfInertia.Set(0, 0, I);
	MomentOfInertia.Set(1, 1, I);
	MomentOfInertia.Set(2, 2, I);

	InverseMomentOfInertia = MomentOfInertia.Invert();
}

void SetSkewSymmetric(mat3& matrix, vec3& v) {
	matrix.Set(0, 0, 0);
	matrix.Set(0, 1, -v.z());
	matrix.Set(0, 2, v.y());
	matrix.Set(1, 0, v.z());
	matrix.Set(1, 1, 0);
	matrix.Set(1, 2, -v.x());
	matrix.Set(2, 0, -v.y());
	matrix.Set(2, 1, v.x());
	matrix.Set(2, 2, 0);
}

void PhysicsObject::HandleCollision(TriangleMeshCollider& collider, float deltaT) {
	// Check if we are colliding with the plane
	if (Collider.IntersectsWith(collider)) {

		// Calculate the contact velocity
		///////////////

		// Get the matrix contact coordinate system to world coordinate system
		mat3 contactToWorld = Collider.GetCollisonBasis(Collider.GetCollisionNormal(collider));

		// Get the relative contact position
		vec3 collisionGlobalPosition = Collider.GetCollisionPoint(collider);
		vec3 collisionRelativePosition = collisionGlobalPosition - Collider.center;

		// Work out the velocity of the contact point.
		vec3 vel = AngularVelocity.cross(collisionRelativePosition);
		vel += Velocity;

		// Turn the velocity into contact-coordinates.
		mat3 worldToContact = contactToWorld.Invert();
		vec3 contactVelocity = worldToContact * vel;

		///////////////
		// Calculate the desired velocity change
		float velocityLimit = 0.25f;

		// Calculate the acceleration induced velocity accumulated this frame
		float velocityFromAcc = 0;

		vec3 contactNormal = Collider.GetCollisionNormal(collider);

		velocityFromAcc += Accumulator  * deltaT * contactNormal;

		float restitution = 0.8f;
		
		// Combine the bounce velocity with the removed
		// acceleration velocity.
		float desiredDeltaVelocity = -contactVelocity.x() - restitution * (contactVelocity.x() - velocityFromAcc);
		
		// Calculate the impulse contact
		vec3 impulseContact;

		float inverseMass = 1.0f / Mass;

		float friction = 0.2f;
		
		// The equivalent of a cross product in matrices is multiplication
		// by a skew symmetric matrix - we build the matrix for converting
		// between linear and angular quantities.
		mat3 impulseToTorque;
		SetSkewSymmetric(impulseToTorque, collisionRelativePosition);

		// Build the matrix to convert contact impulse to change in velocity
		// in world coordinates.
		mat3 deltaVelWorld = impulseToTorque;
		deltaVelWorld *= InverseMomentOfInertia;
		deltaVelWorld *= impulseToTorque;
		deltaVelWorld = deltaVelWorld * -1.0f;
		
		// Do a change of basis to convert into contact coordinates.
		mat3 deltaVelocity = contactToWorld;
		deltaVelocity.Transpose();
		deltaVelocity *= deltaVelWorld;
		deltaVelocity *= contactToWorld;

		// Add in the linear velocity change
		deltaVelocity.data[0] += inverseMass;
		//deltaVelocity.data[4] += inverseMass;
		deltaVelocity.data[3] += inverseMass;
		//deltaVelocity.data[8] += inverseMass;
		deltaVelocity.data[7] += inverseMass;

		// Invert to get the impulse needed per unit velocity
		mat3 impulseMatrix = deltaVelocity.Invert();

		// Find the target velocities to kill
		vec3 velKill(desiredDeltaVelocity,
			-contactVelocity.y(),
			-contactVelocity.z());
		
		// Find the impulse to kill target velocities
		impulseContact = impulseMatrix * velKill;

		// Check for exceeding friction
		float planarImpulse = Kore::sqrt(
			impulseContact.y() * impulseContact.y() +
			impulseContact.z() * impulseContact.z()
			);
		
		//if ((planarImpulse > impulseContact.x() * friction) & (planarImpulse > 0.1f))
		//{
		//	// We need to use dynamic friction
		//	impulseContact.set(impulseContact.x(), impulseContact.y() / planarImpulse, impulseContact.z() / planarImpulse);

		//	float x = deltaVelocity.data[0] +
		//		deltaVelocity.data[1] * friction*impulseContact.y() +
		//		deltaVelocity.data[2] * friction*impulseContact.z();

		//	x = desiredDeltaVelocity / x;

		//	impulseContact.set(x, impulseContact.y() * friction * x, impulseContact.z() * friction * x);
		//}

		// Use the impulse contact
		vec3 impulse = contactToWorld * impulseContact;

		vec3 impulsiveTorque = collisionRelativePosition.cross(impulse);

		vec3 rotationChange = InverseMomentOfInertia * impulsiveTorque;

		vec3 velocityChange = impulse / Mass;

		AngularVelocity += rotationChange;
		Velocity += velocityChange;

		// Move the object out of the collider
		float penetrationDepth = Collider.PenetrationDepth(collider);
		
		SetPosition(Position - contactNormal * penetrationDepth * 1.05f);
	}
}

void PhysicsObject::HandleCollision(PhysicsObject* other, float deltaT) {
	// Check if we are colliding with the plane
	if (Collider.IntersectsWith(other->Collider)) {

		float restitution = 0.8f;

		vec3 collisionNormal = Collider.GetCollisionNormal(other->Collider);
			
		float separatingVelocity = -(other->Velocity - Velocity) * collisionNormal;

		if (separatingVelocity < 0) return;

		float newSeparatingVelocity = -separatingVelocity * restitution;
		
		float deltaVelocity = newSeparatingVelocity - separatingVelocity;
			
		// Move the object out of the collider
		float penetrationDepth = -Collider.PenetrationDepth(other->Collider);
				
		SetPosition(Position - collisionNormal * penetrationDepth);
		other->SetPosition(other->Position + collisionNormal * penetrationDepth );

		vec3 impulse = collisionNormal * -deltaVelocity;

		ApplyImpulse(impulse);
		other->ApplyImpulse(-impulse);
	}
}

void PhysicsObject::ApplyImpulse(vec3 impulse) {
	Velocity += impulse;
}

void PhysicsObject::ApplyForceToCenter(vec3 force) {
	Accumulator += force;
}

void PhysicsObject::Integrate(float deltaT) {
	// Derive a new Velocity based on the accumulated forces
	Velocity += (Accumulator / Mass) * deltaT;

	// Multiply by a damping coefficient (e.g. 0.98)
	float damping = 0.98f;
	Velocity *= damping;

	AngularVelocity *= damping;
	Rotation.addScaledVector(AngularVelocity, deltaT);

	// Derive a new position based on the velocity (Note: Use SetPosition to also set the collider's values)
	SetPosition(Position + Velocity * deltaT);
	
	// Clear the accumulator
	Accumulator = vec3(0, 0, 0);
}

void PhysicsObject::UpdateMatrix() {
	// Update the Mesh matrix
	Rotation.normalise();
	Mesh->M = mat4::Translation(Position.x(), Position.y(), Position.z()) * mat4::Scale(0.5f, 0.5f, 0.5f) * Rotation.getMatrix();
}



