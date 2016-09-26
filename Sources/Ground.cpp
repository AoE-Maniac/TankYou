#include "pch.h"

#include <algorithm>
#include <Kore/Math/Matrix.h>
#include <Kore/Log.h>

#include "Ground.h"
#include "astar.h"

using namespace Kore;

Ground::Ground(float* height, int xCount, int zCount, float xSize, float zSize) {
	this->height = height;
	this->xCount = xCount;
	this->zCount = zCount;
	this->xSize = xSize;
	this->zSize = zSize;
}

/* Triangle Layout:

	 __> x
	|	
	v	*--*
	z	| /|
		|/ |
		*--*
*/
float Ground::getHeight(float x, float z) {
	Ground::TriangleInfo info = getTriangle(x, z);
  if(info.indices.x() == -1)
    return -10.f;

  log(Info, "Triangle for %f, %f:", x, z);
  log(Info, "%d, %d, %d", info.indices[0], info.indices[1], info.indices[2]);
  log(Info, "Tri coords: %f, %f", info.triCoords[0], info.triCoords[1]);

  return interpolate(
    height[info.indices[0]],
    height[info.indices[1]],
    height[info.indices[2]],
    info.triCoords[0],
    info.triCoords[1],
    info.isTopTri);
}

Ground::TriangleInfo Ground::getTriangle(float x, float z) {
  TriangleInfo info;

  if(x < -xSize/2 || x > xSize/2 || z < -zSize/2 || z > zSize/2) {
    info.indices = vec3i(-1, -1, -1);
  } else {
    float triangleXSize = xSize / (xCount-1);
    float triangleZSize = zSize / (zCount-1);

    log(Info, "Tri size: %f, %f", triangleXSize, triangleZSize);

	  int xi = Kore::floor(x/xSize + 0.5f * xCount);
	  int zi = Kore::floor(z/zSize + 0.5f * zCount);

    log(Info, "xi, zi: %d, %d", xi, zi);

    info.triCoords[0] = (x - xi * triangleXSize + xSize/2) / triangleXSize;
    info.triCoords[1] = (z - zi * triangleZSize + zSize/2) / triangleZSize;

    info.isTopTri = info.triCoords.z() <= triangleZSize * (triangleXSize - info.triCoords.x()) / triangleXSize;

    info.indices = vec3i((zi + !info.isTopTri) * xCount + xi, zi * xCount + xi + 1, (zi + 1) * xCount + xi + !info.isTopTri);
  }

  return info;
}