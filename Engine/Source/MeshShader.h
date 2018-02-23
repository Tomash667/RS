#pragma once

#include "Shader.h"

struct MeshShader
{
	MeshShader();
	void Init(Render* render);
	void ResetParams();
	void SetParams(bool is_anim);
	void SetBuffer(const Vec3& tint, const Matrix& matWorldViewProj, const vector<Matrix>* matBones);
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

	struct PsBuffer
	{
		Vec4 tint;
	};

	void InitMeshShader();
	void InitAnimatedMeshShader();

	Render* render;
	Shader mesh_shader, animated_shader;
	CPtr<ID3D11SamplerState> sampler;
	Shader* current_shader;
	Vec3 current_tint;
};
