#pragma once

#include <Kore/Graphics/Graphics.h>
#include <Kore/IO/Reader.h>
#include <vector>

struct FontStyle {
	bool bold;
	bool italic;
	bool underlined;

	FontStyle() : bold(false), italic(false), underlined(false) { }
	FontStyle(bool bold, bool italic, bool underlined) : bold(bold), italic(italic), underlined(underlined) { }
};

struct BakedChar {
	BakedChar() { x0 = -1; }
	
	// coordinates of bbox in bitmap
	int x0;
	int y0;
	int x1;
	int y1;

	float xoff;
	float yoff;
	float xadvance;
};

struct AlignedQuad {
	AlignedQuad() { x0 = -1; }

	// top-left
	float x0;
	float y0;
	float s0;
	float t0;

	// bottom-right
	float x1;
	float y1;
	float s1;
	float t1;

	float xadvance;
};

class Kravur {
public:
	const char* name;
	FontStyle style;
	float size;
private:
	std::vector<BakedChar> chars;
	Kore::Texture* texture;
	int width;
	int height;
	float baseline;
public:
	static Kravur* load(const char* name, FontStyle style, float size);
private:
	Kravur(Kore::Reader* reader);
	float getCharWidth(int charIndex);
public:
	AlignedQuad getBakedQuad(int char_index, float xpos, float ypos);
	Kore::Texture* getTexture();
	float getHeight();
	float charWidth(char ch);
	float charsWidth(const char* ch, int offset, int length);
	float stringWidth(const char* string, int length = -1);
	float getBaselinePosition();
};
