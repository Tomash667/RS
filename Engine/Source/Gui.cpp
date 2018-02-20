#include "Pch.h"
#include "Gui.h"
#include "GuiShader.h"
#include "FontLoader.h"
#include "Font.h"


Gui::Gui(Render* render) : render(render), shader(new GuiShader), font_loader(new FontLoader), v(nullptr)
{
	assert(render);
	gui = this;
}

Gui::~Gui()
{
	delete font_loader;
	delete shader;
}

void Gui::Init()
{
	shader->Init(render);
	font_loader->Init(render);
	default_font = CreateFont("Arial", 14);
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
	Lock();
	FillQuad(Box2d::Create(pos, size), Box2d(0.f, 0.f, 1.f, 1.f), Color::White);
	Flush(image);
}

void Gui::DrawSpritePart(Texture* image, const Int2& pos, const Int2& size, const Vec2& part)
{
	Lock();
	FillQuad(Box2d::Create(pos, size * part), Box2d(Vec2::Zero, part), Color::White);
	Flush(image);
}

void Gui::DrawSpriteGrid(Texture* image, Color color, const GridF& pos, const GridF& uv)
{
	assert(pos.size == uv.size);
	Lock();
	Vec4 current_color = color;
	for(uint y = 0; y < pos.size - 1; ++y)
	{
		for(uint x = 0; x < pos.size - 1; ++x)
		{
			v->pos = pos(x, y);
			v->tex = uv(x, y);
			v->color = current_color;
			++v;

			v->pos = pos(x + 1, y);
			v->tex = uv(x + 1, y);
			v->color = current_color;
			++v;

			v->pos = pos(x, y + 1);
			v->tex = uv(x, y + 1);
			v->color = current_color;
			++v;

			v->pos = pos(x + 1, y);
			v->tex = uv(x + 1, y);
			v->color = current_color;
			++v;

			v->pos = pos(x + 1, y + 1);
			v->tex = uv(x + 1, y + 1);
			v->color = current_color;
			++v;

			v->pos = pos(x, y + 1);
			v->tex = uv(x, y + 1);
			v->color = current_color;
			++v;

			++in_buffer;
		}
	}
	Flush(image);
}

bool Gui::DrawText(Cstring text, Font* font, Color color, int flags, const Rect& rect, const Rect* clip)
{
	if(!font)
		font = default_font;

	int width = rect.SizeX();
	Vec4 current_color = color;
	bool bottom_clip = false;

	Lock();
	SplitTextLines(text, font, width, flags);

	int y;
	if(IS_SET(flags, Font::Bottom))
		y = rect.Bottom() - int(lines.size())*font->height;
	if(IS_SET(flags, Font::VCenter))
		y = rect.Top() + (rect.SizeY() - int(lines.size())*font->height) / 2;
	else
		y = rect.Top();

	for(TextLine& line : lines)
	{
		int x;
		if(IS_SET(flags, Font::Center))
			x = rect.Left() + (width - line.width) / 2;
		else if(IS_SET(flags, Font::Right))
			x = rect.Right() - line.width;
		else
			x = rect.Left();

		ClipResult clip_result = Clip(x, y, line.width, font->height, clip);
		if(clip_result == ClipNone)
			DrawTextLine(font, text, line.begin, line.end, current_color, x, y, nullptr);
		else if(clip_result == ClipPartial)
			DrawTextLine(font, text, line.begin, line.end, current_color, x, y, clip);
		else if(clip_result == ClipBelow)
		{
			// text is below visible region, stop drawing
			bottom_clip = true;
			break;
		}

		y += font->height;
	}

	Flush(font->tex);
	return !bottom_clip;
}

void Gui::SplitTextLines(cstring text, Font* font, int width, int flags)
{
	uint line_begin, line_end, line_index = 0;
	int line_width;
	uint text_end = (uint)strlen(text);

	lines.clear();
	while(font->SplitLine(line_begin, line_end, line_width, line_index, text, text_end, flags, width))
		lines.push_back(TextLine(line_begin, line_end, line_width));
}

void Gui::DrawTextLine(Font* font, cstring text, uint line_begin, uint line_end, const Vec4& color, int x, int y, const Rect* clip)
{
	for(uint i = line_begin; i < line_end; ++i)
	{
		Font::Glyph& glyph = font->glyph[byte(text[i])];
		Int2 glyph_size = Int2(glyph.width, font->height);

		ClipResult clip_result = Clip(x, y, glyph_size.x, glyph_size.y, clip);
		if(clip_result == ClipNone)
			FillQuad(Box2d::Create(Int2(x, y), glyph_size), glyph.uv, color);
		else if(clip_result == ClipPartial)
		{
			Box2d orig_pos = Box2d::Create(Int2(x, y), glyph_size);
			Box2d clip_pos(float(max(x, clip->Left())), float(max(y, clip->Top())),
				float(min(x + glyph_size.x, clip->Right())), float(min(y + glyph_size.y, clip->Bottom())));
			Vec2 orig_size = orig_pos.Size();
			Vec2 clip_size = clip_pos.Size();
			Vec2 s(clip_size.x / orig_size.x, clip_size.y / orig_size.y);
			Vec2 shift = clip_pos.v1 - orig_pos.v1;
			shift.x /= orig_size.x;
			shift.y /= orig_size.y;
			Vec2 uv_size = glyph.uv.Size();
			Box2d clip_uv(glyph.uv.v1 + Vec2(shift.x*uv_size.x, shift.y*uv_size.y));
			clip_uv.v2 += Vec2(uv_size.x*s.x, uv_size.y*s.y);
			FillQuad(clip_pos, clip_uv, color);
		}
		else if(clip_result == ClipRight)
		{
			// text is outside visible region on right, stop drawing line
			break;
		}

		x += glyph_size.x;
		if(in_buffer == GuiShader::MaxQuads)
			Flush(font->tex, true);
	}
}

Gui::ClipResult Gui::Clip(int x, int y, int w, int h, const Rect* clip)
{
	if(!clip)
		return ClipNone;
	else if(x >= clip->Left() && y >= clip->Top() && x + w < clip->Right() && y + h < clip->Bottom())
		return ClipNone;
	else if(y + h < clip->Top())
		return ClipAbove;
	else if(y > clip->Bottom())
		return ClipBelow;
	else if(x > clip->Right())
		return ClipRight;
	else if(x + w < clip->Left())
		return ClipLeft;
	else
		return ClipPartial;
}

void Gui::Lock()
{
	assert(!v);
	v = shader->Lock();
	in_buffer = 0;
}

void Gui::Flush(Texture* tex, bool lock)
{
	assert(tex && v);
	shader->Draw(tex, in_buffer * 6);
	if(lock)
	{
		v = shader->Lock();
		in_buffer = 0;
	}
	else
		v = nullptr;
}

void Gui::FillQuad(const Box2d& pos, const Box2d& tex, const Vec4& color)
{
	v->pos = pos.LeftTop();
	v->tex = tex.LeftTop();
	v->color = color;
	++v;

	v->pos = pos.RightTop();
	v->tex = tex.RightTop();
	v->color = color;
	++v;

	v->pos = pos.LeftBottom();
	v->tex = tex.LeftBottom();
	v->color = color;
	++v;

	v->pos = pos.RightTop();
	v->tex = tex.RightTop();
	v->color = color;
	++v;

	v->pos = pos.RightBottom();
	v->tex = tex.RightBottom();
	v->color = color;
	++v;

	v->pos = pos.LeftBottom();
	v->tex = tex.LeftBottom();
	v->color = color;
	++v;

	++in_buffer;
}
