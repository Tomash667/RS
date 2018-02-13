#include "Pch.h"
#include "Core.h"
#include "Render.h"
#include "Window.h"
#include "Shader.h"


Shader::Shader() : vertex_shader(nullptr), pixel_shader(nullptr), layout(nullptr), buffer(nullptr)
{
}

Shader::~Shader()
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


Render::Render(Window* window) : window(window), device(nullptr), context(nullptr), swap_chain(nullptr), render_target(nullptr), depth_stencil_view(nullptr),
raster_state(nullptr), clear_color(0.f,0.f,0.f,1.f), vsync(true), depth_state(nullptr), no_depth_state(nullptr)
{
	assert(window);
}

Render::~Render()
{
	SafeRelease(depth_state);
	SafeRelease(no_depth_state);
	SafeRelease(raster_state);
	SafeRelease(depth_stencil_view);
	SafeRelease(render_target);
	SafeRelease(swap_chain);
	SafeRelease(context);

	if(device)
	{
		/*#ifdef _DEBUG
		ID3D11Debug* debug;
		device->QueryInterface(__uuidof(ID3D11Debug), (void**)&debug);
		if(debug)
		{
		debug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
		debug->Release();
		}
		#endif*/

		device->Release();
	}
}

void Render::Init()
{
	CreateDeviceAndSwapChain();
	CreateRenderTargetView();
	CreateDepthStencilView();
	context->OMSetRenderTargets(1, &render_target, depth_stencil_view);
	SetViewport();
	CreateRasterState();
}

void Render::CreateDeviceAndSwapChain()
{
	HRESULT result;
	/*IDXGIFactory* factory;

	result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
	if(FAILED(result))
	throw Format("Failed to create IDXGIFactory (%u).", result);

	result = factory->EnumAdapters(0, &adapter);
	if(FAILED(result))
	throw Format("Failed to create IDXGIAdapter (%u).", result);*/

	auto& wnd_size = window->GetSize();

	DXGI_SWAP_CHAIN_DESC swap_desc = { 0 };
	swap_desc.BufferCount = 1;
	swap_desc.BufferDesc.Width = wnd_size.x;
	swap_desc.BufferDesc.Height = wnd_size.y;
	swap_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	/*if(m_vsync_enabled)
	{
	swap_desc.BufferDesc.RefreshRate.Numerator = numerator;
	swap_desc.BufferDesc.RefreshRate.Denominator = denominator;
	}
	else*/
	{
		swap_desc.BufferDesc.RefreshRate.Numerator = 0;
		swap_desc.BufferDesc.RefreshRate.Denominator = 1;
	}

	swap_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	// Set the handle for the window to render to.
	swap_desc.OutputWindow = (HWND)window->GetHandle();

	// Turn multisampling off.
	swap_desc.SampleDesc.Count = 1;
	swap_desc.SampleDesc.Quality = 0;

	swap_desc.Windowed = true;

	// Set the scan line ordering and scaling to unspecified.
	// ???
	swap_desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swap_desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	// Discard the back buffer contents after presenting.
	swap_desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	// Don't set the advanced flags.
	// ???
	swap_desc.Flags = 0;


	D3D_FEATURE_LEVEL feature_level = D3D_FEATURE_LEVEL_11_0;
	int flags = 0;
#ifdef _DEBUG
	flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	result = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, flags, &feature_level, 1,
		D3D11_SDK_VERSION, &swap_desc, &swap_chain, &device, nullptr, &context);
	if(FAILED(result))
		throw Format("Failed to create device and swap chain (%u).", result);
}

void Render::CreateRenderTargetView()
{
	HRESULT result;
	ID3D11Texture2D* back_buffer;
	result = swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&back_buffer);
	if(FAILED(result))
		throw Format("Failed to get back buffer (%u).", result);

	// Create the render target view with the back buffer pointer.
	result = device->CreateRenderTargetView(back_buffer, NULL, &render_target);
	if(FAILED(result))
		throw Format("Failed to create render target view (%u).", result);

	// Release pointer to the back buffer as we no longer need it.
	back_buffer->Release();
}

