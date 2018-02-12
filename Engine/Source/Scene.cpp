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
	mat_view_proj = camera->GetViewMatrix() * Matrix::CreatePerspectiveFieldOfView(PI / 4, float(wnd_size.x) / wnd_size.y, 0.1f, 100.f),

	shader->ResetParams();

	DrawNodes(nodes);
}

void Scene::DrawNodes(vector<SceneNode*>& nodes)
{
	for(auto node : nodes)
	{
		if(!node->visible)
			continue;

		mat_world = Matrix::RotationY(node->rot) * Matrix::Translation(node->pos);
		mat_combined = mat_world * mat_view_proj;

		if(node->GetMeshInstance())
		{
			node->GetMeshInstance()->SetupBones();
			shader->SetParams(true);
			shader->SetBuffer(mat_combined, &node->GetMeshInstance()->GetMatrixBones());
		}
		else
		{
			shader->SetParams(false);
			shader->SetBuffer(mat_combined, nullptr);
		}

		shader->Draw(node->GetMesh());

		vector<SceneNode*>& childs = node->GetChildNodes();
		if(!childs.empty())
			DrawNodes(childs);
	}
}

void Scene::Init()
{
	shader->Init(render);
}
