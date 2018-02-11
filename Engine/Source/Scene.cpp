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
	auto& wnd_size = render->GetWindow()->GetSize();

	Matrix matWorld,
		matProj = Matrix::CreatePerspectiveFieldOfView(PI / 4, float(wnd_size.x) / wnd_size.y, 0.1f, 100.f),
		matView = camera->GetViewMatrix(),
		matCombined;

	shader->ResetParams();

	for(auto node : nodes)
	{
		if(!node->visible)
			continue;

		matWorld = Matrix::RotationY(node->rot) * Matrix::Translation(node->pos);
		matCombined = matWorld * matView * matProj;

		if(node->GetMeshInstance())
		{
			node->GetMeshInstance()->SetupBones();
			shader->SetParams(true);
			shader->SetBuffer(matCombined, &node->GetMeshInstance()->GetMatrixBones());
		}
		else
		{
			shader->SetParams(false);
			shader->SetBuffer(matCombined, nullptr);
		}

		shader->Draw(node->GetMesh());
	}
}

void Scene::Init()
{
	shader->Init(render);
}
