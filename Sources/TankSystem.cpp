#include "Kore/pch.h"
#include "TankSystem.h"

TankSystem::TankSystem(InstancedMeshObject* meshB, InstancedMeshObject* meshT, InstancedMeshObject* meshF, vec3 spawn1a, vec3 spawn1b, vec3 spawn2a, vec3 spawn2b, float delay, Projectiles* projectiles) :
		meshBottom(meshB),
		meshTop(meshT),
		meshFlag(meshF),
		spawnPos1a(spawn1a),
		spawnPos1b(spawn1b),
		spawnPos2a(spawn2a),
		spawnPos2b(spawn2b),
		spawnDelay(delay),
        mProjectiles(projectiles) {
	tanks.reserve(MAX_TANKS);
	spawnTimer = spawnDelay;
}

void spawnTank(std::vector<Tank*>* tanks, vec3 spawnPosa, vec3 spawnPosb, int frac, Projectiles* projectiles) {
	float a = (Kore::Random::get(0, 1000) * 1.0f / 1000);
	Tank* t1 = new Tank(frac);
	t1->SetEnemy(*tanks);
    t1->setProjectile(projectiles);
	tanks->push_back(t1);
	t1->SetPosition(a * spawnPosa + (1 - a) * spawnPosb);
}

void TankSystem::update(float dt) {
	if (spawnTimer <= 0 && tanks.size() <= MAX_TANKS - 2) {
		spawnTank(&tanks, spawnPos1a, spawnPos1b, 0, mProjectiles);
		spawnTank(&tanks, spawnPos2a, spawnPos2b, 1, mProjectiles);
		
		spawnTimer = spawnDelay;
	}

    for (int i = 0; i < tanks.size(); i++) {
        Tank* tank = tanks[i];

		if(tank->hp <= 0)
			kill(i);

        tank->Integrate(dt);
        tank->update(dt);
	}
	
	spawnTimer -= dt;
}

void TankSystem::kill(int i) {
	tanks.erase(tanks.begin() + i);
}

void TankSystem::render(TextureUnit tex, mat4 View) {
	float* dataB = meshBottom->vertexBuffers[1]->lock();
	float* dataT = meshTop->vertexBuffers[1]->lock();
	float* dataF = meshFlag->vertexBuffers[1]->lock();
    for (int i = 0; i < tanks.size(); i++) {
        Tank* tank = tanks[i];
		mat4 botM = tank->GetBottomM();
		setMatrix(dataB, i, 0, 36, botM);
		setMatrix(dataB, i, 16, 36, calculateN(botM * View));
		setVec4(dataB, i, 32, 36, vec4(tank->mFrac * 0.75f, 0, (1 - tank->mFrac) * 0.75f, 1));
		
		mat4 topM = tank->GetTopM(botM);
		setMatrix(dataT, i, 0, 36, topM);
		setMatrix(dataT, i, 16, 36, calculateN(topM * View));
		setVec4(dataT, i, 32, 36, vec4(tank->mFrac * 0.75f, 0, (1 - tank->mFrac) * 0.75f, 1));
		
		mat4 flagM = tank->GetFlagM(botM);
		setMatrix(dataF, i, 0, 36, flagM);
		setMatrix(dataF, i, 16, 36, calculateN(flagM * View));
		setVec4(dataF, i, 32, 36, vec4(tank->mFrac * 0.75f, 0, (1 - tank->mFrac) * 0.75f, 1));
	}
	meshBottom->vertexBuffers[1]->unlock();
	meshTop->vertexBuffers[1]->unlock();
	meshFlag->vertexBuffers[1]->unlock();
	
	meshBottom->render(tex, tanks.size());
	meshTop->render(tex, tanks.size());
	meshFlag->render(tex, tanks.size());
}
