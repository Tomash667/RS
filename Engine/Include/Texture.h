#pragma once

#include "Resource.h"

struct Texture final : Resource
{
	~Texture();

	ID3D11ShaderResourceView* tex;
};
