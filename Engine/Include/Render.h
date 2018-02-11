#pragma once

struct Render
{
	Render(Window* window);
	~Render();
	void Init();
	ID3DBlob* CompileShader(cstring filename, cstring function, bool vertex);
	void CreateShader(Shader& shader, cstring filename, D3D11_INPUT_ELEMENT_DESC* desc, uint desc_count, uint cbuffer_size);
	void BeginScene();
	void EndScene();

	Window* GetWindow() { return window; }
	ID3D11Device* GetDevice() { return device; }
	ID3D11DeviceContext* GetContext() { return context; }
	IDXGISwapChain* GetSwapChain() { return swap_chain; }
	ID3D11RenderTargetView* GetRenderTarget() { return render_target; }
	ID3D11DepthStencilView* GetDepthStencilView() { return depth_stencil_view; }
	const Vec4& GetClearColor() { return clear_color; }

	void SetClearColor(const Vec4& clear_color) { this->clear_color = clear_color; }
	void SetDepthTest(bool enabled);

private:
	void CreateDeviceAndSwapChain();
	void CreateRenderTargetView();
	void CreateDepthStencilView();
	void SetViewport();
	void CreateRasterState();

	Window* window;
	IDXGISwapChain* swap_chain;
	ID3D11Device* device;
	ID3D11DeviceContext* context;
	ID3D11RenderTargetView* render_target;
	ID3D11DepthStencilView* depth_stencil_view;
	ID3D11RasterizerState* raster_state;
	ID3D11DepthStencilState* depth_state, *no_depth_state;
	Vec4 clear_color;
	bool vsync;
};
