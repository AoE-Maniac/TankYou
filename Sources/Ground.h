#pragma once

#include <Kore/Math/Vector.h>
#include <Kore/Math/Matrix.h>

#include "astar.h"

class Ground {
public:
	Ground(float* height, vec3* normals, int xCount, int zCount, float xSize, float zSize);
	float getHeight(float x, float z);
	vec3 getNormal(float x, float z);

private:
	float* height;
	vec3* normals;
	int xCount;
	int zCount;
	float xSize;
	float zSize;

  struct TriangleInfo {
    vec3i indices;
    vec2 triCoords;
    bool isTopTri;
  };

  TriangleInfo getTriangle(float x, float z);

  template<typename T>
  T interpolate(T t1, T t2, T t3, float x, float z, bool topTri) {
    mat2 b;
    b[0] = vec2(-1, -1);
    float h;
	  if(topTri) {
      b[1] = vec2(1, 0);
    } else {
      b[1] = vec2(0, -1);
    }

    vec2 bary = b * vec2(x, z - 1);

    return bary[0] * t1 + bary[1] * t2 + (1 - bary[0] - bary[1]) * t3;
  }
};