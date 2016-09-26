#pragma once

#include <vector>

#include <Kore/Math/Vector.h>
#include <Kore/Audio/Mixer.h>

#include "Tank.h"
#include "Engine/InstancedMeshObject.h"
#include "Engine/Explosion.h"
#include "ParticleRenderer.h"


using namespace Kore;

#define MAX_TANKS 10//100

class TankSystem {
public:
	TankSystem(PhysicsWorld* world, ParticleRenderer* particleRenderer, InstancedMeshObject* meshB, InstancedMeshObject* meshT, InstancedMeshObject* meshF, vec3 spawn1a, vec3 spawn1b, vec3 spawn2a, vec3 spawn2b, float delay, Projectiles* projectiles, VertexStructure** structures);
	void initBars(vec2 halfSize, VertexStructure** structures);
	void update(float dt);
	void render(TextureUnit tex, mat4 View, ConstantLocation vLocation);
	void hover(vec3 cameraPosition, vec3 pickDir);
	void select(vec3 cameraPosition, vec3 pickDir);
	void issueCommand(vec3 cameraPosition, vec3 pickDir);

private:
	Tank* selectedTank;
	Tank* hoveredTank;
    ParticleRenderer* particleRenderer;
    PhysicsWorld* world;
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

	Kore::VertexBuffer** vbs;
	Kore::IndexBuffer* ib;
	Kore::Texture* texture;
};
