//
//  Explosion.cpp
//  Korerorinpa
//
//  Created by KOM TU Darmstadt on 25.09.16.
//  Copyright (c) 2016 KTX Software Development. All rights reserved.
//
#include "pch.h"
#include "Particles.h"
#include "Explosion.h"

Explosion::Explosion(vec3 pos, float spawnArea, float grav, int maxParticles, VertexStructure** structures, Texture* image) :
    ParticleSystem(pos, vec3(0,1,0), 7.f, 2.3f, vec4(1.f, 1.f, 0.f, 1), vec4(1.f, 0.f, 0.f, 1), grav, maxParticles, structures, image)
{
    this->spawnArea = spawnArea;
    setPosition(pos);
    for (int i = 0; i < numParticles; i++) {
        
    }
	exploded = false;
}

void Explosion::pulse()
{
    if(!exploded)
    {
        exploded = true;
        for (int i = 0; i < numParticles; i++) {
            emitParticle(i);
        }
    }
}

void Explosion::update(float deltaTime) {
    // Do we need to spawn a particle?
    
    pulse();
    for (int i = 0; i < numParticles; i++) {
        
        particleTTL[i] -= deltaTime;
        particleVel[i] += vec3(0, -gravity * deltaTime, 0);
        particlePos[i] += particleVel[i] * deltaTime;
    }
}

void Explosion::emitParticle(int index) {
    // Calculate a random position inside the box
    float x = getRandom(emitMin.x(), emitMax.x());
    float y = getRandom(emitMin.y(), emitMax.y());
    float z = getRandom(emitMin.z(), emitMax.z());
    
    float velX = getRandom(-7, 7);
    float velZ = getRandom(-7, 7);
    float velY = getRandom(10, 15);
    
    particlePos[index].set(x, y, z);
    particleVel[index].set(velX, velY, velZ);
    particleTTL[index] = totalTimeToLive;
}