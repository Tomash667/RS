#include "Pch.h"
#include "Scene.h"
#include "Render.h"
#include "Camera.h"

Scene::Scene(Render* render) : render(render), camera(new Camera)
{
	assert(render);
}

Scene::~Scene()
{
	delete camera;
}

void Scene::Add(SceneNode* node)
{
	assert(node);
	nodes.push_back(node);
}

void Scene::Draw()
{
	auto context = render->GetContext();
	context->ClearRenderTargetView(render->GetRenderTarget(), Vec4(0, 0.5f, 1.f, 1.f));
	context->ClearDepthStencilView(render->GetDepthStencilView(), D3D11_CLEAR_DEPTH, 1.f, 0);

	bool m_vsync_enabled = false;
	render->GetSwapChain()->Present(m_vsync_enabled ? 1 : 0, 0);
}
