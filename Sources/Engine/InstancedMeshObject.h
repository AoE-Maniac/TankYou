#pragma once

#include <Kore/Math/Core.h>
#include <Kore/System.h>
#include <Kore/Graphics/Image.h>
#include <Kore/Graphics/Graphics.h>

#include "PhysicsObject.h"

class InstancedMeshObject {
public:
	InstancedMeshObject(const char* meshFile, const char* textureFile, Kore::VertexStructure** structures, int maxCount, float scale = 1.0f);
	
	Kore::VertexBuffer** vertexBuffers;
	void render(Kore::TextureUnit tex, int instances);

private:
	Kore::IndexBuffer* indexBuffer;

	Mesh* mesh;
	Kore::Texture* image;
};
