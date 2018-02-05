#include "Pch.h"
#include "Scene.h"
#include "Camera.h"

Scene::Scene() : camera(new Camera)
{}

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

}
