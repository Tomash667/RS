#pragma once

#include "Vertex.h"

//-----------------------------------------------------------------------------
struct Control
{
	enum SizeMode
	{
		Manual = 0,
		GrowX = 1,
		GrowY = 2,
		Grow = GrowX | GrowY,
		ShrinkX = 4,
		ShrinkY = 8,
		Shrink = ShrinkX | ShrinkY,
		Auto = Grow | Shrink
	};

	Control() : pos(0, 0), size(0, 0), offset(0, 0), gui(nullptr), parent(nullptr), size_mode(Auto) {}
	virtual ~Control() {}
	virtual void Draw() {}
	virtual void Update(float dt) {}

	Int2 pos, size, offset;
	Gui* gui;
	SizeMode size_mode;
	Container* parent;
};

//-----------------------------------------------------------------------------
struct Container : Control
{
	~Container();
	void Add(Control* control);
	void Draw() override;
	void Update(float dt) override;

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
	template<typename... Args>
	void SetText(Cstring msg, const Args&... args)
	{
		SetText(Format(msg, args...));
	}
	const string& GetText() const { return text; }

	string text;
	Font* font;
	Color color;
	int flags;
};

template<typename Type, typename PackedType>
struct Grid
{
	Grid(uint size) : size(size)
	{
		values.resize(size * 2);
	}

	template<typename T>
	void Set(std::initializer_list<T> const& new_values)
	{
		assert(size == new_values.size());
		uint i = 0;
		for(T val : new_values)
		{
			values[i] = (Type)val;
			values[size + i] = (Type)val;
			++i;
		}
	}

	template<typename T>
	void Set(std::initializer_list<T> const& new_x, std::initializer_list<T> const& new_y)
	{
		assert(size == new_x.size() && size == new_y.size());
		uint i = 0;
		for(T val : new_x)
		{
			values[i] = (Type)val;
			++i;
		}
		i = 0;
		for(T val : new_y)
		{
			values[size + i] = (Type)val;
			++i;
		}
	}

	PackedType operator () (int x, int y)
	{
		return PackedType(values[x], values[size + y]);
	}

	vector<Type> values;
	uint size;
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

	void DrawSprite(Texture* image, const Int2& pos, const Int2& size);
	void DrawSpritePart(Texture* image, const Int2& pos, const Int2& size, const Vec2& part);
	bool DrawText(Cstring text, Font* font, Color color, int flags, const Rect& rect, const Rect* clip = nullptr);

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

	Render* render;
	GuiShader* shader;
	FontLoader* font_loader;
	vector<Control*> controls;
	Font* default_font;
	VertexPosTexColor* v;
	uint in_buffer;
	vector<TextLine> lines;
};
