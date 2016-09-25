#include "pch.h"
#include "Landscape.h"
#include <Kore/Math/Random.h>

#include "Engine/Rendering.h"

using namespace Kore;

Kore::VertexBuffer** landscapeVertices;
Kore::IndexBuffer* landscapeIndices;
Kore::Texture* landscapeTexture;

void createLandscape(VertexStructure** structures) {
	Kore::Image* map = new Kore::Image("map.png", true);
	Kore::Image* normalmap = new Kore::Image("mapnormals.png", true);
	landscapeTexture = new Texture("sand.png", true);

	const float size = 50;
	const int w = 50;
	const int h = 50;
	
	landscapeVertices = new VertexBuffer*[2];
	landscapeVertices[0] = new VertexBuffer((w + 1) * (h + 1), *structures[0], 0);
	float* vertices = landscapeVertices[0]->lock();
	int i = 0;
	for (int y = 0; y <= h; ++y) {
		for (int x = 0; x <= w; ++x) {
			int color = 0xff00 & map->at(static_cast<int>(x / (float)(w + 1) * map->width), static_cast<int>(y / (float)(h + 1) * map->height));
			color >>= 8;
			int normal = normalmap->at(static_cast<int>(x / (float)(w + 1) * map->width), static_cast<int>(y / (float)(h + 1) * map->height));
			int nxi = (normal & 0xff000000) >> 24;
			int nyi = (normal & 0xff0000) >> 16;
			int nzi = (normal & 0xff00) >> 8;
			float nx = (nxi / 255.0f - 0.5f) * 2.0f;
			float ny = (nyi / 255.0f - 0.5f) * 2.0f;
			float nz = (nzi / 255.0f - 0.5f) * 2.0f;
			vertices[i++] = -size / 2 + size / w * x; vertices[i++] = color / 255.0f * 10.0f; vertices[i++] = -size / 2 + size / h * y;
			vertices[i++] = x % 2; vertices[i++] = y % 2;
			vertices[i++] = nx; vertices[i++] = ny; vertices[i++] = nz;
		}
	}
	landscapeVertices[0]->unlock();
	
	landscapeVertices[1] = new VertexBuffer(1, *structures[1], 1);
	
	landscapeIndices = new IndexBuffer(w * h * 6);
	int* indices = landscapeIndices->lock();
	i = 0;
	for (int y = 0; y < h; ++y) {
		for (int x = 0; x < w; ++x) {
			int baseindex = y * (w + 1) + x;
			indices[i++] = baseindex;
			indices[i++] = baseindex + 1;
			indices[i++] = baseindex + (w + 1);

			indices[i++] = baseindex + 1;
			indices[i++] = baseindex + (w + 1);
			indices[i++] = baseindex + (w + 1) + 1;
		}
	}
	landscapeIndices->unlock();
}

void renderLandscape(Kore::TextureUnit tex) {
	Graphics::setTexture(tex, landscapeTexture);

	float* data = landscapeVertices[1]->lock();
	setMatrix(data, 0, 0, mat4::Identity());
	setMatrix(data, 0, 1, mat4::Identity());
	landscapeVertices[1]->unlock();
	
	Graphics::setVertexBuffers(landscapeVertices, 2);
	Graphics::setIndexBuffer(*landscapeIndices);
	Graphics::drawIndexedVerticesInstanced(1);
}
