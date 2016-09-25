#pragma once

#include <Kore/Graphics/Graphics.h>
#include "ObjLoader.h"

Kore::mat4 calculateN(Kore::mat4 MV);
void setVertex(float* vertices, int index, float x, float y, float z, float u, float v);
void setVertexFromMesh(float* vertices, int index, Mesh* mesh);
void setMatrix(float* data, int instanceIndex, int matIndex, Kore::mat4 m);
