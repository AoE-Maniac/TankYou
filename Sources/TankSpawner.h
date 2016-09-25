#include <Kore/Math/Vector.h>

using namespace Kore;

class TankSpawner {
public:
	TankSpawner(vec3 spawn1, vec3 spawn2, float spawnRate);
	void update(float dt);


private:
	vec3 spawn1;
	vec3 spawn2;
	float spawnRate;
};
