#pragma once

#include <vector>

#include "Steering.h"

#include "Engine/Collision.h"
#include "Engine/PhysicsObject.h"

enum StateMachineState {
    Wandering,
    Following,
};

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
    
    void SetEnemy(std::vector<Tank*>& enemyTanks);
    std::vector<Tank*> GetEnemy() const;

	int hp;

private:
	float turretAngle;
    float Orientation;
    void SetOrientationFromVelocity();
    void SetTankOrientation(float deltaT);
    
    std::vector<Tank*> enemyTanks;
    Tank* enemyTank;
    
    Steering* steer;
    vec3 randomPosition;
    float maxVelocity;
    
    float yPosition;
    
    float minDistToFollow;
    float minDistToShoot;
    
    void updateStateMachine();
    StateMachineState currentState;

	void onCollision(COLLIDING_OBJECT other, void* collisionData);
    
};
