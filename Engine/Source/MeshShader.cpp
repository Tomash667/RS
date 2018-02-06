#include "Pch.h"
#include "MeshShader.h"
#include "Mesh.h"
#include "Texture.h"
#include "Render.h"

MeshShader::Shader::Shader() : vertex_shader(nullptr), pixel_shader(nullptr), layout(nullptr), buffer(nullptr)
{
}

MeshShader::Shader::~Shader()
{
	if(vertex_shader)
		vertex_shader->Release();
	if(pixel_shader)
		pixel_shader->Release();
	if(layout)
		layout->Release();
	if(buffer)
		buffer->Release();
}

MeshShader::MeshShader() : sampler(nullptr), current_shader(nullptr)
{
}

MeshShader::~MeshShader()
{
	if(sampler)
		sampler->Release();
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

	HRESULT result = render->GetDevice()->CreateSamplerState(&sampler_desc, &sampler);
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

	InitShader(mesh_shader, "mesh.hlsl", desc, countof(desc), sizeof(Buffer));
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

	InitShader(animated_shader, "animated.hlsl", desc, countof(desc), sizeof(AniBuffer));
	animated_shader.vertex_size = sizeof(AniVertex);
}

void MeshShader::InitShader(Shader& shader, cstring filename, D3D11_INPUT_ELEMENT_DESC* desc, uint desc_count, uint cbuffer_size)
{
	auto device = render->GetDevice();

	// create vertex shader
	CPtr<ID3DBlob> vs_buf = render->CompileShader(filename, "vs_main", true);
	HRESULT result = device->CreateVertexShader(vs_buf->GetBufferPointer(), vs_buf->GetBufferSize(), nullptr, &shader.vertex_shader);
	if(FAILED(result))
		throw Format("Failed to create vertex shader (%u).", result);

	// create pixel shader
	CPtr<ID3DBlob> ps_buf = render->CompileShader(filename, "ps_main", false);
	result = device->CreatePixelShader(ps_buf->GetBufferPointer(), ps_buf->GetBufferSize(), nullptr, &shader.pixel_shader);
	if(FAILED(result))
		throw Format("Failed to create pixel shader (%u).", result);

	// create layout
	result = device->CreateInputLayout(desc, desc_count, vs_buf->GetBufferPointer(), vs_buf->GetBufferSize(), &shader.layout);
	if(FAILED(result))
		throw Format("Failed to create input layout (%u).", result);

	// create cbuffer for shader
	D3D11_BUFFER_DESC cb_desc;
	cb_desc.Usage = D3D11_USAGE_DYNAMIC;
	cb_desc.ByteWidth = cbuffer_size;
	cb_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cb_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cb_desc.MiscFlags = 0;
	cb_desc.StructureByteStride = 0;

	result = device->CreateBuffer(&cb_desc, NULL, &shader.buffer);
	if(FAILED(result))
		throw Format("Failed to create cbuffer (%u).", result);
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
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->VSSetConstantBuffers(0, 1, &shader.buffer);
	context->VSSetShader(shader.vertex_shader, nullptr, 0);
	context->PSSetSamplers(0, 1, &sampler);
	context->PSSetShader(shader.pixel_shader, nullptr, 0);
}

void MeshShader::SetBuffer(const Matrix& matWorldViewProj, const vector<Matrix>* matBones)
{
	auto& shader = (matBones ? animated_shader : mesh_shader);
	auto context = render->GetContext();
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT result = context->Map(shader.buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
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
	context->Unmap(shader.buffer, 0);
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
