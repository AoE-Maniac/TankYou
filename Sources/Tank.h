#pragma once
#include "Engine/Collision.h"
#include "Engine/PhysicsObject.h"

class Tank : PhysicsObject {
public:
	Tank(MeshObject* top, MeshObject* bottom);
	void render(ConstantLocation mLocation, ConstantLocation nLocation, TextureUnit tex);
	void rotateTurret(float angle);
	void update();

private:
	MeshObject* Top;
	MeshObject* Bottom;
	float turretAngle;
};