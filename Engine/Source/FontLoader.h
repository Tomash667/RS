#pragma once

struct FontLoader
{
	FontLoader();
	~FontLoader();
	void Init(Render* render);
	Font* Create(cstring name, int size);

private:
	Render* render;
	vector<Font*> fonts;
};
