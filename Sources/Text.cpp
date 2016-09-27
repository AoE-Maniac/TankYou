#include "pch.h"
#include "Text.h"
#include <Kore/IO/FileReader.h>
#include <string.h>

using namespace Kore;

namespace {
	const int bufferSize = 100;

	//TODO: Make this fast
	int findIndex(int charcode, int* fontGlyphs, int glyphCount) {
		for (int i = 0; i < glyphCount; ++i) {
			if (fontGlyphs[i] == charcode) return i;
		}
		return 0;
	}
}

Text::Text() : bilinear(false), lastTexture(nullptr) {
	bufferIndex = 0;
	initBuffers();
}

void Text::setProjection(int width, int height) {
	projectionMatrix = mat4::orthogonalProjection(0, width, height, 0, 0.1, 1000);
	this->projectionMatrix = projectionMatrix;
}

void Text::initBuffers() {
	VertexStructure structure;
	structure.add("vertexPosition", Float3VertexData);
	structure.add("texPosition", Float2VertexData);
	structure.add("vertexColor", Float4VertexData);

	FileReader vs("text.vert");
	FileReader fs("text.frag");
	Shader* vertexShader = new Shader(vs.readAll(), vs.size(), VertexShader);
	Shader* fragmentShader = new Shader(fs.readAll(), fs.size(), FragmentShader);
	program = new Program;
	program->setVertexShader(vertexShader);
	program->setFragmentShader(fragmentShader);
	program->link(structure);

	projectionLocation = program->getConstantLocation("projectionMatrix");
	textureLocation = program->getTextureUnit("tex");

	rectVertexBuffer = new VertexBuffer(bufferSize * 4, structure);
	rectVertices = rectVertexBuffer->lock();

	indexBuffer = new IndexBuffer(bufferSize * 3 * 2);
	int* indices = indexBuffer->lock();
	for (int i = 0; i < bufferSize; ++i) {
		indices[i * 3 * 2 + 0] = i * 4 + 0;
		indices[i * 3 * 2 + 1] = i * 4 + 1;
		indices[i * 3 * 2 + 2] = i * 4 + 2;
		indices[i * 3 * 2 + 3] = i * 4 + 0;
		indices[i * 3 * 2 + 4] = i * 4 + 2;
		indices[i * 3 * 2 + 5] = i * 4 + 3;
	}
	indexBuffer->unlock();
}

void Text::setRectVertices(float bottomleftx, float bottomlefty, float topleftx, float toplefty,
	float toprightx, float toprighty, float bottomrightx, float bottomrighty) {
	int baseIndex = bufferIndex * 9 * 4;
	rectVertices[baseIndex + 0] = bottomleftx;
	rectVertices[baseIndex + 1] = bottomlefty;
	rectVertices[baseIndex + 2] = -5.0;

	rectVertices[baseIndex + 9] = topleftx;
	rectVertices[baseIndex + 10] = toplefty;
	rectVertices[baseIndex + 11] = -5.0;

	rectVertices[baseIndex + 18] = toprightx;
	rectVertices[baseIndex + 19] = toprighty;
	rectVertices[baseIndex + 20] = -5.0;

	rectVertices[baseIndex + 27] = bottomrightx;
	rectVertices[baseIndex + 28] = bottomrighty;
	rectVertices[baseIndex + 29] = -5.0;
}

void Text::setRectTexCoords(float left, float top, float right, float bottom) {
	int baseIndex = bufferIndex * 9 * 4;
	rectVertices[baseIndex + 3] = left;
	rectVertices[baseIndex + 4] = bottom;

	rectVertices[baseIndex + 12] = left;
	rectVertices[baseIndex + 13] = top;

	rectVertices[baseIndex + 21] = right;
	rectVertices[baseIndex + 22] = top;

	rectVertices[baseIndex + 30] = right;
	rectVertices[baseIndex + 31] = bottom;
}

