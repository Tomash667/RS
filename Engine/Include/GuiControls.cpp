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
Sprite::Sprite() : image(nullptr), color(Color::White)
{
}

void Sprite::Draw()
{
	assert(image);
	gui->DrawSprite(image, pos, size, color);
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
		gui->DrawSpritePart(image_front, pos, size, Vec2(Min(progress, 1.f), 1.f));
}


//=================================================================================================
Label::Label() : font(nullptr), color(Color::Black), flags(Font::Left)
{
}

void Label::Draw()
{
	gui->DrawText(text, font, color, flags, Rect::Create(pos, size));
}

void Label::CalculateSize()
{
	Font* f = font ? font : gui->GetDefaultFont();
	size = f->CalculateSize(text);
}


//=================================================================================================
Button::Button() : state(State::Up), id(-1), event(nullptr)
{
	Button* def = (Button*)gui->GetTemplate(typeid(Button));
	if(def)
	{
		font = def->font;
		for(int i = 0; i < 3; ++i)
			image[i] = def->image[i];
		font_color = def->font_color;
		image_color = def->image_color;
		image_size = def->image_size;
		corner_size = def->corner_size;
	}
	else
	{
		font = nullptr;
		for(int i = 0; i < 3; ++i)
			image[i] = nullptr;
		font_color = Color::Black;
		image_color = Color::White;
		image_size = 0;
		corner_size = 0;
	}
}

void Button::Draw()
{
	gui->DrawSpriteGrid(image[(int)state], image_color, image_size, corner_size, pos, size);
	gui->DrawText(text, font, font_color, Font::Center | Font::VCenter, Rect::Create(pos, size));
}

void Button::Update(float dt)
{

}


//=================================================================================================
Panel::Panel()
{
	Panel* def = (Panel*)gui->GetTemplate(typeid(Panel));
	if(def)
	{
		image = def->image;
		color = def->color;
		image_size = def->image_size;
		corner_size = def->corner_size;
	}
	else
	{
		image = nullptr;
		color = Color::White;
		image_size = 0;
		corner_size = 0;
	}
}

void Panel::Draw()
{
	gui->DrawSpriteGrid(image, color, image_size, corner_size, pos, size);
	Container::Draw();
}


//=================================================================================================
DialogBox::DialogBox() : event(nullptr)
{
}

DialogBox::~DialogBox()
{
	DeleteElements(buttons);
}

void DialogBox::AddButton(Cstring text, int id)
{
	Button* button = new Button;
	button->text = text;
	button->id = id;
	buttons.push_back(button);
}

void DialogBox::Show(Gui* gui)
{
	assert(gui);
	this->gui = gui;
	gui->ShowDialog(this);
}

void DialogBox::Draw()
{

}

void DialogBox::Update(float dt)
{

}
