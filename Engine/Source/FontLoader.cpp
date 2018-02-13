#include "Pch.h"
#include "FontLoader.h"
#include "Font.h"
#include "Render.h"
#include "Texture.h"
#include <gdiplus.h>


//=================================================================================================
Font::Font() : tex(nullptr)
{}

Font::~Font()
{
	delete tex;
}


//=================================================================================================
FontLoader::FontLoader() : render(nullptr)
{}

FontLoader::~FontLoader()
{
	DeleteElements(fonts);
}

void FontLoader::Init(Render* render)
{
	assert(render);
	this->render = render;
}

Font* FontLoader::Create(cstring name, int size)
{
	const int padding = 1;
	int weight = 500; // TODO
	Ptr<Font> font;
	HDC hdc = GetDC(nullptr);
	int logic_size = -MulDiv(size, 96, 72);
	//int logic_size = -MulDiv(size, GetDeviceCaps(hdc, LOGPIXELSX), 72);

	// create winapi font
	HFONT winapi_font = CreateFontA(logic_size, 0, 0, 0, weight, false, false, false, DEFAULT_CHARSET, OUT_TT_PRECIS,
		CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, name);
	if(!winapi_font)
	{
		DWORD error = GetLastError();
		ReleaseDC(nullptr, hdc);
		throw Format("Failed to create font '%s' (%u).", name, error);
	}

	// get glyphs weights, font height
	int glyph_w[256];
	SelectObject(hdc, (HGDIOBJ)winapi_font);
	if(GetCharWidth32(hdc, 0, 255, glyph_w) == 0)
	{
		ABC abc[256];
		if(GetCharABCWidths(hdc, 0, 255, abc) == 0)
		{
			DWORD error = GetLastError();
			DeleteObject(winapi_font);
			ReleaseDC(nullptr, hdc);
			throw Format("Failed to get font '%s' glyphs (%u).", name, error);
		}
		for(int i = 0; i <= 255; ++i)
		{
			ABC& a = abc[i];
			glyph_w[i] = a.abcA + a.abcB + a.abcC;
		}
	}
	TEXTMETRIC tm;
	GetTextMetricsA(hdc, &tm);
	font->height = tm.tmHeight;
	ReleaseDC(nullptr, hdc);

	// calculate texture size
	Int2 tex_size(padding * 2, padding * 2 + font->height);
	for(int i = 32; i <= 255; ++i)
	{
		int width = glyph_w[i];
		if(width)
			tex_size.x += width + padding;
	}
	tex_size.x = NextPow2(tex_size.x);
	tex_size.y = NextPow2(tex_size.y);

	// setup glyphs
	Int2 offset(padding, padding);
	for(int i = 0; i < 32; ++i)
		font->glyph[i].width = 0;
	for(int i = 32; i <= 255; ++i)
	{
		Font::Glyph& glyph = font->glyph[i];
		glyph.width = glyph_w[i];
		if(glyph.width)
		{
			glyph.uv.v1 = Vec2(float(offset.x) / tex_size.x, float(offset.y) / tex_size.y);
			glyph.uv.v2 = glyph.uv.v1 + Vec2(float(glyph.width) / tex_size.x, float(font->height) / tex_size.y);
			offset.x += glyph.width + padding;
		}
	}

	// create texture
	auto device = render->GetDevice();
	auto context = render->GetContext();

	D3D11_TEXTURE2D_DESC desc = { 0 };
	desc.Width = tex_size.x;
	desc.Height = tex_size.y;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE; // D3D11_BIND_SHADER_RESOURCE /*| D3D11_BIND_RENDER_TARGET*/;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.CPUAccessFlags = 0;// D3D11_CPU_ACCESS_WRITE;// | D3D11_CPU_ACCESS_READ;
	desc.MiscFlags = D3D11_RESOURCE_MISC_GDI_COMPATIBLE;

	CPtr<ID3D11Texture2D> tex;
	HRESULT result = device->CreateTexture2D(&desc, nullptr, tex);
	if(FAILED(result))
	{
		DeleteObject(winapi_font);
		throw Format("Failed to create font texture '%s' (%ux%u, result %u).", name, tex_size.x, tex_size.y, result);
	}

	D3D11_RENDER_TARGET_VIEW_DESC target_desc = { 0 };
	target_desc.Format = desc.Format;
	target_desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	target_desc.Texture2D.MipSlice = 0;

	ID3D11RenderTargetView* render_target;
	C(device->CreateRenderTargetView(tex, &target_desc, &render_target));

	context->OMSetRenderTargets(1, &render_target, nullptr);

	context->ClearRenderTargetView(render_target, Vec4(1.f, 1.f, 1.f, 0.f));
	//context->ClearRenderTargetView(render_target, Vec4(1.f, 1.f, 1.f, 1.f));

	//=============================
	// WERSJA 4

	

	//Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	//ULONG_PTR           gdiplusToken;

	// Initialize GDI+.
	//Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	// draw to surface
	IDXGISurface1* surface;
	C(tex->QueryInterface(__uuidof(IDXGISurface1), (void**)&surface));

	C(surface->GetDC(TRUE, &hdc));
	{
//		Gdiplus::Graphics graphics(hdc);
		//graphics.SetTextRenderingHint(Gdiplus::TextRenderingHint::TextRenderingHintClearTypeGridFit);
//		graphics.SetCompositingMode(Gdiplus::CompositingMode::CompositingModeSourceCopy);
		//Gdiplus::Bitmap bmp(tex_size.x, tex_size.y, &graphics);
		//bmp.
		// white
		//Gdiplus::SolidBrush brush(Gdiplus::Color(255, 255, 255, 255));
		// black
//		Gdiplus::SolidBrush brush(Gdiplus::Color(255, 0, 0, 0));
		//Gdiplus::SolidBrush alpha_brush(Gdiplus::Color(0, 255, 255, 255));
//		Gdiplus::Font gdi_font(hdc, winapi_font);
//		Gdiplus::PointF point;

		//graphics.Clear(Gdiplus::Color(255, 255, 255, 255));
		//graphics.Clear(Gdiplus::Color(0, 255, 255, 255));

		SetTextColor(hdc, 0xFFFFFFFF);
		//SetBkMode(hdc, TRANSPARENT);

		offset.x = padding;
		offset.y = padding;
		wchar_t wc[4];
		char c[2];
		c[1] = 0;
		for(int i = 32; i <= 255; ++i)
		{
			Font::Glyph& glyph = font->glyph[i];
			if(glyph.width == 0)
				continue;

			/*c[0] = (char)i;
			mbstowcs(wc, c, 4);
			point.X = (float)offset.x;
			point.Y = (float)offset.y;
			graphics.DrawString(wc, 1, &gdi_font, point, &brush);*/

			char c2 = (char)i;
			TextOutA(hdc, offset.x, offset.y, &c2, 1);
			offset.x += glyph.width + padding;
		}
	}

	C(render->GetSwapChain()->Present(0, 0));
	render_target->Release();
	render_target = render->GetRenderTarget();

	context->OMSetRenderTargets(1, &render_target, render->GetDepthStencilView());

	C(surface->ReleaseDC(nullptr));
	surface->Release();




	/* WERSJA 3 - nie dzia³a

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	result = render->GetContext()->Map(tex, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	for(uint y = 0; y < 32; ++y)
	{
		byte* data = ((byte*)mappedResource.pData) + mappedResource.RowPitch * y;
		uint color = 0x004080FF;
		memset(data, *(int*)&color, 4 * tex_size.x);
	}
	render->GetContext()->Unmap(tex, 0);*/


	/* WERSJA 2 - GDI+

	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR           gdiplusToken;

	// Initialize GDI+.
	Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	// draw to surface
	IDXGISurface1* surface;
	C(tex->QueryInterface(__uuidof(IDXGISurface1), (void**)&surface));

	C(surface->GetDC(TRUE, &hdc));
	{
		Gdiplus::Graphics graphics(hdc);
		Gdiplus::Bitmap bmp(tex_size.x, tex_size.y, &graphics);
		bmp.
		Gdiplus::SolidBrush brush(Gdiplus::Color(255, 255, 255, 255));
		Gdiplus::SolidBrush alpha_brush(Gdiplus::Color(0, 255, 255, 255));
		Gdiplus::Font gdi_font(hdc, winapi_font);
		Gdiplus::PointF point;

		graphics.Clear(Gdiplus::Color(255, 255, 255, 255));
		//graphics.Clear(Gdiplus::Color(0, 255, 255, 255));

		offset.x = padding;
		offset.y = padding;
		wchar_t wc[4];
		char c[2];
		c[1] = 0;
		for(int i = 32; i <= 255; ++i)
		{
			Font::Glyph& glyph = font->glyph[i];
			if(glyph.width == 0)
				continue;
			c[0] = (char)i;
			mbstowcs(wc, c, 4);
			point.X = (float)offset.x;
			point.Y = (float)offset.y;
			//graphics.DrawString(wc, 1, &gdi_font, point, &brush);
			//TextOutA(hdc, offset.x, offset.y, &c, 1);
			offset.x += glyph.width + padding;
		}
	}
	C(surface->ReleaseDC(nullptr));
	surface->Release();*/
	DeleteObject(winapi_font);


	// create texture view
	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};
	SRVDesc.Format = desc.Format;

	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	SRVDesc.Texture2D.MipLevels = 1;

	ID3D11ShaderResourceView* texture_view;
	C(render->GetDevice()->CreateShaderResourceView(tex, &SRVDesc, &texture_view));

	font->tex = new Texture;
	font->tex->tex = texture_view;
	font->tex->type = Resource::Texture;
#ifdef _DEBUG
	font->tex->name = Format("Font %s (%u)", name, size);
#endif

	// make tab size of 4 spaces
	Font::Glyph& tab = font->glyph['\t'];
	Font::Glyph& space = font->glyph[' '];
	tab.width = space.width * 4;;
	tab.uv = space.uv;

	return font.Pin();
}
