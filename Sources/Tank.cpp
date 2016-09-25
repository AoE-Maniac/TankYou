#include "Tank.h"
#include "Kore/Math/Matrix.h"

void Tank::setTurretTransform() {
	Top->M = mat4::Translation(0,1,0) * Bottom->M * mat4::Scale(1.7, 1.7, 1.7) *mat4::Rotation(turretAngle, 0, 0);
    Flag->M = mat4::Translation(0,3,0) * Bottom->M * mat4::Scale(0.5, 0.5, 0.5)* mat4::Rotation(pi/4, 0, 0);

}

Tank::Tank(MeshObject* top, MeshObject* bottom, MeshObject* flag) : PhysicsObject(TANK, 10, true, true), Top(top), Bottom(bottom) , Flag(flag){
	Mesh = bottom;
	bottom->M = mat4::Identity();
	turretAngle = 0;
	setTurretTransform();
}

void Tank::render(TextureUnit tex, mat4 V) {
	Top->render(tex, V);
	Bottom->render(tex, V);
    Flag->render(tex, V);
}

void Tank::update(float deltaT) {
	rotateTurret(deltaT * pi / 10);
	UpdateMatrix();
	setTurretTransform();
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
    vec3 pos = GetPosition();
    Bottom->M = mat4::Translation(pos.x(),pos.y(),pos.z()) * mat4::RotationY(Orientation);
}
