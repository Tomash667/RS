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
#include <MeshInstance.h>
#include <Render.h>
#include <Gui.h>
#include <Font.h>
#include "Player.h"
#include "GroundItem.h"
#include "GameGui.h"
#include "Level.h"
#include "MainMenu.h"


Game::Game() : engine(nullptr), player(nullptr), level(nullptr), main_menu(nullptr), game_gui(nullptr)
{
}

Game::~Game()
{
	delete player;
	delete level;
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
	LoadResources();
	//state = GS_MAIN_MENU;
	paused = false;

	ResourceManager* res_mgr = engine->GetResourceManager();
	Scene* scene = engine->GetScene();

	Mesh* mesh = res_mgr->GetMesh("human.qmsh");

	main_menu = new MainMenu;
	level = new Level(scene);

	player = new Player(engine->GetInput(), level);
	player->node = new SceneNode("player");
	player->node->SetMesh(mesh);
	player->node->GetMeshInstance()->Play("stoi", 0, 0);
	player->node->pos = Vec3(0, 0, 0);
	player->node->rot = PI;
	scene->Add(player->node);

	SceneNode* human2 = new SceneNode;
	human2->pos = Vec3(0, 0, -2);
	human2->SetMesh(res_mgr->GetMesh("human2.qmsh"));
	scene->Add(human2);

	SceneNode* gun = new SceneNode("gun");
	gun->SetMesh(res_mgr->GetMesh("m1911.qmsh"));
	//gun->SetMesh(res_mgr->GetMesh("marker.qmsh"));
	gun->pos = Vec3(0, 0, 0);
	gun->rot = 0;
	player->node->AddChild(gun, player->node->GetMesh()->GetPoint("bron"));

	camera = scene->GetCamera();
	camera->mode = Camera::THIRD_PERSON;
	camera->target = player->node;
	camera->dist = 2.f;
	camera->shift = 0.4f;
	camera->target_h = 1.5f;

	SceneNode* floor = new SceneNode("floor");
	floor->SetMesh(res_mgr->GetMesh("floor.qmsh"));
	floor->pos = Vec3(0, 0, 0);
	floor->rot = 0;
	scene->Add(floor);


	SceneNode* marker = new SceneNode("marker");
	marker->pos = Vec3(0, 0, 0);
	marker->rot = 0;
	marker->SetMesh(res_mgr->GetMesh("marker.qmsh"));
	scene->Add(marker);

	engine->GetRender()->SetClearColor(Vec4(0, 0.5f, 1, 1));

	game_gui = new GameGui(this);
	engine->GetGui()->Add(game_gui);
	game_gui->Init();

	GroundItem* ground_item = new GroundItem;
	ground_item->item = Item::Get("gun");
	ground_item->count = 1;
	ground_item->ammo_count = ground_item->item->count;
	ground_item->node = new SceneNode();
	ground_item->node->pos = Vec3(-2.f, 0, 0);
	ground_item->node->SetMesh(ground_item->item->mesh);
	level->AddItem(ground_item);

	ground_item = new GroundItem;
	ground_item->item = Item::Get("food");
	ground_item->count = 1;
	ground_item->ammo_count = 0;
	ground_item->node = new SceneNode();
	ground_item->node->pos = Vec3(-2.f, 0, 2);
	ground_item->node->SetMesh(ground_item->item->mesh);
	level->AddItem(ground_item);

	ground_item = new GroundItem;
	ground_item->item = Item::Get("ammo");
	ground_item->count = ground_item->item->count;
	ground_item->ammo_count = 0;
	ground_item->node = new SceneNode();
	ground_item->node->pos = Vec3(-2.f, 0, -2);
	ground_item->node->SetMesh(ground_item->item->mesh);
	level->AddItem(ground_item);
}

void Game::LoadResources()
{
	ResourceManager* res_mgr = engine->GetResourceManager();

	for(Item& item : Item::items)
	{
		if(item.mesh_id)
		{
			item.mesh = res_mgr->GetMesh(item.mesh_id);
			if(!item.mesh)
				throw Format("Item %s is missing mesh '%s'.", item.id, item.mesh_id);
		}
		if(item.tex_id)
		{
			item.tex = res_mgr->GetTexture(item.tex_id);
			if(!item.tex)
				throw Format("Item %s is missing texture '%s'.", item.id, item.tex_id);
		}
	}

	/*for(Object& obj : Object::objects)
	{
		if(obj.mesh_id)
		{
			obj.mesh = res_mgr->GetMesh(obj.mesh_id);
			if(!obj.mesh)
				throw Format("Object %s is missing mesh '%s'.", obj.id, obj.mesh_id);
		}
	}

	for(Unit& unit : Unit::units)
	{
		if(unit.mesh_id)
		{
			unit.mesh = res_mgr->GetMesh(unit.mesh_id);
			if(!unit.mesh)
				throw Format("Unit %s is missing mesh '%s'.", unit.id, unit.mesh_id);
		}
	}*/
}

void Game::OnUpdate(float dt)
{
	InputManager* input = engine->GetInput();
	if(input->Down(Key::Alt) && input->Pressed(Key::F4))
	{
		engine->Shutdown();
		return;
	}

	if(input->Down(Key::Alt) && input->Pressed(Key::U))
		engine->GetWindow()->UnlockCursor(true);

	if(paused)
	{
		if(input->Pressed(Key::Escape))
			paused = false;
		else if(input->Pressed(Key::Enter))
		{
			engine->Shutdown();
			return;
		}
	}
	else
	{
		if(input->Pressed(Key::Escape))
			paused = true;
	}

	if(!paused)
		UpdateGame(dt);
}

void Game::UpdateGame(float dt)
{
	InputManager* input = engine->GetInput();
	auto& mouse_dif = input->GetMouseMove();

	player->Update(dt);

	// update camera
	const Vec2 c_cam_angle = Vec2(PI + 0.1f, PI * 1.8f - 0.1f);
	camera->rot.x = player->node->rot;
	camera->rot.y = c_cam_angle.Clamp(camera->rot.y - float(mouse_dif.y) / 400);
	camera->Update(dt);
}
