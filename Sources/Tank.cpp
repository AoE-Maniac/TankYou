#include "pch.h"
#include "Tank.h"
#include "Kore/Math/Matrix.h"

const int MAX_HP = 10;

Tank::Tank(int frac) : PhysicsObject(COLLIDING_OBJECT::TANK, 10, true, true) {
	Collider.radius = 0.5f;
	turretAngle = 0;
    currentState = Wait;
    steer = new Steering;
    toPosition = vec3(25, yPosition, 15);
	maxVelocity = 0.5f;
    yPosition = 8.0f;
    Velocity = vec3(0,0,0);
    minDistToFollow = 100;
    minDistToShoot = 20;
	callback = std::bind(&Tank::onCollision, this, std::placeholders::_1, std::placeholders::_2);
	hp = MAX_HP;
	kills = 0;
    enemyTanks = new std::vector<Tank*>;
    mFrac = frac;
	selected = false;
	myProjectileID = -1;
}

float Tank::getHPPerc() {
	return (1.0f * hp) / MAX_HP;
}

float Tank::getXPPerc() {
	return kills / 3.0f;
}

void Tank::score() {
	kills++;
}

void Tank::onDeath() {
	mProj->onShooterDeath(myProjectileID);
}

void Tank::update(float deltaT) {
    updateStateMachine(deltaT);
    SetTankOrientation(deltaT);
    
    yPosition = 8.0f;
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

void Tank::SetOrientationFromVelocity(float deltaT) {
    if (Velocity.getLength() > 0) {
        float orient = Kore::atan2(Velocity.x(), Velocity.z());
        
        if (Kore::abs(orient-Orientation) > 0.3f) {
            float o = deltaT * pi;
            if (orient < Orientation)
                Orientation -= o;
            else
                Orientation += o;
        } else {
            Orientation = orient;
        }
    }
}

void Tank::MoveWithVelocity(vec3 velocity) {
    Velocity = velocity;
    //ApplyForceToCenter(Velocity);
    ApplyImpulse(Velocity);
}

void Tank::MoveToPosition(vec3 position) {
    toPosition = position;
    currentState = Move;
}

void Tank::SetTankOrientation(float deltaT) {
    SetOrientationFromVelocity(deltaT);
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
        case Wait: {
            //log(Info, "Wait");
            
            rotateTurret(deltaT * pi / 10);
            
            Velocity = vec3(0,0,0);
            PhysicsObject::Velocity = Velocity;
            
            break;
        }
            
            
        case Wandering: {
            //log(Info, "Wandering");
            
            rotateTurret(deltaT * pi / 10);
            
            // Wander
            toPosition.y() = yPosition;
            MoveWithVelocity(steer->Wander(getPosition(), toPosition, maxVelocity));
            
            // Follow the target
            for (int i = 0; i < enemyTanks->size(); i++) {
                Tank* tank = (*enemyTanks)[i];
                if(mFrac != tank->mFrac) {
                    float distance = (GetPosition() - tank->GetPosition()).getLength();
                    if (distance < minDistToFollow) {
                        enemyTank = tank;
                        currentState = Wait;
                    }
                }
            }
            
            break;
        }
            
        case Following: {
            log(Info, "Following: %i", mFrac);
            
            float distance = (GetPosition() - enemyTank->GetPosition()).getLength();
            if (distance < minDistToShoot) {
                Velocity = vec3(0,0,0);
                PhysicsObject::Velocity = Velocity;
                
                // Attack the enemy tank
                currentState = Attack;
            } else {
                // Track the enemy
                vec3 velocity = steer->PursueTarget(GetPosition(), enemyTank->GetPosition(), Velocity, enemyTank->Velocity, maxVelocity);
                MoveWithVelocity(velocity);
            }
            
            // Check if enemy is dead
            //if(enemyTank == null) {}
            
            
            break;
        }
            
        case Attack: {
            log(Info, "Shoot");
            
            MoveWithVelocity(vec3(0,0,0));
            
            // Turent should look at the enemy
            vec3 pos = getTurretLookAt();
            vec3 to = enemyTank->getPosition();
            float angle = Kore::atan2(pos.z(), pos.x()) - Kore::atan2(to.z(), to.x());
            rotateTurret(deltaT * angle);
            
            // Shoot and Kill
            vec3 p = GetPosition();
			if (myProjectileID < 0) {
				myProjectileID = mProj->fire(p, enemyTank, 1, 1, this);
			}
            
            break;
        }
            
        case Move: {
            log(Info, "Move");
            
            vec3 velocity = steer->Seek(GetPosition(), toPosition, maxVelocity);
            MoveWithVelocity(velocity);
            
            break;
        }
            
            
    }
}
    
void Tank::onCollision(COLLIDING_OBJECT other, void* collisionData) {
	log(Info, "Tank collided with %d", other);
	switch(other) {
	case COLLIDING_OBJECT::PROJECTILE:
		float projDmg = *((int*) collisionData);
		hp -= projDmg;
		break;
	}
}

void Tank::setProjectile(Projectiles* projectiles) {
    mProj = projectiles;
}

void Tank::FollowAndAttack(Tank *tank) {
    enemyTank = tank;
    currentState = Following;
}

