#pragma once

// external
struct Camera;
struct Engine;
struct Font;
struct GameHandler;
struct Gui;
struct InputManager;
struct Render;
struct Resource;
struct ResourceManager;
struct Mesh;
struct Scene;
struct SceneNode;
struct Sound;
struct SoundManager;
struct Texture;
struct Window;
struct WindowHandler;

// gui controls
struct Button;
struct Container;
struct Control;
struct Label;
struct Panel;
struct ProgressBar;
struct Sprite;

// internal
struct FontLoader;
struct GuiShader;
struct MeshShader;
struct QmshLoader;
struct Shader;

// directx
struct IDXGISwapChain;
struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3D11RenderTargetView;
struct ID3D11DepthStencilView;
struct ID3D10Blob;
typedef ID3D10Blob ID3DBlob;
struct ID3D11RasterizerState;
struct ID3D11ShaderResourceView;
struct ID3D11Buffer;
struct D3D11_INPUT_ELEMENT_DESC;
struct ID3D11DepthStencilState;

// fmod
namespace FMOD
{
	class Sound;
	class System;
}
