#include "Kore/pch.h"
#include "TankSystem.h"

TankSystem::TankSystem(ParticleRenderer* particleRenderer,InstancedMeshObject* meshB, InstancedMeshObject* meshT, InstancedMeshObject* meshF, vec3 spawn1a, vec3 spawn1b, vec3 spawn2a, vec3 spawn2b, float delay, Projectiles* projectiles) :
		meshBottom(meshB),
		meshTop(meshT),
		meshFlag(meshF),
		spawnPos1a(spawn1a),
		spawnPos1b(spawn1b),
		spawnPos2a(spawn2a),
		spawnPos2b(spawn2b),
		spawnDelay(delay),
        mProjectiles(projectiles),
        particleRenderer(particleRenderer) {
	tanks.reserve(MAX_TANKS);
	spawnTimer = spawnDelay;
    particleTexture = new Texture("particle.png", true);
	selectedTank = __nullptr;
}

void spawnTank(std::vector<Tank*>& tanks, std::vector<Explosion*>* explosions, vec3 spawnPosa, vec3 spawnPosb, int frac, Projectiles* projectiles) {
	float a = (Kore::Random::get(0, 1000) * 1.0f / 1000);
	Tank* t1 = new Tank(frac);
	t1->SetEnemy(tanks);
    t1->setProjectile(projectiles);
	tanks.push_back(t1);
    explosions->push_back(nullptr);
	t1->SetPosition(a * spawnPosa + (1 - a) * spawnPosb);
}

void TankSystem::update(float dt) {
	if (spawnTimer <= 0 && tanks.size() <= MAX_TANKS - 2) {
		spawnTank(tanks, &explosions, spawnPos1a, spawnPos1b, 0, mProjectiles);
		spawnTank(tanks, &explosions, spawnPos2a, spawnPos2b, 1, mProjectiles);
		
		spawnTimer = spawnDelay;
	}

    std::vector<int> emptyIndices;
    for (int i = 0; i < tanks.size(); i++) {
        Tank* tank = tanks[i];
        float xpos = tank->getPosition()[0];
        float ypos = tank->getPosition()[2];
		if(tank->hp <= 0 || explosions[i] != nullptr)
        {
            if( kill(i) )
            {
                tanks.erase(tanks.begin()+i);
                explosions.erase(explosions.begin()+i);
                --i;
            }
        }
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

bool TankSystem::kill(int i) {
    if(explosions[i] == nullptr && tanks[i] != nullptr)
    {
        explosions[i] = new Explosion(tanks[i]->getPosition(), 3.f, 10.f, 200,
                                      particleRenderer->getStructures(), particleTexture );
        particleRenderer->addParticleSystem(explosions[i]);
        Sound *winSound = new Sound("shoot_sound.wav");
        Mixer::play(winSound);
    } else
    {
        if(explosions[i]->isReady() == true)
        {
            particleRenderer->removeParticleSystem(explosions[i]);
            delete explosions[i];
            explosions[i] = nullptr;
            delete tanks[i];
            tanks[i] = nullptr;
            return true;
        }
    }
    return false;
}

void TankSystem::render(TextureUnit tex, mat4 View, ConstantLocation vLocation) {
	float* dataB = meshBottom->vertexBuffers[1]->lock();
	float* dataT = meshTop->vertexBuffers[1]->lock();
	float* dataF = meshFlag->vertexBuffers[1]->lock();
    
    int j = 0;
    for (int i = 0; i < tanks.size(); i++) {
        if(explosions[i] == nullptr)
        {
			Tank* tank = tanks[i];
			mat4 botM = tank->GetBottomM();
			vec4 col = vec4(tank->mFrac * 0.75f, 0, (1 - tank->mFrac) * 0.75f, 1);
			if (tank->selected) col = vec4(Kore::max(1.0f * tank->mFrac, 0.75f), 0.25f, Kore::max(1.0f * (1 - tank->mFrac), 0.75f), 1);

			setMatrix(dataB, j, 0, 36, botM);
			setMatrix(dataB, j, 16, 36, calculateN(botM * View));
			setVec4(dataB, j, 32, 36, col);
		
			mat4 topM = tank->GetTopM(botM);
			setMatrix(dataT, j, 0, 36, topM);
			setMatrix(dataT, j, 16, 36, calculateN(topM * View));
			setVec4(dataT, j, 32, 36, col);
		
			mat4 flagM = tank->GetFlagM(botM);
			setMatrix(dataF, j, 0, 36, flagM);
			setMatrix(dataF, j, 16, 36, calculateN(flagM * View));
			setVec4(dataF, j, 32, 36, col);
            ++j;
        }
	}
    
	meshBottom->vertexBuffers[1]->unlock();
	meshTop->vertexBuffers[1]->unlock();
	meshFlag->vertexBuffers[1]->unlock();
	
	meshBottom->render(tex, j);
	meshTop->render(tex, j);
	meshFlag->render(tex, j);
}

void TankSystem::select(vec3 cameraPosition, vec3 pickDir) {
	if (selectedTank != __nullptr) {
		selectedTank->selected = false;
		selectedTank = __nullptr;
	}

	selectedTank = getHitTank(cameraPosition, pickDir);
	if (selectedTank != __nullptr) {
		selectedTank->selected = true;
	}
}

void TankSystem::issueCommand(vec3 cameraPosition, vec3 pickDir) {
	if (selectedTank != __nullptr) {
		Tank* hitTank = getHitTank(cameraPosition, pickDir);

		if (hitTank == __nullptr) {
			float x = (selectedTank->GetPosition().y() - cameraPosition.y()) / pickDir.y();
			vec3 pos = cameraPosition + x * pickDir;
			selectedTank->MoveToPosition(pos);
			log(Kore::LogLevel::Info, "Moving to %f, %f, %f", pos.x(), pos.y(), pos.z());
		}
		else if (hitTank->mFrac != selectedTank->mFrac) {
			selectedTank->FollowAndAttack(hitTank);
			log(Kore::LogLevel::Info, "Attack at %f, %f, %f", hitTank->GetPosition().x(), hitTank->GetPosition().y(), hitTank->GetPosition().z());
		}
	}
}

Tank* TankSystem::getHitTank(vec3 cameraPosition, vec3 pickDir) {
	for (unsigned i = 0; i < tanks.size(); ++i) {
		if (tanks[i]->Collider.IntersectsWith(cameraPosition, pickDir)) {
			return tanks[i];
		}
	}
	return __nullptr;
}
