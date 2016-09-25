#pragma once
#include "Engine/Collision.h"
#include "Engine/PhysicsObject.h"

class Tank : public PhysicsObject {
public:
	Tank(MeshObject* top, MeshObject* bottom);
	void render(TextureUnit tex);
	void rotateTurret(float angle);
	void update(float deltaT);
	vec3 getTurretLookAt();
	vec3 getPosition();
    void Move(vec3 velocity);
    vec3 Velocity;

private:
	MeshObject* Top;
	MeshObject* Bottom;
	float turretAngle;
	void setTurretTransform();
    float Orientation;
    void SetOrientationFromVelocity();
    void SetTankOrientation(float deltaT);
};
