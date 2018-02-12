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

	player = new SceneNode("player");
	player->SetMeshInstance(mesh);
	player->pos = Vec3(0, 0, 0);
	player->rot = PI;
	scene->Add(player);

	moving = false;
	player->GetMeshInstance()->Play("stoi", 0, 0);

	camera = scene->GetCamera();
	camera->mode = Camera::THIRD_PERSON;
	camera->target = player;
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

	const Int2& wnd_size = engine->GetWindow()->GetSize();

	Gui* gui = engine->GetGui();
	Sprite* sprite = new Sprite;
	sprite->image = res_mgr->GetTexture("crosshair_dot.png");
	sprite->size = Int2(16, 16);
	sprite->pos = (wnd_size - sprite->size) / 2;
	gui->Add(sprite);

	ProgressBar* hp_bar = new ProgressBar;
	hp_bar->image_back = res_mgr->GetTexture("bar_empty.png");
	hp_bar->image_front = res_mgr->GetTexture("hp_bar.png");
	hp_bar->size = Int2(128, 8);
	hp_bar->pos = Int2(0, wnd_size.y - hp_bar->size.y);
	hp_bar->progress = 0.6f;
	gui->Add(hp_bar);
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


	// update player
	int dir = 0;
	if(input->Down(Key::W))
		dir += 10;
	if(input->Down(Key::S))
		dir -= 10;
	if(input->Down(Key::D))
		dir += 1;
	if(input->Down(Key::A))
		dir -= 1;
	if(dir != 0)
	{
		float speed, d;
		switch(dir)
		{
		case +1: // right
			d = 0;
			speed = 5.f;
			break;
		case -9: // right back
			d = PI / 4;
			speed = 2.5f;
			break;
		case -10: // back
			d = PI / 2;
			speed = 2.5f;
			break;
		case -11: // left back
			d = PI * 3 / 4;
			speed = 2.5f;
			break;
		case -1: // left
			d = PI;
			speed = 5.f;
			break;
		case +9: // left forward
			d = PI * 5 / 4;
			speed = 7.f;
			break;
		default:
		case +10: // forward
			d = PI * 3 / 2;
			speed = 7.f;
			break;
		case +11: // right forward
			d = PI * 7 / 4;
			speed = 7.f;
			break;
		}

		if(player->GetMeshInstance()->GetGroup(0).GetAnimation()->name == "biegnie")
			speed *= player->GetMeshInstance()->GetGroup(0).GetBlendT();
		else
			speed = 0.f;
		speed *= dt;

		d += player->rot - PI / 2;
		player->pos += Vec3(sin(d)*speed, 0, cos(d)*speed);

		if(!moving)
		{
			player->GetMeshInstance()->Play("biegnie", 0, 0);
			moving = true;
		}
	}
	else
	{
		if(moving)
		{
			player->GetMeshInstance()->Play("stoi", 0, 0);
			moving = false;
		}
	}

	auto& mouse_dif = input->GetMouseMove();
	player->rot = Clip(player->rot + float(mouse_dif.x) / 800);

	player->GetMeshInstance()->Update(dt);

	// update camera
	const Vec2 c_cam_angle = Vec2(PI + 0.1f, PI * 1.8f - 0.1f);
	camera->rot.x = player->rot;
	camera->rot.y = c_cam_angle.Clamp(camera->rot.y - float(mouse_dif.y) / 400);
	camera->Update(dt);

	// TODO: remove
	engine->GetWindow()->SetTitle(Format("X:%g; Z:%g; R:%g", player->pos.x, player->pos.z, Clip(player->rot + PI / 2)));
}
