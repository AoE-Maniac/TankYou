#pragma once

#include <vector>

#include "Steering.h"

#include "Engine/Collision.h"
#include "Engine/PhysicsObject.h"
#include "Projectiles.h"

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
	
	mat4 GetBottomM();
	mat4 GetTopM(mat4 bottomM);
	mat4 GetFlagM(mat4 bottomM);
    
    void SetEnemy(std::vector<Tank*>& enemyTanks);
    std::vector<Tank*>* GetEnemy() const;
    
    void setProjectile(Projectiles& projectiles);

private:
	float turretAngle;
    float Orientation;
    void SetOrientationFromVelocity();
    void SetTankOrientation(float deltaT);
    
    std::vector<Tank*>* enemyTanks;
    Tank* enemyTank;
    
    Steering* steer;
    vec3 randomPosition;
    float maxVelocity;
    
    float yPosition;
    
    float minDistToFollow;
    float minDistToShoot;
    
    void updateStateMachine(float deltaT);
    StateMachineState currentState;
    
    Projectiles* mProjectiles;
};
