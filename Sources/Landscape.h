#pragma once

#include <Kore/Graphics/Graphics.h>

extern Kore::VertexBuffer* landscapeVertices;
extern Kore::IndexBuffer* landscapeIndices;
extern Kore::Texture* landscapeTexture;

void createLandscape();
void renderLandscape(Kore::ConstantLocation mLocation, Kore::ConstantLocation nLocation, Kore::TextureUnit tex);
