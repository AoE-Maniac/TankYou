#include <Kore/Math/Vector.h>
#include <Kore/Math/Random.h>

//#include "Engine/MeshObject.h"

using namespace Kore;

class Steering {
    
private:
    
    //float maxVelocity = 2.0f;
    //float minVelocity = 1.0f;
    
    float ariveRadius;
    
public:
    
    Steering();
    
    vec3 Seek(vec3 characterPos, vec3 targetPos, vec3 maxVelocity);
    
    vec3 Flee(vec3 characterPos, vec3 targetPos, vec3 maxVelocity);
    
    vec3 Wander(vec3 characterPos, vec3& targetPos, vec3 maxVelocity);
    
    vec3 PursueTarget(vec3 characterPos, vec3 targetPos, vec3 characterVel, vec3 targetVel, float maxVelocity);
    
    vec3 EvadeTarget(vec3 characterPos, vec3 targetPos, vec3 characterVel, vec3 targetVel, float maxVelocity);
    
    bool Arrive(vec3 characterPos, vec3 targetPos);
    
};

/*
class AICharacter {
    
private:
    
    float Orientation;
    
    float Rotation;
    
    MeshObject* meshObject;
    
    
public:
    
    AICharacter();
    
    vec3 Position;
    vec3 Velocity;

    void SetOrientationFromVelocity();
    
    float GetOrientation();
    
};*/
