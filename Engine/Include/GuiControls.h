#pragma once

//-----------------------------------------------------------------------------
struct Control
{
	Control() : pos(Int2::Zero), size(Int2::Zero), visible(true) {}
	virtual ~Control() {}
	virtual void Draw() {}
	virtual void Update(float dt) {}

	Int2 pos, size;
	bool visible;
	static Gui* gui;
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
	void CalculateSize();

	string text;
	Color color;
	int flags;
	Font* font;
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
	GridF pos_grid, uv_grid;
};
