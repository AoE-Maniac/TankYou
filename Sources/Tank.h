#pragma once
#include "Engine/Collision.h"
#include "Engine/PhysicsObject.h"

class Tank : public PhysicsObject {
public:
	Tank();
	void rotateTurret(float angle);
	void update(float deltaT);
	vec3 getTurretLookAt();
	vec3 getPosition();
    void Move(vec3 velocity);
    vec3 Velocity;
	
	mat4 Tank::GetBottomM();
	mat4 Tank::GetTopM(mat4 bottomM);
	mat4 Tank::GetFlagM(mat4 bottomM);
private:
	float turretAngle;
    float Orientation;
    void SetOrientationFromVelocity();
    void SetTankOrientation(float deltaT);
};
