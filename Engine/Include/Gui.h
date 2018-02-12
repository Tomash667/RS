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
	void Draw() override;

	string text;
};

//-----------------------------------------------------------------------------
struct Panel : Container
{
	void Draw() override;
};

//-----------------------------------------------------------------------------
struct Gui : Container
{
	Gui(Render* render);
	~Gui();
	Font* CreateFont(Cstring name, int size);
	void Draw() override;
	void DrawSprite(Texture* image, const Int2& pos, const Int2& size);
	void DrawSpritePart(Texture* image, const Int2& pos, const Int2& size, const Vec2& part);
	void Init();

private:
	Render* render;
	GuiShader* shader;
	vector<Control*> controls;
};
