#pragma once

#include "Shader.h"
#include "Vertex.h"

struct GuiShader
{
	struct Buffer
	{
		Vec2 size;
		Vec2 _pad;
	};

	GuiShader();
	~GuiShader();
	void Init(Render* render);
	void SetParams();
	void RestoreParams();
	void Draw(Texture* tex, uint count);
	VertexPosTexColor* Lock();
	Texture* GetEmptyTexture() { return empty_texture; }

	static const uint MaxVertex = 256 * 3;

private:
	Render* render;
	Shader shader;
	CPtr<ID3D11SamplerState> sampler;
	CPtr<ID3D11Buffer> vb;
	CPtr<ID3D11BlendState> blend_state, no_blend_state;
	VertexPosTexColor* locked_data;
	Texture* empty_texture;
};
