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
    minDistToShoot = 50;
	callback = std::bind(&Tank::onCollision, this, std::placeholders::_1, std::placeholders::_2);
	hp = MAX_HP;
	kills = 0;
    enemyTanks = new std::vector<Tank*>;
    mFrac = frac;
	selected = false;
    Orientation = Random::get(Kore::pi);
    won = false;
	myProjectileID = -1;
	tts = 0;
    collisionData = this;
}

float Tank::getHPPerc() {
	return Kore::min(1.0f, Kore::max(0.0f, (1.0f * hp) / MAX_HP));
}

float Tank::getXPPerc() {
	return Kore::min(1.0f, Kore::max(0.0f, kills / 50.0f));
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
}

float Tank::RotateTurrentToTarget(vec3 targetPosition) {
    vec3 fromPos = getTurretLookAt();
    vec3 toPos = targetPosition - GetPosition();
    toPos = toPos/toPos.getLength();
    float angle = Kore::atan2(fromPos.z(),fromPos.x()) - Kore::atan2(toPos.z(),toPos.x());
    
    angle -= Orientation;

    turretAngle += angle;
    
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


float angle = 0;
void Tank::updateStateMachine(float deltaT) {
    tts -= deltaT;

    if (!won && getXPPerc() >= 1.0f) {
        currentState = Won;
    }
    
    switch (currentState) {
        case Wait: {
            //log(Info, "Wait");
            
            rotateTurret(deltaT * pi / 10);
            
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
            
            RotateTurrentToTarget(enemyTank->GetPosition());
            
            float distance = (GetPosition() - enemyTank->GetPosition()).getLength();
            if (distance < minDistToShoot) {
                StopTheTank();
                
                // Attack the enemy tank
                currentState = Attack;
            } else {
                // Track the enemy
                vec3 velocity = steer->PursueTarget(GetPosition(), enemyTank->GetPosition(), Velocity, enemyTank->Velocity, maxVelocity);
                MoveWithVelocity(velocity);
                //turretAngle = Kore::pi;
            }

            break;
        }
            
        case Attack: {
            //log(Info, "Shoot %i", mFrac);
            
            RotateTurrentToTarget(enemyTank->GetPosition());

            bool enemyTankLiving = false;
            for( int i = 0; i < enemyTanks->size() && ! enemyTankLiving; i++ )
            {
                if ((*enemyTanks)[i] == enemyTank) {
                    enemyTankLiving = true;
                }
            }
            if(!enemyTankLiving)
            {
                currentState = Wait;
                break;
            }
            
            // Shoot and Kill
            vec3 p = vec3(0,1,0) + GetPosition() + (enemyTank->GetPosition() - GetPosition()).normalize() * 7.f;
            if (myProjectileID < 0) {
				if (tts <= 0) {
					tts = 1;
					myProjectileID = mProj->fire(p, enemyTank, 1, 1, this);
				}
            }
            
            float distance = (GetPosition() - enemyTank->GetPosition()).getLength();
            if (distance > minDistToShoot) {
                currentState = Following;
            }
            
            break;
        }
            
        case Move: {
            //log(Info, "Move");
            
            //rotateTurret(deltaT * pi / 10);
            RotateTurrentToTarget(toPosition);
            
            if (steer->Arrive(getPosition(), toPosition)) {
                currentState = Wait;
            }
            
            vec3 velocity = steer->Seek(getPosition(), toPosition, maxVelocity);
            MoveWithVelocity(velocity);
            
            break;
        }
           
        case Won: {
            //log(Info, "Won %i", mFrac);
            
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
		float projDmg = (*((projectile_collision_data*) collisionData)).damage;
        if( (*((projectile_collision_data*) collisionData)).dest == (PhysicsObject*)this )
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

