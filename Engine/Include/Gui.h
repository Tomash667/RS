#pragma once

#include "Vertex.h"

//-----------------------------------------------------------------------------
enum class AutoSize
{
	Manual = 0,
	Grow = 1,
	Shrink = 2,
	Auto = 3
};

//-----------------------------------------------------------------------------
struct Control
{
	Control() : pos(Int2::Zero), size(Int2::Zero), min_size(Int2::Zero), max_size(Int2::MaxValue), gui(nullptr), parent(nullptr) {}
	virtual ~Control() {}
	virtual void Draw() {}
	virtual void Update(float dt) {}
	void OnDraw();

	Int2 pos, size, min_size, max_size;
	Gui* gui;
	Container* parent;
};

//-----------------------------------------------------------------------------
struct Container : Control
{
	Container() : auto_size(AutoSize::Auto) {}
	~Container();
	void Add(Control* control);
	void Draw() override;
	void Update(float dt) override;

	AutoSize auto_size;
private:
	vector<Control*> controls;
};

//-----------------------------------------------------------------------------
struct Sprite : Control
{
	Sprite();
	void Draw() override;

	Texture* image;
};

//-----------------------------------------------------------------------------
struct ProgressBar : Control
{
	ProgressBar();
	void Draw() override;

	Texture* image_front, *image_back;
	float progress;
};

//-----------------------------------------------------------------------------
struct Label : Control
{
	Label();
	void Draw() override;
	void SetText(Cstring text);
	Font* GetUsedFont();

	string text;
	Color color;
	int flags;
	Font* font;
	AutoSize auto_size;
};

//-----------------------------------------------------------------------------
struct Panel : Container
{
	Panel();
	void Draw() override;
	void Setup();

	Texture* image;
	Color color;
	int image_size, corner_size;
	Grid<float, Vec2> pos_grid, uv_grid;
};

//-----------------------------------------------------------------------------
struct Gui : Container
{
	Gui(Render* render);
	~Gui();
	Font* CreateFont(Cstring name, int size, int weight = 4);
	void Init();
	void Draw() override;

	void Draw(Control* control);
	void DrawSprite(Texture* image, const Int2& pos, const Int2& size);
	void DrawSpritePart(Texture* image, const Int2& pos, const Int2& size, const Vec2& part);
	bool DrawText(Cstring text, Font* font, Color color, int flags, const Rect& rect, const Rect* clip = nullptr);

	Font* GetDefaultFont() { return default_font; }

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
	void SplitTextLines(cstring text);

	Render* render;
	GuiShader* shader;
	FontLoader* font_loader;
	Font* default_font;
	VertexPosTexColor* v;
	uint in_buffer;
	vector<TextLine> lines;
	Control* current;
	Int2 offset;
};


inline void Control::OnDraw()
{
	gui->Draw(this);
}
