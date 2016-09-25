#include <set>
#include <algorithm>
#include <limits>
#include <vector>

#include <Kore/Log.h>

#include "astar.h"

std::list<vec3> astar(std::vector<vec3> nodes, std::vector<std::vector<int>> neighbours, vec3 start, vec3 goal) {
	std::set<int> visited;
	std::set<int> discovered;

	int startIndex, goalIndex;

	{
		float minStartDist = -1.f;
		float minGoalDist = -1.f;
		for(int i = 0; i < nodes.size(); i++) {
			float d = start.distance(nodes[i]);
			if(minStartDist < 0 || d < minStartDist) {
				minStartDist = d;
				startIndex = i;
			}

			d = goal.distance(nodes[i]);
			if(minGoalDist < 0 || d < minGoalDist) {
				minGoalDist = d;
				goalIndex = i;
			}
		}
	}

	log(Info, "Starting at %d, ending at %d", startIndex, goalIndex);

	discovered.insert(startIndex);

	int* cameFrom = new int[nodes.size()];
	std::fill_n(cameFrom, nodes.size(), -1);

	float* gScore = new float[nodes.size()];
	std::fill_n(gScore, nodes.size(), std::numeric_limits<float>::max());
	gScore[startIndex] = 0;

	float* fScore = new float[nodes.size()];
	std::fill_n(fScore, nodes.size(), std::numeric_limits<float>::max());
	fScore[goalIndex] = nodes[goalIndex].distance(nodes[startIndex]);

	bool found = false;

	int current;
	while(!discovered.empty()) {
		current = -1;
		float f = -1.f;
		std::for_each(discovered.begin(), discovered.end(), [&](int i) {
			if(f < 0 || fScore[i] < f) {
				current = i;
				f = fScore[i];
			}
		});

		log(Info, "\nCurrent: %d", current);

		if(current == goalIndex) {
			found = true;
			break;
		}

		discovered.erase(current);
		visited.insert(current);

		std::for_each(neighbours[current].begin(), neighbours[current].end(), [&](int n) {
			if(visited.find(n) == visited.end() )
			{
				log(Info, "Neighbour: %d", n);
				float tScore = gScore[current] + nodes[current].distance(nodes[n]);
				bool wasDiscovered = discovered.find(n) != discovered.end();
				log(Info, "was discovered: %d", wasDiscovered);
				discovered.insert(n);
				if(!wasDiscovered || tScore < gScore[n]) {
					cameFrom[n] = current;
					gScore[n] = tScore;
					fScore[n] = tScore + nodes[n].distance(nodes[goalIndex]);
					log(Info, "Found better score: g: %f, f: %f", gScore[n], fScore[n]);
				}
			}
		});
	}

	if(!found)
		return std::list<vec3>();
	
	std::list<vec3> path;
	path.push_front(goal);
	path.push_front(nodes[current]);

	log(Info, "path:\n goal\n%d", current);

	while(cameFrom[current] != -1) {
		current = cameFrom[current];
		path.push_front(nodes[current]);
		log(Info, "%d", current);
	}

	path.push_front(start);
	
	log(Info, "start");
	
	delete[] fScore;
	delete[] gScore;
	delete[] cameFrom;
	
	return path;
}