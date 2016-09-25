#pragma once

#include "pch.h"
#include "InstancedMeshObject.h"

#include <Kore/IO/FileReader.h>
#include <Kore/Math/Core.h>
#include <Kore/System.h>
#include <Kore/Graphics/Image.h>
#include <Kore/Graphics/Graphics.h>

#include <cassert>

#include "ObjLoader.h"
#include "PhysicsObject.h"
#include "Rendering.h"

using namespace Kore;

InstancedMeshObject::InstancedMeshObject(const char* meshFile, const char* textureFile, VertexStructure** structures, int maxCount, float scale) {
	mesh = loadObj(meshFile);
	image = new Texture(textureFile, true);
		
	vertexBuffers = new VertexBuffer*[2];
	vertexBuffers[0] = new VertexBuffer(mesh->numVertices, *structures[0], 0);
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
		
	vertexBuffers[1] = new VertexBuffer(maxCount, *structures[1], 1);

	indexBuffer = new IndexBuffer(mesh->numFaces * 3);
	int* indices = indexBuffer->lock();
	for (int i = 0; i < mesh->numFaces * 3; i++) {
		indices[i] = mesh->indices[i];
	}
	indexBuffer->unlock();
}

void InstancedMeshObject::render(TextureUnit tex, int instances) {
	Graphics::setTexture(tex, image);
	Graphics::setVertexBuffers(vertexBuffers, 2);
	Graphics::setIndexBuffer(*indexBuffer);
	Graphics::drawIndexedVerticesInstanced(instances);
}