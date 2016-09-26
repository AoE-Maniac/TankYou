#pragma once

#include <vector>

#include <Kore/Math/Vector.h>
#include <Kore/Audio/Mixer.h>

#include "Tank.h"
#include "Engine/InstancedMeshObject.h"
#include "Engine/Explosion.h"
#include "ParticleRenderer.h"


using namespace Kore;

#define MAX_TANKS 100

class TankSystem {
public:
	TankSystem(ParticleRenderer* particleRenderer, InstancedMeshObject* meshB, InstancedMeshObject* meshT, InstancedMeshObject* meshF, vec3 spawn1a, vec3 spawn1b, vec3 spawn2a, vec3 spawn2b, float delay, Projectiles* projectiles);
	void update(float dt);
	void render(TextureUnit tex, mat4 View, ConstantLocation vLocation);
	void select(vec3 cameraPosition, vec3 pickDir);
	void issueCommand(vec3 cameraPosition, vec3 pickDir);

private:
	Tank* selectedTank;
    ParticleRenderer* particleRenderer;
	float spawnDelay;
	float spawnTimer;
	vec3 spawnPos1a;
	vec3 spawnPos1b;
	vec3 spawnPos2a;
	vec3 spawnPos2b;
	InstancedMeshObject* meshBottom;
	InstancedMeshObject* meshTop;
	InstancedMeshObject* meshFlag;
    Texture* particleTexture;
	std::vector<Tank*> tanks;
    std::vector<Explosion*> explosions;
    std::vector<int> emptyIndices;
    Projectiles* mProjectiles;
	bool kill(int i);
	Tank* getHitTank(vec3 cameraPosition, vec3 pickDir);
};
