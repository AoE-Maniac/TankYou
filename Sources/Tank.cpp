#include "Tank.h"
#include "Kore/Math/Matrix.h"

Tank::Tank() : PhysicsObject(10, true, true) {
	Collider.radius = 0.5f;
	turretAngle = 0;
}

void Tank::update(float deltaT) {
	rotateTurret(deltaT * pi / 10);
    SetTankOrientation(deltaT);
}

void Tank::rotateTurret(float angle) {
	turretAngle += angle;
}

vec3 Tank::getTurretLookAt() {
	return mat4::Rotation(turretAngle, 0, 0) * vec4(0,0,-1,1);
}

vec3 Tank::getPosition() {
	return GetPosition() + vec3(0,1,0) + vec3((mat4::Rotation(turretAngle, 0, 0) * vec4(0,0,-3,1)));
}

void Tank::SetOrientationFromVelocity() {
    if (Velocity.getLength() > 0) {
        Orientation = Kore::atan2(Velocity.x(), Velocity.z());
    }
}

void Tank::Move(vec3 velocity) {
    Velocity = velocity;
    SetOrientationFromVelocity();
    ApplyForceToCenter(velocity);
}

void Tank::SetTankOrientation(float deltaT) {
}

mat4 Tank::GetBottomM() {
    vec3 pos = GetPosition();
	return mat4::Translation(pos.x(), pos.y(), pos.z()) * mat4::RotationY(Orientation);
}

mat4 Tank::GetTopM(mat4 bottomM) {
	return mat4::Translation(0,1,0) * bottomM * mat4::Rotation(turretAngle, 0, 0);
}

mat4 Tank::GetFlagM(mat4 bottomM) {
	return mat4::Translation(0,3,0) * bottomM * mat4::Rotation(pi/4, 0, 0);
}