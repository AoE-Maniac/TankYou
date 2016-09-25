#pragma once

#include <Kore/Graphics/Graphics.h>

extern Kore::VertexBuffer** landscapeVertices;
extern Kore::IndexBuffer* landscapeIndices;

void createLandscape(Kore::VertexStructure** structures);
void renderLandscape();
