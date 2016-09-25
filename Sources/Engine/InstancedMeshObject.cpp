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

InstancedMeshObject::InstancedMeshObject(int maxCount, const char* meshFile, const char* textureFile, const VertexStructure& structure, float scale) : maxInstanceCount(maxCount) {
	mesh = loadObj(meshFile);
	image = new Texture(textureFile, true);

	vertexBuffer = new VertexBuffer(mesh->numVertices, structure, 0);
	float* vertices = vertexBuffer->lock();
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
	vertexBuffer->unlock();

	indexBuffer = new IndexBuffer(mesh->numFaces * 3);
	int* indices = indexBuffer->lock();
	for (int i = 0; i < mesh->numFaces * 3; i++) {
		indices[i] = mesh->indices[i];
	}
	indexBuffer->unlock();

	instances = new PhysicsObject*[maxInstanceCount];
	for (int i = 0; i < maxInstanceCount; i++) {
		instances[i] = nullptr;
	}

	currInstanceCount = 0;
}

void InstancedMeshObject::addInstance(PhysicsObject* instance) {
	assert(currInstanceCount + 1 < maxInstanceCount);
	instances[currInstanceCount] = instance;
	currInstanceCount++;
}

void InstancedMeshObject::render(ConstantLocation mLocation, ConstantLocation nLocation, TextureUnit tex) {
	for (int i = 0; i < currInstanceCount; ++i) {
		mat4 M = instances[i]->GetMatrix();

		Graphics::setMatrix(mLocation, M);
		Graphics::setMatrix(nLocation, calculateN(M));
			
		Graphics::setTexture(tex, image);
		Graphics::setVertexBuffer(*vertexBuffer);
		Graphics::setIndexBuffer(*indexBuffer);
		Graphics::drawIndexedVertices();
	}
}
