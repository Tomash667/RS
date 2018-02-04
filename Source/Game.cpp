#include "Pch.h"
#include "Game.h"
#include "Item.h"
#include "Object.h"
#include "Unit.h"
#include <Engine.h>
#include <ResourceManager.h>
#include <SceneNode.h>
#include <Scene.h>
#include <Camera.h>
#include "InputManager.h"

Game::Game() : engine(nullptr)
{}

Game::~Game()
{
	delete engine;
}

int Game::Run()
{
	engine = new Engine(this);
	return engine->Run();
}

void Game::OnInit()
{
	//LoadResources();
	//state = GS_MAIN_MENU;

	ResourceManager* res_mgr = engine->GetResourceManager();
	Scene* scene = engine->GetScene();

	Mesh* mesh = res_mgr->GetMesh("human.qmsh");

	player = new SceneNode;
	player->SetMeshInstance(mesh);
	player->pos = Vec3(0, 0, 0);
	player->rot = 0;
	scene->Add(player);

	Camera* cam = scene->GetCamera();
	cam->from = Vec3(-5, 5, -5);
	cam->to = Vec3(0, 1, 0);
}

//void Game::LoadResources()
//{
//	ResourceManager& res_mgr = ResourceManager::Get();
//
//	for(Item& item : Item::items)
//	{
//		if(item.mesh_id)
//		{
//			item.mesh = res_mgr.GetMesh(item.mesh_id);
//			if(!item.mesh)
//				throw Format("Item %s is missing mesh '%s'.", item.id, item.mesh_id);
//		}
//		if(item.tex_id)
//		{
//			item.tex = res_mgr.GetTexture(item.tex_id);
//			if(!item.tex)
//				throw Format("Item %s is missing texture '%s'.", item.id, item.tex_id);
//		}
//	}
//
//	for(Object& obj : Object::objects)
//	{
//		if(obj.mesh_id)
//		{
//			obj.mesh = res_mgr.GetMesh(obj.mesh_id);
//			if(!obj.mesh)
//				throw Format("Object %s is missing mesh '%s'.", obj.id, obj.mesh_id);
//		}
//	}
//
//	for(Unit& unit : Unit::units)
//	{
//		if(unit.mesh_id)
//		{
//			unit.mesh = res_mgr.GetMesh(unit.mesh_id);
//			if(!unit.mesh)
//				throw Format("Unit %s is missing mesh '%s'.", unit.id, unit.mesh_id);
//		}
//	}
//}

void Game::OnUpdate(float dt)
{
	//if(state != GS_GAME)
	//	return;

	if(engine->GetInput()->Down(Key::W))
		player->pos.z += 5.f * dt;
}
