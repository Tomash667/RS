#pragma once

#include "GuiControls.h"
#include "Vertex.h"

struct Gui : Container
{
	Gui(Render* render);
	~Gui();
	Font* CreateFont(Cstring name, int size, int weight = 4);
	void Init(const Int2& wnd_size);
	void Draw(const Matrix& mat_view_proj, const Int2& wnd_size);
	bool To2dPoint(const Vec3& pos, Int2& pt);
	void ShowDialog(DialogBox* dialog);

	void DrawSprite(Texture* image, const Int2& pos, const Int2& size, Color color = Color::White);
	void DrawSpritePart(Texture* image, const Int2& pos, const Int2& size, const Vec2& part, Color color = Color::White);
	void DrawSpriteGrid(Texture* image, Color color, const GridF& pos, const GridF& uv);
	bool DrawText(Cstring text, Font* font, Color color, int flags, const Rect& rect, const Rect* clip = nullptr);
	void DrawRect(Texture* image, const Rect& rect, Color color = Color::White);
	void DrawSpriteCircle(Texture* image, float start_angle, float dir, float t, const Vec2& center, float r, Color color = Color::White);

	Font* GetDefaultFont() { return default_font; }
	const Int2& GetWindowSize() { return wnd_size; }

private:
	enum ClipResult
	{
		ClipNone,
		ClipAbove,
		ClipBelow,
		ClipRight,
		ClipLeft,
		ClipPartial
	};

	struct TextLine
	{
		uint begin, end;
		int width;

		TextLine(uint begin, uint end, int width) : begin(begin), end(end), width(width)
		{
		}
	};

	ClipResult Clip(int x, int y, int w, int h, const Rect* clip);
	void DrawTextLine(Font* font, cstring text, uint line_begin, uint line_end, const Vec4& color, int x, int y, const Rect* clip);
	void Lock();
	void Flush(Texture* tex, bool lock = false);
	void FillQuad(const Box2d& pos, const Box2d& tex, const Vec4& color);
	void SplitTextLines(cstring text, Font* font, int width, int flags);

	Render* render;
	GuiShader* shader;
	FontLoader* font_loader;
	Font* default_font;
	VertexPosTexColor* v;
	uint in_buffer;
	vector<TextLine> lines;
	Int2 wnd_size;
	Matrix mat_view_proj;
};
