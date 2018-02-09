#pragma once

struct Control
{
	virtual ~Control() {}
	virtual void Draw() {}

	Int2 pos, size;
	Gui* gui;
};

struct Sprite : Control
{
	Sprite();
	void Draw() override;

	Texture* image;
};

struct ProgressBar : Control
{
	ProgressBar();
	void Draw() override;

	Texture* image_front, *image_back;
	float progress;
};

struct Gui
{
	Gui(Render* render);
	~Gui();
	void Add(Control* control);
	void Draw();
	void DrawSprite(Texture* image, const Int2& pos, const Int2& size);
	void DrawSpritePart(Texture* image, const Int2& pos, const Int2& size, const Vec2& part);
	void Init();

private:
	Render* render;
	GuiShader* shader;
	vector<Control*> controls;
};
