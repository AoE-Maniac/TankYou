#pragma once

#include <Kore/Graphics/Graphics.h>
#include "Ground.h"

#include "Engine/InstancedMeshObject.h"

const int MAP_SIZE_INNER = 200;
const int MAP_SIZE_OUTER = 300;
const int STONE_COUNT = 64;

extern Kore::VertexBuffer** landscapeVertices;
extern Kore::IndexBuffer* landscapeIndices;
extern Kore::Texture* landscapeTexture;

void createLandscape(Kore::VertexStructure** structures, float size, InstancedMeshObject* sMesh, int sCount, Ground*&);
void renderLandscape(Kore::TextureUnit tex);