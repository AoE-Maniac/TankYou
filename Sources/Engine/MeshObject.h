#pragma once

#include "pch.h"

#include <Kore/IO/FileReader.h>
#include <Kore/Math/Core.h>
#include <Kore/Math/Random.h>
#include <Kore/System.h>
#include <Kore/Input/Keyboard.h>
#include <Kore/Input/Mouse.h>
#include <Kore/Audio/Mixer.h>
#include <Kore/Graphics/Image.h>
#include <Kore/Graphics/Graphics.h>
#include <Kore/Log.h>

#include "ObjLoader.h"
#include "Rendering.h"

class MeshObject {
public:
	MeshObject(const char* meshFile, const char* textureFile, Kore::VertexStructure** structures, float scale = 1.0f) {
		mesh = loadObj(meshFile);
		image = new Kore::Texture(textureFile, true);
		
		vertexBuffers = new Kore::VertexBuffer*[2];
		vertexBuffers[0] = new Kore::VertexBuffer(mesh->numVertices, *structures[0], 0);
		float* vertices = vertexBuffers[0]->lock();
		for (int i = 0; i < mesh->numVertices; ++i) {
			vertices[i * 8 + 0] = mesh->vertices[i * 8 + 0] * scale;
			vertices[i * 8 + 1] = mesh->vertices[i * 8 + 1] * scale;
			vertices[i * 8 + 2] = mesh->vertices[i * 8 + 2] * scale;
			vertices[i * 8 + 3] = mesh->vertices[i * 8 + 3];
			vertices[i * 8 + 4] = 1.0f - mesh->vertices[i * 8 + 4];
			vertices[i * 8 + 5] = mesh->vertices[i * 8 + 5];
			vertices[i * 8 + 6] = mesh->vertices[i * 8 + 6];
			vertices[i * 8 + 7] = mesh->vertices[i * 8 + 7];
		}
		vertexBuffers[0]->unlock();
		
		vertexBuffers[1] = new Kore::VertexBuffer(1, *structures[1], 1);

		indexBuffer = new Kore::IndexBuffer(mesh->numFaces * 3);
		int* indices = indexBuffer->lock();
		for (int i = 0; i < mesh->numFaces * 3; i++) {
			indices[i] = mesh->indices[i];
		}
		indexBuffer->unlock();
	}

	void render(Kore::TextureUnit tex, int instances) {
		Kore::Graphics::setTexture(tex, image);
		Kore::Graphics::setVertexBuffers(vertexBuffers, 2);
		Kore::Graphics::setIndexBuffer(*indexBuffer);
		Kore::Graphics::drawIndexedVerticesInstanced(instances);
	}

	Kore::VertexBuffer** vertexBuffers;
	Kore::IndexBuffer* indexBuffer;

	Mesh* mesh;
	Kore::Texture* image;
};
