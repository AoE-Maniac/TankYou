#include "Kore/pch.h"
#include "TankSpawner.h"

TankSpawner::TankSpawner(vec3 spawn1, vec3 spawn2, float spawnRate) {
	this->spawn1 = spawn1;
	this->spawn2 = spawn2;
	this->spawnRate = spawnRate;
}
