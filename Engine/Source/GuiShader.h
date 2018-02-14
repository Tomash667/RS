#pragma once

#include "Shader.h"

struct GuiShader
{
	struct Buffer
	{
		Vec4 color;
		Vec2 size;
		Vec2 _pad;
	};

	struct Vertex
	{
		Vec3 pos;
		Vec2 tex;
	};

	GuiShader();
	void Init(Render* render);
	void SetParams();
	void SetGlobals(Color color, bool force = false);
	void RestoreParams();
	void Draw(Texture* tex, Vertex* v, uint count);

	static const uint MaxVertex = 6;

private:
	Render* render;
	Shader shader;
	CPtr<ID3D11SamplerState> sampler;
	CPtr<ID3D11Buffer> vb;
	CPtr<ID3D11BlendState> blend_state, no_blend_state;
	Color current_color;
};
