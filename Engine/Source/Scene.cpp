#include "Pch.h"
#include "Scene.h"
#include "Render.h"
#include "Camera.h"
#include "SceneNode.h"
#include "MeshShader.h"
#include "MeshInstance.h"
#include "Window.h"

Scene::Scene(Render* render) : render(render), camera(new Camera), shader(new MeshShader)
{
	assert(render);
}

Scene::~Scene()
{
	delete shader;
	delete camera;
	DeleteElements(nodes);
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

	auto& wnd_size = render->GetWindow()->GetSize();

	Matrix matWorld,
		matProj = Matrix::CreatePerspectiveFieldOfView(PI / 4, float(wnd_size.x) / wnd_size.y, 0.1f, 100.f),
		matView = camera->GetViewMatrix(),
		matCombined;

	for(auto node : nodes)
	{
		matWorld = Matrix::RotationY(node->rot) * Matrix::Translation(node->pos);
		matCombined = matWorld * matView * matProj;

		if(node->inst)
		{
			node->inst->SetupBones();
			shader->SetParams(true);
			shader->SetBuffer(matCombined, &node->inst->GetMatrixBones());
		}
		else
		{
			shader->SetParams(false);
			shader->SetBuffer(matCombined, nullptr);
		}

		shader->Draw(node->mesh);
	}

	bool m_vsync_enabled = false;
	render->GetSwapChain()->Present(m_vsync_enabled ? 1 : 0, 0);
}

void Scene::Init()
{
	shader->Init(render);
}
