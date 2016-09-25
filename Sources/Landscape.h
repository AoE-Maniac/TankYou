#pragma once

#include <Kore/Graphics/Graphics.h>

extern Kore::VertexBuffer** landscapeVertices;
extern Kore::IndexBuffer* landscapeIndices;
extern Kore::Texture* landscapeTexture;

void createLandscape(Kore::VertexStructure** structures);
void renderLandscape(Kore::TextureUnit tex);