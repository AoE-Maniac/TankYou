#pragma once

#include <vector>

#include <Kore/Math/Vector.h>

#include "Tank.h"

using namespace Kore;

class TankSystem {
public:
	TankSystem(int initialTankCount = 100);
	void update(float dt);


private:
	std::vector<Tank*> tanks;
};