#include "Kore/pch.h"
#include "TankSystem.h"

TankSystem::TankSystem(InstancedMeshObject* meshB, InstancedMeshObject* meshT, InstancedMeshObject* meshF, vec3 spawn1a, vec3 spawn1b, vec3 spawn2a, vec3 spawn2b, float delay) :
		meshBottom(meshB),
		meshTop(meshT),
		meshFlag(meshF),
		spawnPos1a(spawn1a),
		spawnPos1b(spawn1b),
		spawnPos2a(spawn2a),
		spawnPos2b(spawn2b),
		spawnDelay(delay) {
	tanks.reserve(MAX_TANKS);
	spawnTimer = spawnDelay;
}

void spawnTank(std::vector<Tank*>* tanks, vec3 spawnPosa, vec3 spawnPosb) {
	float a = (Kore::Random::get(0, 1000) * 1.0f / 1000);
	Tank* t1 = new Tank();
	tanks->push_back(t1);
	t1->SetPosition(a * spawnPosa + (1 - a) * spawnPosb);
}

void TankSystem::update(float dt) {
	if (spawnTimer <= 0 && tanks.size() <= MAX_TANKS - 2) {
		spawnTank(&tanks, spawnPos1a, spawnPos1b);
		spawnTank(&tanks, spawnPos2a, spawnPos2b);
		
		spawnTimer = spawnDelay;
	}

    for (int i = 0; i < tanks.size(); i++) {
        Tank* tank = tanks[i];

        tank->Integrate(dt);
        tank->update(dt);
	}
	
	spawnTimer -= dt;
}

void TankSystem::render(TextureUnit tex, mat4 View) {
	float* dataB = meshBottom->vertexBuffers[1]->lock();
	float* dataT = meshTop->vertexBuffers[1]->lock();
	float* dataF = meshFlag->vertexBuffers[1]->lock();
    for (int i = 0; i < tanks.size(); i++) {
        Tank* tank = tanks[i];
		mat4 botM = tank->GetBottomM();
		setMatrix(dataB, i, 0, botM);
		setMatrix(dataB, i, 1, calculateN(botM * View));
		
		mat4 topM = tank->GetTopM(botM);
		setMatrix(dataT, i, 0, topM);
		setMatrix(dataT, i, 1, calculateN(topM * View));
		
		mat4 flagM = tank->GetFlagM(botM);
		setMatrix(dataF, i, 0, flagM);
		setMatrix(dataF, i, 1, calculateN(flagM * View));
	}
	meshBottom->vertexBuffers[1]->unlock();
	meshTop->vertexBuffers[1]->unlock();
	meshFlag->vertexBuffers[1]->unlock();
	
	meshBottom->render(tex, tanks.size());
	meshTop->render(tex, tanks.size());
	meshFlag->render(tex, tanks.size());
}