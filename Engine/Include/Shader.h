#pragma once

struct Shader
{
	ID3D11VertexShader* vertex_shader;
	ID3D11PixelShader* pixel_shader;
	ID3D11InputLayout* layout;
	ID3D11Buffer* buffer;
	uint vertex_size;

	Shader();
	~Shader();
};
