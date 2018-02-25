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

	DrawNodes(nodes, nullptr, nullptr);
}

void Scene::DrawNodes(vector<SceneNode*>& nodes, SceneNode* parent, Matrix* parent_matrix)
{
	Matrix mat_world;

	for(auto node : nodes)
	{
		if(!node->visible)
			continue;

		mat_world = Matrix::RotationY(node->rot) * Matrix::Translation(node->pos);
		if(parent)
		{
			if(node->attach_point)
				//mat_world = mat_world * node->attach_point->mat * parent->inst->GetMatrixBones()[node->attach_point->bone];
				mat_world = node->attach_point->mat * parent->inst->GetMatrixBones()[node->attach_point->bone];
			mat_world *= *parent_matrix;
		}
		mat_combined = mat_world * mat_view_proj;

		if(node->inst)
		{
			node->GetMeshInstance()->SetupBones();
			shader->SetParams(true);
			shader->SetBuffer(node->tint, mat_combined, &node->GetMeshInstance()->GetMatrixBones());
		}
		else
		{
			shader->SetParams(false);
			shader->SetBuffer(node->tint, mat_combined, nullptr);
		}

		shader->Draw(node->GetMesh());

		vector<SceneNode*>& childs = node->GetChildNodes();
		if(!childs.empty())
			DrawNodes(childs, node, &mat_world);
	}
}

void Scene::Init()
{
	shader->Init(render);
}

void Scene::Remove(SceneNode* node)
{
	assert(node);
	DeleteElement(nodes, node);
}
