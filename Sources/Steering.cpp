#include "Engine/pch.h"
#include "Steering.h"

Steering::Steering() {
    Random::init(1);
    ariveRadius = 5;
}

vec3 Steering::Seek(vec3 characterPos, vec3 targetPos, float maxVelocity) {
    vec3 distance = targetPos - characterPos;
    distance = distance / distance.getLength();
    distance.multiply(maxVelocity);
    
    if (distance.getLength() < ariveRadius) {
        // Slow down the character
        distance / ariveRadius;
    }
    
    return distance;
}

vec3 Steering::Flee(vec3 characterPos, vec3 targetPos, float maxVelocity) {
    return -Seek(characterPos, targetPos, maxVelocity);
}

vec3 Steering::Wander(vec3 characterPos, vec3 targetPos) {
    float randVelocity = Random::get(1, 2);

    return Seek(characterPos, targetPos, randVelocity);
}

vec3 Steering::PursueTarget(vec3 characterPos, vec3 targetPos, vec3 characterVel, vec3 targetVel, float maxVelocity) {
    vec3 distance = targetPos - characterPos;
    float time = distance.getLength() / maxVelocity;    // time needed to catch the target
    vec3 p = targetPos + targetVel * time;              // predict the target position
    vec3 velocity = p - characterPos;
    velocity = velocity / velocity.getLength();
    velocity.multiply(maxVelocity);
    return velocity;
}

vec3 Steering::EvadeTarget(vec3 characterPos, vec3 targetPos, vec3 characterVel, vec3 targetVel, float maxVelocity) {
    return - PursueTarget(characterPos, targetPos, characterVel, targetVel, maxVelocity);
}

bool Steering::Arrive(vec3 characterPos, vec3 targetPos) {
    vec3 distance = targetPos - characterPos;
    if (distance.getLength() < ariveRadius)
        return true;
    return false;
}
