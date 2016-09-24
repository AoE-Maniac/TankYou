#include "pch.h"
#include "Landscape.h"

using namespace Kore;

Kore::VertexBuffer* landscapeVertices;
Kore::IndexBuffer* landscapeIndices;

void createLandscape() {
	VertexStructure structure;
	structure.add("pos", Float3VertexData);
	structure.add("tex", Float2VertexData);
	structure.add("nor", Float3VertexData);
	landscapeVertices = new Kore::VertexBuffer(4, structure);

	float* vertices = landscapeVertices->lock();
	const float size = 50;
	int i = 0;
	vertices[i++] = -size / 2; vertices[i++] = 0; vertices[i++] = size / 2; vertices[i++] = 0; vertices[i++] = 0; vertices[i++] = 0; vertices[i++] = 1; vertices[i++] = 0;
	vertices[i++] = size / 2; vertices[i++] = 0; vertices[i++] = size / 2; vertices[i++] = 0; vertices[i++] = 0; vertices[i++] = 0; vertices[i++] = 1; vertices[i++] = 0;
	vertices[i++] = size / 2; vertices[i++] = 0; vertices[i++] = -size / 2; vertices[i++] = 0; vertices[i++] = 0; vertices[i++] = 0; vertices[i++] = 1; vertices[i++] = 0;
	vertices[i++] = -size / 2; vertices[i++] = 0; vertices[i++] = -size / 2; vertices[i++] = 0; vertices[i++] = 0; vertices[i++] = 0; vertices[i++] = 1; vertices[i++] = 0;
	landscapeVertices->unlock();

	landscapeIndices = new IndexBuffer(6);
	int* indices = landscapeIndices->lock();
	i = 0;
	indices[i++] = 0; indices[i++] = 1; indices[i++] = 2;
	indices[i++] = 0; indices[i++] = 2; indices[i++] = 3;
	landscapeIndices->unlock();
}

void renderLandscape(Kore::ConstantLocation mLocation, Kore::ConstantLocation nLocation) {
	Graphics::setMatrix(mLocation, mat4::Identity());
	Graphics::setMatrix(nLocation, mat4::Identity());

	Graphics::setVertexBuffer(*landscapeVertices);
	Graphics::setIndexBuffer(*landscapeIndices);
	Graphics::drawIndexedVertices();
}
