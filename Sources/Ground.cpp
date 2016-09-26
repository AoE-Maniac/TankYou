#include "pch.h"

#include <algorithm>
#include <Kore/Math/Matrix.h>
#include <Kore/Log.h>

#include "Ground.h"
#include "astar.h"

using namespace Kore;

Ground::Ground(float* height, vec3* normals, int xCount, int zCount, float xSize, float zSize) {
	this->height = height;
	this->normals = normals;
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

vec3 Ground::getNormal(float x, float z) {
	Ground::TriangleInfo info = getTriangle(x, z);
  if(info.indices.x() == -1)
    return vec3(0, 1, 0);

  vec3 n = vec3(interpolate(
    normals[info.indices[0]].x(),
    normals[info.indices[1]].x(),
    normals[info.indices[2]].x(),
    info.triCoords[0],
    info.triCoords[1],
    info.isTopTri), interpolate(
    normals[info.indices[0]].y(),
    normals[info.indices[1]].y(),
    normals[info.indices[2]].y(),
    info.triCoords[0],
    info.triCoords[1],
    info.isTopTri), interpolate(
    normals[info.indices[0]].z(),
    normals[info.indices[1]].z(),
    normals[info.indices[2]].z(),
    info.triCoords[0],
    info.triCoords[1],
    info.isTopTri));
	return n.normalize();
}

Ground::TriangleInfo Ground::getTriangle(float x, float z) {
  TriangleInfo info;

  if(x < -xSize/2 || x > xSize/2 || z < -zSize/2 || z > zSize/2) {
    info.indices = vec3i(-1, -1, -1);
  } else {
    float triangleXSize = xSize / (xCount-1);
    float triangleZSize = zSize / (zCount-1);

    log(Info, "Tri size: %f, %f", triangleXSize, triangleZSize);

	  int xi = Kore::floor(x/xSize * xCount + 0.5f * xCount);
	  int zi = Kore::floor(z/zSize * zCount + 0.5f * zCount);

    log(Info, "xi, zi: %d, %d", xi, zi);

    info.triCoords[0] = (x - xi * triangleXSize + xSize/2) / triangleXSize;
    info.triCoords[1] = (z - zi * triangleZSize + zSize/2) / triangleZSize;

    info.isTopTri = info.triCoords.z() <= triangleZSize * (triangleXSize - info.triCoords.x()) / triangleXSize;

    info.indices = vec3i((zi + !info.isTopTri) * xCount + xi, zi * xCount + xi + 1, (zi + 1) * xCount + xi + !info.isTopTri);
  }

  return info;
}