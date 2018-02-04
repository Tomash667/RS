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
	// TODO
	assert(node);
}
