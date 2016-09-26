#include "pch.h"
#include "Tank.h"
#include "Kore/Math/Matrix.h"

const int MAX_HP = 10;

Tank::Tank(int frac) : PhysicsObject(COLLIDING_OBJECT::TANK, 10, true, true, true) {
	Collider.radius = 6.f;
	turretAngle = 0.f;
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
    Orientation = 0;//pi/2.f;//0;
    won = false;
	myProjectileID = -1;
}

float Tank::getHPPerc() {
	return Kore::min(1.0f, Kore::max(0.0f, (1.0f * hp) / MAX_HP));
}

float Tank::getXPPerc() {
	return Kore::min(1.0f, Kore::max(0.0f, kills / 5.0f));
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
    
    //yPosition = 8.0f;
}

void Tank::rotateTurret(float angle) {
	turretAngle += angle;
    
    if(turretAngle > 2 * Kore::pi) {
        turretAngle = 0;
    }
}

float Tank::RotateTurrentToTarget() {
    vec3 fromPos = getTurretLookAt();
    //log(Info, "turret %f %f %i", turretLookAt.x(), turretLookAt.z(), mFrac);
    //vec3 endPos = toPosition - GetPosition();
    vec3 toPos = enemyTank->GetPosition() - GetPosition();
    toPos = toPos/toPos.getLength();
    //log(Info, "end %f %f %i", toPos.x(), toPos.z(), mFrac);
    float angle = Kore::atan2(fromPos.z(),fromPos.x()) - Kore::atan2(toPos.z(),toPos.x());
    
    angle += Orientation;
    log(Info, "%f %i", angle, mFrac);
    
    rotateTurret(angle);
    
    return angle;
}

vec3 Tank::getTurretLookAt() {
    return mat4::Rotation(turretAngle, 0, 0) * vec4(0,0,-1,1);
}

vec3 Tank::getTankLookAt() {
    return mat4::Rotation(Orientation, 0, 0) * vec4(0,0,-1,1);
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
    Velocity = velocity * 0.5f;
    //ApplyForceToCenter(Velocity);
    ApplyImpulse(Velocity);
}

void Tank::MoveToPosition(vec3 position) {
    toPosition = position;
//    currentState = Move;
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


bool dummy = false;
void Tank::updateStateMachine(float deltaT) {
    
    if (!won && getXPPerc() >= 1.0f) {
        currentState = Won;
    }
    
    switch (currentState) {
        case Wait: {
            //log(Info, "Wait");
            
            //rotateTurret(deltaT * pi / 10);
            
            if(selected) {
            
            vec3 getTankLook = getTankLookAt();
            log(Info, "tank %f %f %i", getTankLook.x(), getTankLook.z(), mFrac);
            
                vec3 turretLookAt = getTurretLookAt();
                log(Info, "turret %f %f %i", turretLookAt.x(), turretLookAt.z(), mFrac);
                vec3 endPos = toPosition - GetPosition();
                endPos = endPos/endPos.getLength();
                log(Info, "end %f %f %i", endPos.x(), endPos.z(), mFrac);
                //vec3 turretLookAt = vec3(-1,0,1);
                //vec3 endPos = vec3(1,0,1);
                float angle = Kore::atan2(turretLookAt.z(),turretLookAt.x()) - Kore::atan2(endPos.z(),endPos.x());
                log(Info, "%f %i", angle, mFrac);
            
            //angle-= Orientation;
 
                rotateTurret(angle / 10.f);
                
            }
            
            
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
                        currentState = Following;
                    }
                }
            }
            
            break;
        }
            
        case Following: {
            //log(Info, "Following: %i", mFrac);
            
            float distance = (GetPosition() - enemyTank->GetPosition()).getLength();
            if (distance < minDistToShoot) {
                StopTheTank();
                
                // Attack the enemy tank
                currentState = Attack;
            } else {
                // Track the enemy
                if(!dummy) {
                    dummy = true;
                vec3 velocity = steer->PursueTarget(GetPosition(), enemyTank->GetPosition(), Velocity, enemyTank->Velocity, maxVelocity);
                MoveWithVelocity(velocity);
                
                //RotateTurrentToTarget();
                }
            }

            break;
        }
            
        case Attack: {
            //log(Info, "Shoot %i", mFrac);
            
            //RotateTurrentToTarget();
            
            // Shoot and Kill
            vec3 p = GetPosition() + (enemyTank->GetPosition() - GetPosition()).normalize() * 10.f;
            if (myProjectileID < 0) {
                //myProjectileID = mProj->fire(p, enemyTank, 1, 1, this);
            }
            
            float distance = (GetPosition() - enemyTank->GetPosition()).getLength();
            if (distance > minDistToShoot) {
                currentState = Following;
            }
            
            break;
        }
            
        case Move: {
            //log(Info, "Move");
            
            rotateTurret(deltaT * pi / 10);
            
            if (steer->Arrive(getPosition(), toPosition)) {
                currentState = Wait;
            }
            
            vec3 velocity = steer->Seek(getPosition(), toPosition, maxVelocity);
            MoveWithVelocity(velocity);
            
            break;
        }
           
        case Won: {
            log(Info, "Won %i", mFrac);
            
            won = true;
            selected = false;
            
            int x = getPosition().x();
            int z = MAP_SIZE_INNER/2;
            toPosition = vec3(x, yPosition, z);
            currentState = Move;
        }
            
    }
}

void Tank::StopTheTank() {
    Velocity = vec3(0,0,0);
    PhysicsObject::Velocity = Velocity;
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

