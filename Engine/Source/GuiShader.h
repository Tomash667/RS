#pragma once

#include "Shader.h"

struct GuiShader
{
	struct Vertex
	{
		Vec3 pos;
		Vec2 tex;
	};

	GuiShader();
	void Init(Render* render);
	void SetParams();
	void Draw(Texture* tex, Vertex* v, uint count);

	static const uint MaxVertex = 6;

private:
	Render* render;
	Shader shader;
	CPtr<ID3D11SamplerState> sampler;
	CPtr<ID3D11Buffer> vb;
	CPtr<ID3D11BlendState> blend_state;
};
