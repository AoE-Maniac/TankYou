//
//  CameraSystemManager.h
//  Korerorinpa
//
//  Created by KOM TU Darmstadt on 25.09.16.
//  Copyright (c) 2016 KTX Software Development. All rights reserved.
//

#pragma once

#include "pch.h"
#include <Kore/math/matrix.h>
#include "Engine/Particles.h"
#include <set>

using namespace Kore;

#include <vector>

class ParticleRenderer
{
public:
    ParticleRenderer(Kore::VertexStructure** structures);
    void render(TextureUnit tex, mat4 View, ConstantLocation vLocation);
    
    void addParticleSystem(ParticleSystem* system);
    void removeParticleSystem(ParticleSystem* system);
    Kore::VertexStructure** getStructures();
private:
    std::set<ParticleSystem*> particlesystems;
    Kore::VertexStructure** structures;
};