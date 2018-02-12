#pragma once

struct Font
{
	struct Glyph
	{
		Box2d uv;
		int width;
		bool ok;
	};

	Texture* tex;
	Glyph glyph[256];
	int height;
};
