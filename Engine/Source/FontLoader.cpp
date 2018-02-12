#include "Pch.h"
#include "FontLoader.h"
#include "Font.h"

/*
load from cache
if not exists: https://stackoverflow.com/questions/5979632/d3d11-how-to-draw-gdi-text-to-a-gxdi-surface-without-d2d
make texture: total size x font height
*/

void FontLoader::Create(cstring name, int size)
{
	int weight = 500; // TODO
	Ptr<Font> font;
	HDC hdc = GetDC(nullptr);
	int logic_size = -MulDiv(size, 96, 72);
	//int logic_size = -MulDiv(size, GetDeviceCaps(hdc, LOGPIXELSX), 72);

	// create winapi font
	HFONT winapi_font = CreateFontA(logic_size, 0, 0, 0, weight, false, false, false, DEFAULT_CHARSET, OUT_TT_PRECIS,
		CLIP_DEFAULT_PRECIS, PROOF_QUALITY, DEFAULT_PITCH | FF_DONTCARE, name);
	if(!winapi_font)
	{
		DWORD error = GetLastError();
		ReleaseDC(nullptr, hdc);
		//Error("Failed to create font (%s, size:%d, weight:%d, code:%d).", name, size, weight, error);
		//return nullptr;
	}

	// get glyphs weights, font height
	int glyph_w[256];
	HGDIOBJ prev = SelectObject(hdc, (HGDIOBJ)winapi_font);
	if(GetCharWidth32(hdc, 0, 255, glyph_w) == 0)
	{
		ABC abc[256];
		if(GetCharABCWidths(hdc, 0, 255, abc) == 0)
		{
			//Error("B³¹d pobierania szerokoœci znaków (%s, rozmiar:%d, waga:%d, b³¹d:%d).", name, size, weight, GetLastError());
			SelectObject(hdc, prev);
			DeleteObject(winapi_font);
			ReleaseDC(nullptr, hdc);
			//return nullptr;
		}
		for(int i = 0; i <= 255; ++i)
		{
			ABC& a = abc[i];
			glyph_w[i] = a.abcA + a.abcB + a.abcC;
		}
	}
	TEXTMETRIC tm;
	GetTextMetricsA(hdc, &tm);
	int height = tm.tmHeight;
	SelectObject(hdc, prev);
	DeleteObject(winapi_font);
	ReleaseDC(nullptr, hdc);

	//return nullptr;
}
