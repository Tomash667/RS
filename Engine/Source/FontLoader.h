#pragma once

struct FontLoader
{
	FontLoader();
	~FontLoader();
	void Init(Render* render);
	Font* Load(cstring name, int size, int weight);

private:
	void InitGdi();
	void RenderFontToTexture(ID3D11Texture2D* tex, Font* font, void* winapi_font);

	Render* render;
	vector<Font*> fonts;
	const int padding = 2;
	bool gdi_initialized;
};
