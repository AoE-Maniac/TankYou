#pragma once

#include <Kore/Graphics/Graphics.h>
#include "Ground.h"

#include "Engine/InstancedMeshObject.h"

extern Kore::VertexBuffer** landscapeVertices;
extern Kore::IndexBuffer* landscapeIndices;
extern Kore::Texture* landscapeTexture;

void createLandscape(Kore::VertexStructure** structures, float size, InstancedMeshObject* sMesh, int sCount, Ground** ground);
void renderLandscape(Kore::TextureUnit tex);