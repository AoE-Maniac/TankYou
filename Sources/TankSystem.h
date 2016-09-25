#pragma once

#include <vector>

#include <Kore/Math/Vector.h>

#include "Tank.h"
#include "Engine/InstancedMeshObject.h"

using namespace Kore;

#define MAX_TANKS 100

class TankSystem {
public:
	TankSystem(InstancedMeshObject* meshB, InstancedMeshObject* meshT, InstancedMeshObject* meshF, vec3 spawn1, vec3 spawn2, float delay);
	void update(float dt);
	void render(TextureUnit tex, mat4 View);

private:
	float spawnDelay;
	float spawnTimer;
	vec3 spawnPos1;
	vec3 spawnPos2;
	InstancedMeshObject* meshBottom;
	InstancedMeshObject* meshTop;
	InstancedMeshObject* meshFlag;
	std::vector<Tank*> tanks;
};