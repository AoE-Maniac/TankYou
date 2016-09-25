#pragma once
#include "pch.h"

class Explosion : public ParticleSystem
{
public:
    Explosion(Kore::vec3 pos, float spawnArea, float grav, int maxParticles, Kore::VertexStructure** structures, Kore::Texture* image);
    void update(float deltaTime);
    void emitParticle(int index);
    void pulse();
private:
    bool exploded;
};