void Render::CreateDepthStencilView()
{
	auto& wnd_size = window->GetSize();

	// create depth buffer texture
	D3D11_TEXTURE2D_DESC depthBufferDesc = { 0 };
	// Set up the description of the depth buffer.
	depthBufferDesc.Width = wnd_size.x;
	depthBufferDesc.Height = wnd_size.y;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	// Create the texture for the depth buffer using the filled out description.
	ID3D11Texture2D* m_depthStencilBuffer;
	C(device->CreateTexture2D(&depthBufferDesc, nullptr, &m_depthStencilBuffer));

	//==================================================================
	// Depth stencil state
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc = { 0 };
	// Set up the description of the stencil state.
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;

	// Stencil operations if pixel is front-facing.
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Stencil operations if pixel is back-facing.
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Create the depth stencil state.
	C(device->CreateDepthStencilState(&depthStencilDesc, &depth_state));
	context->OMSetDepthStencilState(depth_state, 1);

	// create depth stencil state with disabled z test
	depthStencilDesc.DepthEnable = false;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;
	C(device->CreateDepthStencilState(&depthStencilDesc, &no_depth_state));

	//==================================================================
	// Initailze the depth stencil view.
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc{};

	// Set up the depth stencil view description.
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	// Create the depth stencil view.
	C(device->CreateDepthStencilView(m_depthStencilBuffer, &depthStencilViewDesc, &depth_stencil_view));

	m_depthStencilBuffer->Release();
}

ID3DBlob* Render::CompileShader(cstring filename, cstring function, bool vertex)
{
	assert(filename && function);

	uint flags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
	flags |= D3DCOMPILE_DEBUG;
#endif

	cstring path = Format("Shaders/%s", filename);
	ID3DBlob* shaderBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;
	wchar_t path_w[MAX_PATH];
	size_t len;
	mbstowcs_s(&len, path_w, path, MAX_PATH);
	HRESULT result = D3DCompileFromFile(path_w, nullptr, nullptr, function, vertex ? "vs_5_0" : "ps_5_0", flags, 0, &shaderBlob, &errorBlob);
	if(FAILED(result))
	{
		if(shaderBlob)
			shaderBlob->Release();
		if(errorBlob)
		{
			auto err = (cstring)errorBlob->GetBufferPointer();
			auto msg = Format("Failed to compile '%s' function '%s': %s (code %u).", path, function, err, result);
			errorBlob->Release();
			throw msg;
		}
		else
			throw Format("Failed to compile '%s' function '%s' (code %u).", path, function, result);
	}

	if(errorBlob)
	{
		auto err = (cstring)errorBlob->GetBufferPointer();
		printf("Shader '%s' warnings: %s", path, err);
		errorBlob->Release();
	}

	return shaderBlob;
}

void Render::CreateShader(Shader& shader, cstring filename, D3D11_INPUT_ELEMENT_DESC* desc, uint desc_count, uint cbuffer_size)
{
	// create vertex shader
	CPtr<ID3DBlob> vs_buf = CompileShader(filename, "vs_main", true);
	HRESULT result = device->CreateVertexShader(vs_buf->GetBufferPointer(), vs_buf->GetBufferSize(), nullptr, &shader.vertex_shader);
	if(FAILED(result))
		throw Format("Failed to create vertex shader (%u).", result);

	// create pixel shader
	CPtr<ID3DBlob> ps_buf = CompileShader(filename, "ps_main", false);
	result = device->CreatePixelShader(ps_buf->GetBufferPointer(), ps_buf->GetBufferSize(), nullptr, &shader.pixel_shader);
	if(FAILED(result))
		throw Format("Failed to create pixel shader (%u).", result);

	// create layout
	result = device->CreateInputLayout(desc, desc_count, vs_buf->GetBufferPointer(), vs_buf->GetBufferSize(), &shader.layout);
	if(FAILED(result))
		throw Format("Failed to create input layout (%u).", result);

	// create cbuffer for shader
	if(cbuffer_size > 0)
	{
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
}

void Render::SetViewport()
{
	auto& wnd_size = window->GetSize();

	D3D11_VIEWPORT viewport;
	viewport.Width = (float)wnd_size.x;
	viewport.Height = (float)wnd_size.y;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;

	context->RSSetViewports(1, &viewport);
}

void Render::CreateRasterState()
{
	D3D11_RASTERIZER_DESC rasterDesc;
	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_BACK;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	HRESULT result = device->CreateRasterizerState(&rasterDesc, &raster_state);
	if(FAILED(result))
		throw Format("Failed to create resterizer state (%u).", result);

	context->RSSetState(raster_state);
}

void Render::BeginScene()
{
	context->ClearRenderTargetView(render_target, clear_color);
	context->ClearDepthStencilView(depth_stencil_view, D3D11_CLEAR_DEPTH, 1.f, 0);
}

void Render::EndScene()
{
	C(swap_chain->Present(vsync ? 1 : 0, 0));
}

void Render::SetDepthTest(bool enabled)
{
	context->OMSetDepthStencilState(enabled ? depth_state : no_depth_state, 0);
}
