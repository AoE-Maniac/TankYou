#pragma once

#include <Kore/Math/Core.h>
#include <Kore/System.h>
#include <Kore/Graphics/Image.h>
#include <Kore/Graphics/Graphics.h>

#include "PhysicsObject.h"

using namespace Kore;

class InstancedMeshObject {
public:
	InstancedMeshObject(const char* meshFile, const char* textureFile, VertexStructure** structures, int maxCount, float scale = 1.0f);
	
	VertexBuffer** vertexBuffers;
	void render(TextureUnit tex, int instances);

private:
	IndexBuffer* indexBuffer;

	Mesh* mesh;
	Texture* image;
};
