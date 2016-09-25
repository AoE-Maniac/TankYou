#include "pch.h"
#include "Rendering.h"

#include <Kore/Graphics/Graphics.h>

using namespace Kore;

mat4 calculateN(mat4 MV) {
	return MV.Invert().Transpose();
}

void setVertex(float* vertices, int index, float x, float y, float z, float u, float v) {
	vertices[index * 8 + 0] = x;
	vertices[index * 8 + 1] = y;
	vertices[index * 8 + 2] = z;
	vertices[index * 8 + 3] = u;
	vertices[index * 8 + 4] = v;
	vertices[index * 8 + 5] = 0.0f;
	vertices[index * 8 + 6] = 0.0f;
	vertices[index * 8 + 7] = -1.0f;
}

void setVertexFromMesh(float* vertices, int i, Mesh* mesh, float scale = 1.0) {
	vertices[i * 8 + 0] = mesh->vertices[i * 8 + 0] * scale;
	vertices[i * 8 + 1] = mesh->vertices[i * 8 + 1] * scale;
	vertices[i * 8 + 2] = mesh->vertices[i * 8 + 2] * scale;
	vertices[i * 8 + 3] = mesh->vertices[i * 8 + 3];
	vertices[i * 8 + 4] = 1.0f - mesh->vertices[i * 8 + 4];
	vertices[i * 8 + 5] = mesh->vertices[i * 8 + 5];
	vertices[i * 8 + 6] = mesh->vertices[i * 8 + 6];
	vertices[i * 8 + 7] = mesh->vertices[i * 8 + 7];
}

void setVec4(float* data, int instanceIndex, int off, int size, vec4 v) {
	int offset = off + instanceIndex * size;
	data[offset +  0] = v.x();
	data[offset +  1] = v.y();
	data[offset +  2] = v.z();
	data[offset +  3] = v.w();
}

void setMatrix(float* data, int instanceIndex, int off, int size, mat4 m) {
	int offset = off + instanceIndex * size;
	data[offset +  0] = m[0][0];
	data[offset +  1] = m[1][0];
	data[offset +  2] = m[2][0];
	data[offset +  3] = m[3][0];
	data[offset +  4] = m[0][1];
	data[offset +  5] = m[1][1];
	data[offset +  6] = m[2][1];
	data[offset +  7] = m[3][1];
	data[offset +  8] = m[0][2];
	data[offset +  9] = m[1][2];
	data[offset + 10] = m[2][2];
	data[offset + 11] = m[3][2];
	data[offset + 12] = m[0][3];
	data[offset + 13] = m[1][3];
	data[offset + 14] = m[2][3];
	data[offset + 15] = m[3][3];
}