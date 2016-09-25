#include "Kore/pch.h"
#include "TankSystem.h"

TankSystem::TankSystem(ParticleRenderer* particleRenderer, InstancedMeshObject* meshB, InstancedMeshObject* meshT, InstancedMeshObject* meshF, vec3 spawn1, vec3 spawn2, float delay) :
		meshBottom(meshB),
		meshTop(meshT),
		meshFlag(meshF),
		spawnPos1(spawn1),
		spawnPos2(spawn2),
		spawnDelay(delay),
        particleRenderer(particleRenderer){
	tanks.reserve(MAX_TANKS);
	spawnTimer = spawnDelay;
    particleTexture = new Texture("particle.png", true);
}

void TankSystem::update(float dt) {
	if (spawnTimer <= 0 && tanks.size() <= MAX_TANKS - 2) {
		Tank* t1 = new Tank();
		tanks.push_back(t1);
        explosions.push_back(nullptr);
		t1->SetPosition(spawnPos1);
		
		Tank* t2 = new Tank();
		tanks.push_back(t2);
        explosions.push_back(nullptr);
		t2->SetPosition(spawnPos2);
		
		spawnTimer = spawnDelay;
	}

    for (int i = 0; i < tanks.size(); i++) {
        Tank* tank = tanks[i];

		if(tank->hp <= 0 || explosions[i] != nullptr)
			kill(i);
        else
        {
            tank->Integrate(dt);
            tank->update(dt);
        }
	}
    
    for( int i = 0; i < explosions.size(); i++ )
    {
        if(explosions[i] != nullptr)
        {
            explosions[i]->update(dt);
        }
    }
	
	spawnTimer -= dt;
}

void TankSystem::kill(int i) {
    if(explosions[i] == nullptr)
    {
        explosions[i] = new Explosion(tanks[i]->getPosition(), 3.f, 10.f, 200,
                                      particleRenderer->getStructures(), particleTexture );
        particleRenderer->addParticleSystem(explosions[i]);
    } else
    {
        if(explosions[i]->isReady() == true)
        {
            delete explosions[i];
            particleRenderer->removeParticleSystem(explosions[i]);
            explosions[i] = nullptr;
            delete tanks[i];
            tanks.erase(tanks.begin() + i);
        }
    }
}

void TankSystem::render(TextureUnit tex, mat4 View, ConstantLocation vLocation, ConstantLocation tintLocation) {
	float* dataB = meshBottom->vertexBuffers[1]->lock();
	float* dataT = meshTop->vertexBuffers[1]->lock();
	float* dataF = meshFlag->vertexBuffers[1]->lock();
    
    int unrenderedTanks = 0;
    int j = 0;
    for (int i = 0; i < tanks.size(); i++) {
        if(explosions[i] == nullptr)
        {
            Tank* tank = tanks[i];
            mat4 botM = tank->GetBottomM();
            setMatrix(dataB, j, 0, botM);
            setMatrix(dataB, j, 1, calculateN(botM * View));
		
            mat4 topM = tank->GetTopM(botM);
            setMatrix(dataT, j, 0, topM);
            setMatrix(dataT, j, 1, calculateN(topM * View));
		
            mat4 flagM = tank->GetFlagM(botM);
            setMatrix(dataF, j, 0, flagM);
            setMatrix(dataF, j, 1, calculateN(flagM * View));
            ++j;
        } else
        {
            ++unrenderedTanks;
        }
	}
    
	meshBottom->vertexBuffers[1]->unlock();
	meshTop->vertexBuffers[1]->unlock();
	meshFlag->vertexBuffers[1]->unlock();
	
	meshBottom->render(tex, tanks.size()-unrenderedTanks);
	meshTop->render(tex, tanks.size()-unrenderedTanks);
	meshFlag->render(tex, tanks.size()-unrenderedTanks);
}