#include "Pch.h"
#include "Gui.h"
#include "Font.h"


//=================================================================================================
Gui* Control::gui;


//=================================================================================================
Container::~Container()
{
	DeleteElements(controls);
}

void Container::Add(Control* control)
{
	assert(control);
	controls.push_back(control);
}

void Container::Draw()
{
	for(Control* ctrl : controls)
	{
		if(ctrl->visible)
			ctrl->Draw();
	}
}

void Container::Update(float dt)
{
	for(Control* ctrl : controls)
		ctrl->Update(dt);
}


//=================================================================================================
Sprite::Sprite() : image(nullptr)
{
}

void Sprite::Draw()
{
	assert(image);
	gui->DrawSprite(image, pos, size);
}


//=================================================================================================
ProgressBar::ProgressBar() : progress(1.f), image_front(nullptr), image_back(nullptr)
{
}

void ProgressBar::Draw()
{
	assert(image_front && image_back);
	gui->DrawSprite(image_back, pos, size);
	if(progress > 0.f)
		gui->DrawSpritePart(image_front, pos, size, Vec2(progress, 1.f));
}


//=================================================================================================
Label::Label() : font(nullptr), color(Color::Black), flags(Font::Left)
{
}

void Label::Draw()
{
	gui->DrawText(text, font, color, 0, Rect::Create(pos, size));
}

void Label::CalculateSize()
{
	Font* f = font ? font : gui->GetDefaultFont();
	size = f->CalculateSize(text);
}


//=================================================================================================
Panel::Panel() : image(nullptr), image_size(0), corner_size(0), pos_grid(4), uv_grid(4), color(Color::White)
{
}

void Panel::Draw()
{
	gui->DrawSpriteGrid(image, color, pos_grid, uv_grid);
	Container::Draw();
}

void Panel::Setup()
{
	assert(image_size > 0
		&& corner_size > 0
		&& image_size - corner_size * 2 > 0
		&& size.x >= corner_size * 2
		&& size.y >= corner_size * 2);
	uv_grid.Set({ 0.f, float(corner_size) / image_size, float(image_size - corner_size) / image_size, 1.f });
	pos_grid.Set({ pos.x, pos.x + corner_size, pos.x + size.x - corner_size, pos.x + size.x },
		{ pos.y, pos.y + corner_size, pos.y + size.y - corner_size, pos.y + size.y });
}
