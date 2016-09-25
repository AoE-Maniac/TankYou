#pragma once

#include <vector>

#include <Kore/Math/Vector.h>

#include "Tank.h"
#include "Engine/InstancedMeshObject.h"
#include "Engine/Explosion.h"
#include "ParticleRenderer.h"

using namespace Kore;

#define MAX_TANKS 100

class TankSystem {
public:
	TankSystem(ParticleRenderer* particleRenderer, InstancedMeshObject* meshB, InstancedMeshObject* meshT, InstancedMeshObject* meshF, vec3 spawn1, vec3 spawn2, float delay);
	void update(float dt);
	void render(TextureUnit tex, mat4 View, ConstantLocation vLocation, ConstantLocation tintLocation);

private:
    ParticleRenderer* particleRenderer;
	float spawnDelay;
	float spawnTimer;
	vec3 spawnPos1;
	vec3 spawnPos2;
	InstancedMeshObject* meshBottom;
	InstancedMeshObject* meshTop;
	InstancedMeshObject* meshFlag;
    Texture* particleTexture;
	std::vector<Tank*> tanks;
    std::vector<Explosion*> explosions;
	void kill(int i);
};