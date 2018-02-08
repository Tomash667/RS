#include "Pch.h"
#include "Gui.h"
#include "GuiShader.h"


void Sprite::Draw()
{
	assert(image);
	gui->DrawSprite(image, pos, size);
}


Gui::Gui(Render* render) : render(render), shader(new GuiShader)
{
	assert(render);
}

Gui::~Gui()
{
	DeleteElements(controls);
	delete shader;
}

void Gui::Add(Control* control)
{
	assert(control);
	control->gui = this;
	controls.push_back(control);
}

void Gui::Draw()
{
	shader->SetParams();
	for(Control* control : controls)
		control->Draw();
}

void Gui::DrawSprite(Texture* image, const Int2& pos, const Int2& size)
{
	GuiShader::Vertex v[6] = {
		{ { float(pos.x), float(pos.y), 0.f }, { 0.f, 0.f } },
		{ { float(pos.x + size.x), float(pos.y), 0.f },{ 1.f, 0.f } },
		{ { float(pos.x), float(pos.y + size.y), 0.f },{ 0.f, 1.f } },
		{ { float(pos.x + size.x), float(pos.y), 0.f },{ 1.f, 0.f } },
		{ { float(pos.x + size.x), float(pos.y + size.y), 0.f },{ 1.f, 1.f } },
		{ { float(pos.x), float(pos.y + size.y), 0.f },{ 0.f, 1.f } },
	};
	shader->Draw(image, v, 6);
}

void Gui::Init()
{
	shader->Init(render);
}
