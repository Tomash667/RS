#include "Pch.h"
#include "GuiShader.h"
#include "Render.h"
#include "Window.h"
#include "Texture.h"

GuiShader::GuiShader() : render(nullptr), sampler(nullptr), vb(nullptr)
{}

void GuiShader::Init(Render* render)
{
	assert(render);
	this->render = render;

	// create shader
	D3D11_INPUT_ELEMENT_DESC desc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	render->CreateShader(shader, "gui.hlsl", desc, countof(desc), sizeof(Vec4)); // cbuffer padding to Vec4
	shader.vertex_size = sizeof(Vertex);

	// create texture sampler
	D3D11_SAMPLER_DESC sampler_desc;
	sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
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
		throw Format("Failed to create gui sampler state (%u).", result);

	// create vertex buffer
	D3D11_BUFFER_DESC v_desc;
	v_desc.Usage = D3D11_USAGE_DYNAMIC;
	v_desc.ByteWidth = MaxVertex * sizeof(Vertex);
	v_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	v_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	v_desc.MiscFlags = 0;
	v_desc.StructureByteStride = 0;
	
	result = render->GetDevice()->CreateBuffer(&v_desc, nullptr, vb);
	if(FAILED(result))
		throw Format("Failed to create gui vertex buffer (%u).", result);
}

void GuiShader::SetParams()
{
	auto context = render->GetContext();
	context->IASetInputLayout(shader.layout);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->VSSetShader(shader.vertex_shader, nullptr, 0);
	context->PSSetSamplers(0, 1, &sampler.Get());
	context->PSSetShader(shader.pixel_shader, nullptr, 0);

	// set size
	Vec2 wnd_size = Vec2(render->GetWindow()->GetSize());
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT result = context->Map(shader.buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	memcpy(mappedResource.pData, &wnd_size, sizeof(wnd_size));
	context->Unmap(shader.buffer, 0);
	context->VSSetConstantBuffers(0, 1, &shader.buffer);
}

void GuiShader::Draw(Texture* tex, Vertex* v, uint count)
{
	assert(tex && v && count > 0 && count <= MaxVertex);
	auto context = render->GetContext();

	// copy vertex data
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT result = context->Map(vb, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	memcpy(mappedResource.pData, v, sizeof(Vertex) * count);
	context->Unmap(vb, 0);

	// set vertex buffer
	uint stride = sizeof(Vertex),
		offset = 0;
	context->IASetVertexBuffers(0, 1, &vb.Get(), &stride, &offset);

	context->PSSetShaderResources(0, 1, &tex->tex);
	context->Draw(count, 0);
}