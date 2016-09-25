#include "pch.h"
#include "Tank.h"
#include "Kore/Math/Matrix.h"

Tank::Tank() : PhysicsObject(10, true, true) {
	bottom->M = mat4::Identity();
	Collider.radius = 0.5f;
	turretAngle = 0;
    currentState = Wandering;
    steer = new Steering;
    randomPosition = vec3(25, yPosition, 15);
	maxVelocity = 50;
    yPosition = 8.0f;
    minDistToFollow = 50;
    minDistToShoot = 10;
}

void Tank::update(float deltaT) {
    updateStateMachine();
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


void Tank::SetEnemy(std::vector<Tank*>& tanks) {
    enemyTanks = tanks;
}

std::vector<Tank*> Tank::GetEnemy() const {
    return enemyTanks;
}

void Tank::updateStateMachine() {
    
    switch (currentState) {
        case Wandering:
            //log(Info, "Wandering");
            
            // Wander TODO
            randomPosition.y() = yPosition;
            Move(steer->Wander(getPosition(), randomPosition, maxVelocity));
            
            // Follow the target
            for (int i = 0; i < enemyTanks.size(); i++) {
                Tank* tank = enemyTanks[i];
                //log(Info, "%i: %f %f", i, tank->GetPosition().y(), tank->GetPosition().y());
                float distance = (GetPosition() - tank->GetPosition()).getLength();
                
                if (distance < minDistToFollow) {
                    enemyTank = tank;
                    //currentState = Following;
                }
            }
            
            break;
            
        case Following:
            //log(Info, "Following");
            
            float distance = (GetPosition() - enemyTank->GetPosition()).getLength();
            //if (distance > 10) {
                // Track the enemy
                //vec3 velocity = steer->PursueTarget(GetPosition(), enemyTank->GetPosition(), Velocity, enemyTank->Velocity, maxVelocity);
                //Move(velocity);
            //} else {
                // Shoot and Kill
                
            //}
            
            
            break;
    }
    
    
}
