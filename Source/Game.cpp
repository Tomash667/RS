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


Game::Game() : engine(nullptr), player(nullptr), game_gui(nullptr)
{
}

Game::~Game()
{
	CleanWorld();
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

	ResourceManager* res_mgr = engine->GetResourceManager();
	Scene* scene = engine->GetScene();

	Mesh* mesh = res_mgr->GetMesh("human.qmsh");

	player = new Player;
	player->node = new SceneNode("player");
	player->node->SetMesh(mesh);
	player->node->GetMeshInstance()->Play("stoi", 0, 0);
	player->node->pos = Vec3(0, 0, 0);
	player->node->rot = PI;
	scene->Add(player->node);

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

	InitGui();

	GroundItem* ground_item = new GroundItem;
	ground_item->item = Item::Get("gun");
	ground_item->count = ground_item->item->count;
	ground_item->node = new SceneNode();
	ground_item->node->pos = Vec3(-2.f, 0, 0);
	ground_item->node->SetMesh(ground_item->item->mesh);
	items.push_back(ground_item);
	scene->Add(ground_item->node);

	ground_item = new GroundItem;
	ground_item->item = Item::Get("food");
	ground_item->count = ground_item->item->count;
	ground_item->node = new SceneNode();
	ground_item->node->pos = Vec3(-2.f, 0, 2);
	ground_item->node->SetMesh(ground_item->item->mesh);
	items.push_back(ground_item);
	scene->Add(ground_item->node);

	ground_item = new GroundItem;
	ground_item->item = Item::Get("ammo");
	ground_item->count = ground_item->item->count;
	ground_item->node = new SceneNode();
	ground_item->node->pos = Vec3(-2.f, 0, -2);
	ground_item->node->SetMesh(ground_item->item->mesh);
	items.push_back(ground_item);
	scene->Add(ground_item->node);
}

void Game::InitGui()
{
	Gui* gui = engine->GetGui();
	ResourceManager* res_mgr = engine->GetResourceManager();
	const Int2& wnd_size = engine->GetWindow()->GetSize();

	game_gui = new GameGui(this);
	gui->Add(game_gui);

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

	label_paused = new Label;
	label_paused->size = wnd_size;
	label_paused->flags = Font::Center | Font::VCenter;
	label_paused->text = "GAME PAUSED\nEsc - continue, Enter - exit to menu";
	label_paused->visible = false;
	label_paused->font = gui->CreateFont("Arial", 32, 5);
	gui->Add(label_paused);

	label_fps = new Label;
	label_fps->pos = Int2(6, 6);
	label_fps->color = Color::Black;

	panel_fps = new Panel;
	panel_fps->Add(label_fps);
	panel_fps->image = res_mgr->GetTexture("panel.png");
	panel_fps->image_size = 32;
	panel_fps->corner_size = 12;
	panel_fps->size = Int2(32, 32);
	panel_fps->Setup();
	gui->Add(panel_fps);
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

	if(label_paused->visible)
	{
		if(input->Pressed(Key::Escape))
			label_paused->visible = false;
		else if(input->Pressed(Key::Enter))
		{
			engine->Shutdown();
			return;
		}
	}
	else
	{
		if(input->Pressed(Key::Escape))
			label_paused->visible = true;
	}

	if(!label_paused->visible)
		UpdateGame(dt);

	// update gui
	if(input->Pressed(Key::F1))
		panel_fps->visible = !panel_fps->visible;
	if(panel_fps->visible)
	{
		label_fps->text = Format("Fps: %g\nPos: %g, %g\nRot: %g", FLT10(engine->GetFps()), FLT10(player->node->pos.x), FLT10(player->node->pos.z),
			FLT100(Clip(player->node->rot + PI / 2)));
		label_fps->CalculateSize();
		Int2 new_size = Int2::Max(label_fps->size + Int2(12, 12), panel_fps->size);
		if(new_size != panel_fps->size)
		{
			panel_fps->size = new_size;
			panel_fps->Setup();
		}
	}
}

