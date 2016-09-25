#pragma once
#include "pch.h"

class Explosion : public ParticleSystem
{
public:
    Explosion(vec3 pos, float spawnArea, float grav, int maxParticles, VertexStructure** structures, Texture* image);
    void update(float deltaTime);
    void emitParticle(int index);
    void pulse();
private:
    bool exploded;
};
