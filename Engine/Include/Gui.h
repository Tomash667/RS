#pragma once

//-----------------------------------------------------------------------------
struct Control
{
	virtual ~Control() {}
	virtual void Draw() {}
	virtual void Update(float dt) {}

	Int2 pos, size;
	Gui* gui;
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

	string text;
	Font* font;
	Color color;
};

//-----------------------------------------------------------------------------
struct Panel : Container
{
	void Draw() override;
};

//-----------------------------------------------------------------------------
struct Gui : Container
{
	enum DrawTextFlags
	{
		Left = 0,
		Center = 1,
		Right = 2,
		Top = 0,
		VCenter = 4,
		Bottom = 8
	};

	Gui(Render* render);
	~Gui();
	Font* CreateFont(Cstring name, int size, int weight = 4);
	void Draw() override;
	void DrawSprite(Texture* image, const Int2& pos, const Int2& size);
	void DrawSpritePart(Texture* image, const Int2& pos, const Int2& size, const Vec2& part);
	void DrawText(Cstring text, Font* font, Color color, int flags, const Rect& rect);
	void Init();

private:
	Render* render;
	GuiShader* shader;
	FontLoader* font_loader;
	vector<Control*> controls;
	Font* default_font;
};
