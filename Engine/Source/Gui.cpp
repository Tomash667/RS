#include "Pch.h"
#include "Gui.h"
#include "GuiShader.h"
#include "FontLoader.h"


//=================================================================================================
Container::~Container()
{
	DeleteElements(controls);
}

void Container::Add(Control* control)
{
	assert(control);
	control->gui = gui;
	controls.push_back(control);
}

void Container::Draw()
{
	for(Control* ctrl : controls)
		ctrl->Draw();
}

void Container::Update(float dt)
{
	for(Control* ctrl : controls)
		ctrl->Update(dt);
}


//=================================================================================================
Sprite::Sprite() : image(nullptr)
{}

void Sprite::Draw()
{
	assert(image);
	gui->DrawSprite(image, pos, size);
}


//=================================================================================================
ProgressBar::ProgressBar() : progress(1.f), image_front(nullptr), image_back(nullptr)
{}

void ProgressBar::Draw()
{
	assert(image_front && image_back);
	gui->DrawSprite(image_back, pos, size);
	if(progress > 0.f)
		gui->DrawSpritePart(image_front, pos, size, Vec2(progress, 1.f));
}


//=================================================================================================
Label::Label() : font(nullptr), color(Color::Black)
{}

void Label::Draw()
{
	gui->DrawText(text, font, color, 0, Rect::Create(pos, size));
}


//=================================================================================================
void Panel::Draw()
{

}


//=================================================================================================
Gui::Gui(Render* render) : render(render), shader(new GuiShader), font_loader(new FontLoader)
{
	assert(render);
	gui = this;
}

Gui::~Gui()
{
	delete font_loader;
	delete shader;
}

Font* Gui::CreateFont(Cstring name, int size, int weight)
{
	assert(size > 0 && weight > 0 && weight <= 10);
	return font_loader->Load(name, size, weight * 100);
}

void Gui::Draw()
{
	shader->SetParams();
	Container::Draw();
	shader->RestoreParams();
}

void Gui::DrawSprite(Texture* image, const Int2& pos, const Int2& size)
{
	GuiShader::Vertex v[6] = {
		{ { float(pos.x), float(pos.y), 0.f }, { 0.f, 0.f } },
		{ { float(pos.x + size.x), float(pos.y), 0.f }, { 1.f, 0.f } },
		{ { float(pos.x), float(pos.y + size.y), 0.f }, { 0.f, 1.f } },
		{ { float(pos.x + size.x), float(pos.y), 0.f }, { 1.f, 0.f } },
		{ { float(pos.x + size.x), float(pos.y + size.y), 0.f }, { 1.f, 1.f } },
		{ { float(pos.x), float(pos.y + size.y), 0.f }, { 0.f, 1.f } },
	};
	shader->SetGlobals(Color::White);
	shader->Draw(image, v, 6);
}

void Gui::DrawSpritePart(Texture* image, const Int2& pos, const Int2& size, const Vec2& part)
{
	GuiShader::Vertex v[6] = {
		{ { float(pos.x), float(pos.y), 0.f },{ 0.f, 0.f } },
		{ { float(pos.x + part.x * size.x), float(pos.y), 0.f }, { part.x, 0.f } },
		{ { float(pos.x), float(pos.y + size.y), 0.f }, { 0.f, part.y } },
		{ { float(pos.x + part.x * size.x), float(pos.y), 0.f }, { part.x, 0.f } },
		{ { float(pos.x + part.x * size.x), float(pos.y + part.y * size.y), 0.f }, { part.x, part.y } },
		{ { float(pos.x), float(pos.y + part.y * size.y), 0.f }, { 0.f, part.y } },
	};
	shader->SetGlobals(Color::White);
	shader->Draw(image, v, 6);
}

void Gui::DrawText(Cstring text, Font* font, Color color, int flags, const Rect& rect)
{
	if(!font)
		font = default_font;
	shader->SetGlobals(color);
}

void Gui::Init()
{
	shader->Init(render);
	font_loader->Init(render);
	default_font = CreateFont("Arial", 14);
}
