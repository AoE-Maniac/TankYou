#pragma once

#include <Kore/Graphics/Graphics.h>
#include "ObjLoader.h"

Kore::mat4 calculateN(Kore::mat4 MV);
void setVertex(float* vertices, int index, float x, float y, float z, float u, float v);
void setVertexFromMesh(float* vertices, int index, Mesh* mesh);
void setVec4(float* data, int instanceIndex, int off, int size, Kore::vec4 v);
void setMatrix(float* data, int instanceIndex, int off, int size, Kore::mat4 m);
