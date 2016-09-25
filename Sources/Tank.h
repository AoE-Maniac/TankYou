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
	Tank(MeshObject* top, MeshObject* bottom, MeshObject* flag);
	void render(TextureUnit tex, mat4 V);
	void rotateTurret(float angle);
	void update(float deltaT);
	vec3 getTurretLookAt();
	vec3 getPosition();
    void Move(vec3 velocity);
    vec3 Velocity;
    
    void SetEnemy(std::vector<Tank*>& enemyTanks);
    std::vector<Tank*>* GetEnemy() const;
    
    void setProjectile(Projectiles& projectiles);

private:
	MeshObject* Top;
	MeshObject* Bottom;
    MeshObject* Flag;
	float turretAngle;
	void setTurretTransform();
    float Orientation;
    void SetOrientationFromVelocity();
    void SetTankOrientation(float deltaT);
    
    std::vector<Tank*>* enemyTanks;
    Tank* enemyTank;
    
    Steering* steer;
    vec3 randomPosition;
    float maxVelocity = 50;
    
    float yPosition = 8.0f;
    
    float minDistToFollow = 100;
    float minDistToShoot = 20;
    
    void updateStateMachine(float deltaT);
    StateMachineState currentState;
    
    Projectiles* mProjectiles;
};
