#pragma once

struct MeshShader
{
	MeshShader();
	~MeshShader();
	void Init(Render* render);
	void SetParams(bool is_anim);
	void SetBuffer(const Matrix& matWorldViewProj, const vector<Matrix>* matBones);
	void Draw(Mesh* mesh);

private:
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

	struct Buffer
	{
		Matrix matWorldViewProj;
	};

	struct AniBuffer
	{
		Matrix matWorldViewProj;
		Matrix matBones[32];
	};

	void InitMeshShader();
	void InitAnimatedMeshShader();
	void InitShader(Shader& shader, cstring filename, D3D11_INPUT_ELEMENT_DESC* desc, uint desc_count, uint cbuffer_size);

	Render* render;
	Shader mesh_shader, animated_shader;
	ID3D11SamplerState* sampler;
	Shader* current_shader;
};