void Text::setRectColors(float opacity, int color) {
	int baseIndex = bufferIndex * 9 * 4;
	float R = ((color >> 24) & 0xff) / 255.0f;
	float G = ((color >> 16) & 0xff) / 255.0f;
	float B = ((color >> 8) & 0xff) / 255.0f;
	float A = ((color) & 0xff) / 255.0f;

	float a = opacity * A;

	rectVertices[baseIndex + 5] = R;
	rectVertices[baseIndex + 6] = G;
	rectVertices[baseIndex + 7] = B;
	rectVertices[baseIndex + 8] = a;

	rectVertices[baseIndex + 14] = R;
	rectVertices[baseIndex + 15] = G;
	rectVertices[baseIndex + 16] = B;
	rectVertices[baseIndex + 17] = a;

	rectVertices[baseIndex + 23] = R;
	rectVertices[baseIndex + 24] = G;
	rectVertices[baseIndex + 25] = B;
	rectVertices[baseIndex + 26] = a;

	rectVertices[baseIndex + 32] = R;
	rectVertices[baseIndex + 33] = G;
	rectVertices[baseIndex + 34] = B;
	rectVertices[baseIndex + 35] = a;
}

void Text::drawBuffer() {
	rectVertexBuffer->unlock();
	Graphics::setVertexBuffer(*rectVertexBuffer);
	Graphics::setIndexBuffer(*indexBuffer);
	//g.setPipeline(pipeline == null ? shaderPipeline : pipeline);
	Graphics::setTexture(textureLocation, lastTexture);
	Graphics::setMatrix(projectionLocation, projectionMatrix);
	//Graphics::setTextureParameters(textureLocation, TextureAddressing.Clamp, TextureAddressing.Clamp, bilinear ? TextureFilter.LinearFilter : TextureFilter.PointFilter, bilinear ? TextureFilter.LinearFilter : TextureFilter.PointFilter, MipMapFilter.NoMipFilter);
	Graphics::setTextureMipmapFilter(textureLocation, NoMipFilter);
	Graphics::setTextureAddressing(textureLocation, U, Clamp);
	Graphics::setTextureAddressing(textureLocation, V, Clamp);

	Graphics::drawIndexedVertices(0, bufferIndex * 2 * 3);

	//Graphics::setTexture(textureLocation, nullptr);
	bufferIndex = 0;
	rectVertices = rectVertexBuffer->lock();
}

void Text::setBilinearFilter(bool bilinear) {
	end();
	this->bilinear = bilinear;
}

void Text::setFont(Kravur* font) {
	this->font = font;
}

void Text::start() {
	program->set();
	Graphics::setBlendingMode(SourceAlpha, Kore::BlendingOperation::InverseSourceAlpha);
	Graphics::setRenderState(BlendingState, true);
	Graphics::setRenderState(DepthTest, false);
}

void Text::drawString(const char* text, int color, float x, float y, const mat3& transformation) {
	Texture* tex = font->getTexture();
	if (lastTexture != nullptr && tex != lastTexture) drawBuffer();
	lastTexture = tex;

	float xpos = x;
	float ypos = y;
	unsigned length = strlen(text);
	for (unsigned i = 0; i < length; ++i) {
		AlignedQuad q = font->getBakedQuad(text[i] - 32, xpos, ypos);
		if (q.x0 >= 0) {
			if (bufferIndex + 1 >= bufferSize) drawBuffer();
			setRectColors(1.0f, color);
			setRectTexCoords(q.s0 * tex->width / tex->texWidth, q.t0 * tex->height / tex->texHeight, q.s1 * tex->width / tex->texWidth, q.t1 * tex->height / tex->texHeight);
			vec3 p0 = transformation * vec3(q.x0, q.y1, 1.0f); //bottom-left
			vec3 p1 = transformation * vec3(q.x0, q.y0, 1.0f); //top-left
			vec3 p2 = transformation * vec3(q.x1, q.y0, 1.0f); //top-right
			vec3 p3 = transformation * vec3(q.x1, q.y1, 1.0f); //bottom-right
			setRectVertices(p0.x(), p0.y(), p1.x(), p1.y(), p2.x(), p2.y(), p3.x(), p3.y());
			xpos += q.xadvance;
			++bufferIndex;
		}
	}
}

void Text::end() {
	if (bufferIndex > 0) drawBuffer();
	lastTexture = nullptr;
}
