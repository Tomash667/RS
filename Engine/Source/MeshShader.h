#pragma once

#include "Shader.h"

struct MeshShader
{
	MeshShader();
	void Init(Render* render);
	void ResetParams();
	void SetParams(bool is_anim);
	void SetBuffer(const Matrix& matWorldViewProj, const vector<Matrix>* matBones);
	void Draw(Mesh* mesh);

private:
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

	Render* render;
	Shader mesh_shader, animated_shader;
	CPtr<ID3D11SamplerState> sampler;
	Shader* current_shader;
};
