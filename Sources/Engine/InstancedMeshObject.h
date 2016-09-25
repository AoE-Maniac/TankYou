#pragma once

#include <Kore/Math/Core.h>
#include <Kore/System.h>
#include <Kore/Graphics/Image.h>
#include <Kore/Graphics/Graphics.h>

#include "PhysicsObject.h"

using namespace Kore;

class InstancedMeshObject {
public:
	InstancedMeshObject(int maxCount, const char* meshFile, const char* textureFile, const VertexStructure& structure, float scale = 1.0f);

	void addInstance(PhysicsObject* instance);
	void render(ConstantLocation mLocation, ConstantLocation nLocation, TextureUnit tex);

private:
	VertexBuffer* vertexBuffer;
	IndexBuffer* indexBuffer;
	int currInstanceCount;
	int maxInstanceCount;
	PhysicsObject** instances;

	Mesh* mesh;
	Texture* image;
};
