#include "Pch.h"
#include "GuiShader.h"
#include "Render.h"
#include "Window.h"
#include "Texture.h"

GuiShader::GuiShader() : render(nullptr), locked_data(nullptr), empty_texture(nullptr)
{
}

GuiShader::~GuiShader()
{
	delete empty_texture;
}

void GuiShader::Init(Render* render)
{
	assert(render);
	this->render = render;

	// create shader
	D3D11_INPUT_ELEMENT_DESC desc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	uint cbuffer_size[] = { sizeof(Buffer), 0 };
	render->CreateShader(shader, "gui.hlsl", desc, countof(desc), cbuffer_size);
	shader.vertex_size = sizeof(VertexPosTexColor);

	// create texture sampler
	D3D11_SAMPLER_DESC sampler_desc;
	sampler_desc.Filter = D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
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
	v_desc.ByteWidth = MaxVertex * sizeof(VertexPosTexColor);
	v_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	v_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	v_desc.MiscFlags = 0;
	v_desc.StructureByteStride = 0;

	result = render->GetDevice()->CreateBuffer(&v_desc, nullptr, vb);
	if(FAILED(result))
		throw Format("Failed to create gui vertex buffer (%u).", result);

	// create blend state
	D3D11_BLEND_DESC b_desc = { 0 };
	b_desc.RenderTarget[0].BlendEnable = true;
	b_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	b_desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	b_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	b_desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	b_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	b_desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	b_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	result = render->GetDevice()->CreateBlendState(&b_desc, blend_state);
	if(FAILED(result))
		throw Format("Failed to create gui blend state (%u).", result);

	render->GetContext()->OMGetBlendState(no_blend_state, nullptr, nullptr);

	// create empty texture
	D3D11_TEXTURE2D_DESC t_desc = { 0 };
	t_desc.Width = 1;
	t_desc.Height = 1;
	t_desc.MipLevels = 1;
	t_desc.ArraySize = 1;
	t_desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	t_desc.SampleDesc.Count = 1;
	t_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	t_desc.Usage = D3D11_USAGE_DEFAULT;

	Vec4 color = Color::White;
	D3D11_SUBRESOURCE_DATA t_sub_data = { 0 };
	t_sub_data.pSysMem = &color;
	t_sub_data.SysMemPitch = sizeof(Vec4);

	ID3D11Texture2D* tex;
	C(render->GetDevice()->CreateTexture2D(&t_desc, &t_sub_data, &tex));

	ID3D11ShaderResourceView* tex_view;
	D3D11_SHADER_RESOURCE_VIEW_DESC tv_desc = { 0 };
	tv_desc.Format = t_desc.Format;
	tv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	tv_desc.Texture2D.MipLevels = 1;

	C(render->GetDevice()->CreateShaderResourceView(tex, &tv_desc, &tex_view));
	
	tex->Release();

	empty_texture = new Texture;
	empty_texture->name = "EmptyTexture";
	empty_texture->tex = tex_view;
	empty_texture->type = Resource::Texture;
}

void GuiShader::SetParams()
{
	auto context = render->GetContext();
	context->IASetInputLayout(shader.layout);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->VSSetShader(shader.vertex_shader, nullptr, 0);
	context->PSSetSamplers(0, 1, &sampler.Get());
	context->PSSetShader(shader.pixel_shader, nullptr, 0);
	context->OMSetBlendState(blend_state, nullptr, 0xFFFFFFFF);
	context->VSSetConstantBuffers(0, 1, &shader.vs_buffer);
	render->SetDepthTest(false);
	Vec2 wnd_size = Vec2(render->GetWindow()->GetSize());
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	C(context->Map(shader.vs_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource));
	Buffer* buffer = (Buffer*)mappedResource.pData;
	buffer->size = wnd_size;
	context->Unmap(shader.vs_buffer, 0);
}

void GuiShader::RestoreParams()
{
	render->GetContext()->OMSetBlendState(no_blend_state, nullptr, 0xFFFFFFFF);
	render->SetDepthTest(true);
}

VertexPosTexColor* GuiShader::Lock()
{
	assert(!locked_data);
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	C(render->GetContext()->Map(vb, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource));
	locked_data = (VertexPosTexColor*)mappedResource.pData;
	return locked_data;
}

void GuiShader::Draw(Texture* tex, uint count)
{
	assert(tex && locked_data && count <= MaxVertex);

	auto context = render->GetContext();
	context->Unmap(vb, 0);
	locked_data = nullptr;

	if(count == 0)
		return;

	uint stride = sizeof(VertexPosTexColor),
		offset = 0;
	context->IASetVertexBuffers(0, 1, &vb.Get(), &stride, &offset);

	context->PSSetShaderResources(0, 1, &tex->tex);
	context->Draw(count, 0);
}
