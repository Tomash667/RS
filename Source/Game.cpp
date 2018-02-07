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
#include <Window.h>
#include <InputManager.h>
#include <ctime>


Game::Game() : engine(nullptr)
{}

Game::~Game()
{
	delete engine;
}

int Game::Run()
{
	engine = new Engine(this);

	time_t t = time(0);
	tm tm;
	localtime_s(&tm, &t);
	Info("RS v0");
	Info("Date: %04d-%02d-%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);

	StartupOptions options;
	options.title = "RS v0";
	return engine->Run(&options);
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

	camera = scene->GetCamera();
	camera->from = Vec3(-5, 5, -5);
	camera->to = Vec3(0, 1, 0);

	SceneNode* floor = new SceneNode;
	floor->mesh = res_mgr->GetMesh("floor.qmsh");
	floor->pos = Vec3(0, 0, 0);
	floor->rot = 0;
	scene->Add(floor);

	cam_rot = 0;
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


	InputManager* input = engine->GetInput();
	if(input->Down(Key::Alt) && input->Pressed(Key::F4))
	{
		engine->Shutdown();
		return;
	}

	if(input->Down(Key::Alt) && input->Pressed(Key::U))
		engine->GetWindow()->UnlockCursor(true);

	if(input->Down(Key::W))
		player->pos.z += 5.f * dt;

	// update camera
	auto& mouse_dif = input->GetMouseMove();
	player->rot += mouse_dif.x;

	Vec3 target = player->pos;
	target.y += 1.5f;

	camera->to = target;
	camera->from = target;
	camera->from += Vec3(cos(player->rot + PI/2) * 2, 0.25f, sin(player->rot+PI/2) * 2);


}
