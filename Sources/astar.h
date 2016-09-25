#pragma once

#include <list>

#include "Kore/Math/Vector.h"

using namespace Kore;

std::list<vec3> astar(std::vector<vec3> nodes, std::vector<std::vector<int>> neighbours, vec3 start, vec3 goal);