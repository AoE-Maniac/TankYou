#pragma once

#include <list>
#include <vector>

#include "Kore/Math/Vector.h"

using namespace Kore;

typedef std::vector<vec3> NodeList;
typedef std::vector<std::vector<int>> NeighbourList;

std::list<vec3> astar(NodeList nodes, NeighbourList neighbours, vec3 start, vec3 goal);