void Game::UpdateGame(float dt)
{
	InputManager* input = engine->GetInput();
	auto& mouse_dif = input->GetMouseMove();

	// items in front of player
	if(player->action == Player::Action::NONE)
	{
		const float pick_range = 2.f;
		float best_range = pick_range;
		GroundItem* best_item = nullptr;
		for(GroundItem* item : items)
		{
			float dist = Vec3::Distance2d(player->node->pos, item->node->pos);
			if(dist < best_range)
			{
				float angle = AngleDiff(Clip(player->node->rot + PI / 2), Clip(-Vec3::Angle2d(player->node->pos, item->node->pos)));
				if(angle < PI / 4)
				{
					best_item = item;
					best_range = dist;
				}
			}
		}
		if(player->item_before && player->item_before != best_item)
		{
			player->item_before->node->tint = Vec3::One;
			player->item_before = nullptr;
		}
		if(best_item && best_item != player->item_before)
		{
			player->item_before = best_item;
			best_item->node->tint = Vec3(2, 2, 2);
		}

		if(player->item_before && input->Pressed(Key::E))
		{
			player->action = Player::Action::PICKUP;
			player->action_state = 0;
			player->new_anim = Player::Animation::ACTION;
			player->node->GetMeshInstance()->Play("podnosi", 0, 0);
		}
	}
	else if(player->action == Player::Action::PICKUP)
	{
		MeshInstance* inst = player->node->GetMeshInstance();
		if(player->action_state == 0)
		{
			float speed = 3.f * dt;
			float expected_rot = Clip(-Vec3::Angle2d(player->node->pos, player->item_before->node->pos) - PI/2);
			float dif = AngleDiff(player->node->rot, expected_rot);
			if(dif < speed)
			{
				player->node->rot = expected_rot;
				player->action_state = 1;
			}
			else
			{
				float arc = ShortestArc(player->node->rot, expected_rot);
				player->node->rot = Clip(player->node->rot + Sign(arc) * speed);
			}
		}

		if(player->action_state != 2 && inst->GetProgress(0) >= 19.f / 34)
		{
			engine->GetScene()->Remove(player->item_before->node);
			DeleteElement(items, player->item_before);
			player->item_before = nullptr;
			player->action_state = 2;
		}

		if(inst->GetEndResultClear(0))
		{
			player->new_anim = Player::Animation::STAND;
			player->action = Player::Action::NONE;
		}
	}
	else
	{
		if(player->item_before)
		{
			player->item_before->node->tint = Vec3::One;
			player->item_before = nullptr;
		}
	}

	if(player->action == Player::Action::NONE)
	{
		// move player
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
				player->new_anim = Player::Animation::RUN_RIGHT;
				break;
			case -9: // right back
				d = PI / 4;
				speed = 2.5f;
				player->new_anim = Player::Animation::WALK_BACK;
				break;
			case -10: // back
				d = PI / 2;
				speed = 2.5f;
				player->new_anim = Player::Animation::WALK_BACK;
				break;
			case -11: // left back
				d = PI * 3 / 4;
				speed = 2.5f;
				player->new_anim = Player::Animation::WALK_BACK;
				break;
			case -1: // left
				d = PI;
				speed = 5.f;
				player->new_anim = Player::Animation::RUN_LEFT;
				break;
			case +9: // left forward
				d = PI * 5 / 4;
				speed = 7.f;
				player->new_anim = Player::Animation::RUN;
				break;
			default:
			case +10: // forward
				d = PI * 3 / 2;
				speed = 7.f;
				player->new_anim = Player::Animation::RUN;
				break;
			case +11: // right forward
				d = PI * 7 / 4;
				speed = 7.f;
				player->new_anim = Player::Animation::RUN;
				break;
			}

			if(player->anim == player->new_anim)
				speed *= player->node->GetMeshInstance()->GetGroup(0).GetBlendT();
			else
				speed = 0.f;
			speed *= dt;

			d += player->node->rot - PI / 2;
			player->node->pos += Vec3(sin(d)*speed, 0, cos(d)*speed);
		}
		else
			player->new_anim = Player::Animation::STAND;

		// rotate player
		player->node->rot = Clip(player->node->rot + float(mouse_dif.x) / 800);
		if(player->new_anim == Player::Animation::STAND && mouse_dif.x != 0)
		{
			if(mouse_dif.x > 0)
				player->new_anim = Player::Animation::ROTATE_RIGHT;
			else
				player->new_anim = Player::Animation::ROTATE_LEFT;
		}
	}

	// update animation
	MeshInstance* inst = player->node->GetMeshInstance();
	if(player->anim != player->new_anim)
	{
		player->anim = player->new_anim;
		switch(player->anim)
		{
		case Player::Animation::STAND:
			inst->Play("stoi", 0, 0);
			break;
		case Player::Animation::RUN:
		case Player::Animation::RUN_LEFT:
		case Player::Animation::RUN_RIGHT:
			inst->Play("biegnie", 0, 0);
			break;
		case Player::Animation::WALK_BACK:
			inst->Play("idzie", PLAY_BACK, 0);
			break;
		case Player::Animation::ROTATE_LEFT:
			inst->Play("w_lewo", 0, 0);
			break;
		case Player::Animation::ROTATE_RIGHT:
			inst->Play("w_prawo", 0, 0);
			break;
		}
	}
	player->node->GetMeshInstance()->Update(dt);

	// update camera
	const Vec2 c_cam_angle = Vec2(PI + 0.1f, PI * 1.8f - 0.1f);
	camera->rot.x = player->node->rot;
	camera->rot.y = c_cam_angle.Clamp(camera->rot.y - float(mouse_dif.y) / 400);
	camera->Update(dt);
}

void Game::CleanWorld()
{
	DeleteElements(items);
	delete player;
}
