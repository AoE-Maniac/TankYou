//
//  CameraSystemManager.cpp
//  Korerorinpa
//
//  Created by KOM TU Darmstadt on 25.09.16.
//  Copyright (c) 2016 KTX Software Development. All rights reserved.
//

#include "ParticleRenderer.h"

ParticleRenderer::ParticleRenderer(Kore::VertexStructure** structures) :
    structures(structures)
{
}

void ParticleRenderer::render(TextureUnit tex, mat4 View, ConstantLocation vLocation)
{
    std::set<ParticleSystem*>::iterator it;
    for( it = particlesystems.begin(); it != particlesystems.end(); ++it)
    {
        (*it)->render(tex, vLocation, View);
    }
}

void ParticleRenderer::addParticleSystem( ParticleSystem* system )
{
    particlesystems.insert(system);
}

void ParticleRenderer::removeParticleSystem( ParticleSystem* system )
{
    particlesystems.erase(system);
}

Kore::VertexStructure** ParticleRenderer::getStructures()
{
    return structures;
}