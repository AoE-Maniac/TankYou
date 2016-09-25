#include "pch.h"
#include "Tank.h"
#include "Kore/Math/Matrix.h"


Tank::Tank() : PhysicsObject(COLLIDING_OBJECT::TANK, 10, true, true) {
	Collider.radius = 0.5f;
	turretAngle = 0;
    currentState = Wandering;
    steer = new Steering;
    randomPosition = vec3(25, yPosition, 15);
	maxVelocity = 50;
    yPosition = 8.0f;
    minDistToFollow = 50;
    minDistToShoot = 10;
	//callback = std::bind(&Tank::onCollision, this, std::placeholders::_1, std::placeholders::_2);
	hp = 10;
    enemyTanks = new std::vector<Tank*>;
}

void Tank::update(float deltaT) {
    updateStateMachine(deltaT);
	//rotateTurret(deltaT * pi / 10);
    SetTankOrientation(deltaT);
	maxVelocity = 50;
    yPosition = 8.0f;
    minDistToFollow = 50;
    minDistToShoot = 10;
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
    enemyTanks = &tanks;
}

std::vector<Tank*>* Tank::GetEnemy() const {
    return enemyTanks;
}

void Tank::updateStateMachine(float deltaT) {
    
    switch (currentState) {
        case Wandering:
            //log(Info, "Wandering");
            
            // Wander
            randomPosition.y() = yPosition;
            Move(steer->Wander(getPosition(), randomPosition, maxVelocity));
            
            // Follow the target
            for (int i = 0; i < enemyTanks->size(); i++) {
                Tank* tank = (*enemyTanks)[i];
                if(tank != this) {
                    
                    //setTurrentTransform();
                    
                    
                    float distance = (GetPosition() - tank->GetPosition()).getLength();
                    if (distance < minDistToFollow) {
                        enemyTank = tank;
                        currentState = Following;
                    }
                }
            }
            
            break;
            
        case Following:
            //log(Info, "Following");
            
            float distance = (GetPosition() - enemyTank->GetPosition()).getLength();
            if (distance < minDistToShoot) {
                //log(Info, "Shoot");
                
                vec3 pos = getTurretLookAt();
                vec3 to = enemyTank->getPosition();
                float angle = Kore::atan2(pos.z(), pos.x()) - Kore::atan2(to.z(), to.x());
                rotateTurret(deltaT * angle);
                
                // Shoot and Kill
                vec3 p = GetPosition();
                vec3 l = getTurretLookAt();
                //mProjectiles->fire(p, l, 1);
                
                Move(vec3(0,0,0));
            } else {
                // Track the enemy
                vec3 velocity = steer->PursueTarget(GetPosition(), enemyTank->GetPosition(), Velocity, enemyTank->Velocity, maxVelocity);
                Move(velocity);
            }
            
            
            break;
    }
}
    
void Tank::onCollision(COLLIDING_OBJECT other, void* collisionData) {
	switch(other) {
	case COLLIDING_OBJECT::PROJECTILE:
		float projDmg = *((int*) collisionData);
		hp -= projDmg;
		break;
	}
}

void Tank::setProjectile(Projectiles& projectiles) {
    mProjectiles = &projectiles;
}
