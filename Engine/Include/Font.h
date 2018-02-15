#pragma once

struct Font
{
	enum FontFlags
	{
		Left = 0,
		Center = 1,
		Right = 2,
		Top = 0,
		VCenter = 4,
		Bottom = 8,
		SingleLine = 16
	};

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
	bool SplitLine(uint& out_begin, uint& out_end, int& out_width, uint& in_out_index, cstring text, uint text_end, int flags, int width) const;
	int GetCharWidth(char c) const
	{
		return glyph[(byte)c].width;
	}
};
