#include "Tank.h"
#include "Kore/Math/Matrix.h"

void Tank::setTurretTransform() {
	Top->M = mat4::Translation(0,1,0) * Bottom->M * mat4::Rotation(turretAngle, 0, 0);
}

Tank::Tank(MeshObject* top, MeshObject* bottom) : PhysicsObject(10, true, true), Top(top), Bottom(bottom) {
	Mesh = bottom;
	bottom->M = mat4::Identity();
	turretAngle = 0;
	setTurretTransform();
}

void Tank::render(ConstantLocation mLocation, ConstantLocation nLocation, TextureUnit tex) {
	Top->render(mLocation, nLocation, tex);
	Bottom->render(mLocation, nLocation, tex);
}

void Tank::update(float deltaT) {
	rotateTurret(deltaT * pi / 10);
	UpdateMatrix();
	setTurretTransform();
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