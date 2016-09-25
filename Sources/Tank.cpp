#include "pch.h"
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
    currentState = Wandering;
    steer = new Steering;
    randomPosition = vec3(25, yPosition, 15);
}

void Tank::render(TextureUnit tex) {
	Top->render(tex);
	Bottom->render(tex);
}

void Tank::update(float deltaT) {
    updateStateMachine();
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
