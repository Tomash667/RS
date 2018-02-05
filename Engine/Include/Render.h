#pragma once

struct Render
{
	Render(Window* window);
	~Render();
	void Init();
	Window* GetWindow() { return window; }
	ID3D11Device* GetDevice() { return device; }
	ID3D11DeviceContext* GetContext() { return context; }
	IDXGISwapChain* GetSwapChain() { return swap_chain; }
	ID3D11RenderTargetView* GetRenderTarget() { return render_target; }
	ID3D11DepthStencilView* GetDepthStencilView() { return depth_stencil_view; }
	ID3DBlob* CompileShader(cstring filename, cstring function, bool vertex);

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
};
