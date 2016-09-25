#pragma once

#include <vector>

#include <Kore/Math/Vector.h>

#include "Tank.h"
#include "Engine/InstancedMeshObject.h"

using namespace Kore;

#define MAX_TANKS 100

class TankSystem {
public:
	TankSystem(InstancedMeshObject* meshB, InstancedMeshObject* meshT, InstancedMeshObject* meshF, vec3 spawn1a, vec3 spawn1b, vec3 spawn2a, vec3 spawn2b, float delay);
	void update(float dt);
	void render(TextureUnit tex, mat4 View);

private:
	float spawnDelay;
	float spawnTimer;
	vec3 spawnPos1a;
	vec3 spawnPos1b;
	vec3 spawnPos2a;
	vec3 spawnPos2b;
	InstancedMeshObject* meshBottom;
	InstancedMeshObject* meshTop;
	InstancedMeshObject* meshFlag;
	std::vector<Tank*> tanks;
};