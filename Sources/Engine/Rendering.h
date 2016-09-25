#pragma once

#include <Kore/Graphics/Graphics.h>

Kore::mat4 calculateN(Kore::mat4 M);
void setVertex(float* vertices, int index, float x, float y, float z, float u, float v);
void setMatrix(float* data, int instanceIndex, int matIndex, Kore::mat4 m);
