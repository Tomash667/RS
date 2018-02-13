#pragma once

struct Font
{
	struct Glyph
	{
		Box2d uv;
		int width;
	};

	Texture* tex;
	Glyph glyph[256];
	int height;

	Font();
	~Font();
};
