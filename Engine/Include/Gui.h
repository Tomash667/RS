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
	void Draw() override;

	Texture* image;
};

struct Gui
{
	Gui(Render* render);
	~Gui();
	void Add(Control* control);
	void Draw();
	void DrawSprite(Texture* image, const Int2& pos, const Int2& size);
	void Init();

private:
	Render* render;
	GuiShader* shader;
	vector<Control*> controls;
};
