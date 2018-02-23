#include "Pch.h"
#include "MeshShader.h"
#include "Mesh.h"
#include "Texture.h"
#include "Render.h"


MeshShader::MeshShader() : current_shader(nullptr)
{
}

void MeshShader::Init(Render* render)
{
	assert(render);
	this->render = render;

	InitMeshShader();
	InitAnimatedMeshShader();

	// create texture sampler
	D3D11_SAMPLER_DESC sampler_desc;
	sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.MipLODBias = 0.0f;
	sampler_desc.MaxAnisotropy = 1;
	sampler_desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	sampler_desc.BorderColor[0] = 0;
	sampler_desc.BorderColor[1] = 0;
	sampler_desc.BorderColor[2] = 0;
	sampler_desc.BorderColor[3] = 0;
	sampler_desc.MinLOD = 0;
	sampler_desc.MaxLOD = D3D11_FLOAT32_MAX;

	HRESULT result = render->GetDevice()->CreateSamplerState(&sampler_desc, sampler);
	if(FAILED(result))
		throw Format("Failed to create sampler state (%u).", result);
}

void MeshShader::InitMeshShader()
{
	D3D11_INPUT_ELEMENT_DESC desc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	uint cbuffer_size[] = { sizeof(Buffer), sizeof(PsBuffer) };
	render->CreateShader(mesh_shader, "mesh.hlsl", desc, countof(desc), cbuffer_size);
	mesh_shader.vertex_size = sizeof(Vertex);
}

void MeshShader::InitAnimatedMeshShader()
{
	D3D11_INPUT_ELEMENT_DESC desc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BLENDWEIGHT", 0, DXGI_FORMAT_R32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BLENDINDICES", 0, DXGI_FORMAT_R8G8B8A8_UINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	uint cbuffer_size[] = { sizeof(AniBuffer), 0 };
	render->CreateShader(animated_shader, "animated.hlsl", desc, countof(desc), cbuffer_size);
	animated_shader.ps_buffer = mesh_shader.ps_buffer;
	animated_shader.ps_buffer->AddRef();
	animated_shader.vertex_size = sizeof(AniVertex);
}

void MeshShader::ResetParams()
{
	current_shader = nullptr;
	auto context = render->GetContext();
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->PSSetSamplers(0, 1, &sampler.Get());
	current_tint = Vec3(-1, -1, -1);
}

void MeshShader::SetParams(bool is_animated)
{
	Shader* shader_ptr = &(is_animated ? animated_shader : mesh_shader);
	if(current_shader == shader_ptr)
		return;
	current_shader = shader_ptr;

	auto context = render->GetContext();
	auto& shader = *shader_ptr;

	context->IASetInputLayout(shader.layout);
	context->VSSetConstantBuffers(0, 1, &shader.vs_buffer);
	context->VSSetShader(shader.vertex_shader, nullptr, 0);
	context->PSSetConstantBuffers(0, 1, &shader.ps_buffer);
	context->PSSetShader(shader.pixel_shader, nullptr, 0);
}

void MeshShader::SetBuffer(const Vec3& tint, const Matrix& matWorldViewProj, const vector<Matrix>* matBones)
{
	auto& shader = (matBones ? animated_shader : mesh_shader);
	auto context = render->GetContext();
	D3D11_MAPPED_SUBRESOURCE mappedResource;

	C(context->Map(shader.vs_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource));
	if(matBones)
	{
		AniBuffer& b = *(AniBuffer*)mappedResource.pData;
		b.matWorldViewProj = matWorldViewProj.Transpose();
		for(size_t i = 0, count = matBones->size(); i < count; ++i)
			b.matBones[i] = matBones->at(i).Transpose();
	}
	else
	{
		Buffer& b = *(Buffer*)mappedResource.pData;
		b.matWorldViewProj = matWorldViewProj.Transpose();
	}
	context->Unmap(shader.vs_buffer, 0);

	if(tint != current_tint)
	{
		current_tint = tint;
		C(context->Map(shader.ps_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource));
		PsBuffer& b = *(PsBuffer*)mappedResource.pData;
		b.tint = Vec4(tint, 1.f);
		context->Unmap(shader.ps_buffer, 0);
	}
}

void MeshShader::Draw(Mesh* mesh)
{
	auto context = render->GetContext();
	uint stride = current_shader->vertex_size,
		offset = 0;
	context->IASetVertexBuffers(0, 1, &mesh->vb, &stride, &offset);
	context->IASetIndexBuffer(mesh->ib, DXGI_FORMAT_R16_UINT, 0);

	for(auto& sub : mesh->subs)
	{
		context->PSSetShaderResources(0, 1, sub.tex ? &sub.tex->tex : nullptr);
		context->DrawIndexed(sub.tris * 3, sub.first * 3, sub.min_ind);
	}
}
