#include "Kore/pch.h"
#include "TankSystem.h"

TankSystem::TankSystem(InstancedMeshObject* meshB, InstancedMeshObject* meshT, InstancedMeshObject* meshF, vec3 spawn1, vec3 spawn2, float delay) :
		meshBottom(meshB),
		meshTop(meshT),
		meshFlag(meshF),
		spawnPos1(spawn1),
		spawnPos2(spawn2),
		spawnDelay(delay) {
	tanks.reserve(MAX_TANKS);
	spawnTimer = spawnDelay;
}

void TankSystem::update(float dt) {
	if (spawnTimer <= 0 && tanks.size() <= MAX_TANKS - 2) {
		Tank* t1 = new Tank();
		tanks.push_back(t1);
		t1->SetPosition(spawnPos1);
		
		Tank* t2 = new Tank();
		tanks.push_back(t2);
		t2->SetPosition(spawnPos2);
		
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