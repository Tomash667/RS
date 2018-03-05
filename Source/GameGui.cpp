#include "Pch.h"
#include "GameGui.h"
#include "Game.h"
#include "Player.h"
#include <Gui.h>
#include "GroundItem.h"
#include "Item.h"
#include <Font.h>
#include "SceneNode.h"
#include <Engine.h>
#include <ResourceManager.h>
#include <InputManager.h>


GameGui::GameGui(Game* game) : game(game)
{
	assert(game);
	progrez = 0.f;
}

void GameGui::Init()
{
	ResourceManager* res_mgr = game->engine->GetResourceManager();
	const Int2& wnd_size = gui->GetWindowSize();

	Panel* def_panel = new Panel;
	def_panel->image = res_mgr->GetTexture("panel.png");
	def_panel->image_size = 32;
	def_panel->corner_size = 12;
	gui->AddTemplate(typeid(Panel), def_panel);

	Button* def_button = new Button;
	def_button->image[0] = res_mgr->GetTexture("panel.png");
	def_button->image[1] = res_mgr->GetTexture("panel_hover.png");
	def_button->image[2] = res_mgr->GetTexture("panel_down.png");
	def_button->image_size = 32;
	def_button->corner_size = 12;
	def_button->font_color = Color::White;
	gui->AddTemplate(typeid(Button), def_button);
	
	t_no_weapon = res_mgr->GetTexture("no_weapon.png");

	Sprite* sprite = new Sprite;
	sprite->image = res_mgr->GetTexture("gui.png");
	sprite->pos = Int2(0, wnd_size.y - 87);
	sprite->size = Int2(512, 128);
	sprite->color = Color(255, 255, 255, 192);
	Add(sprite);

	sprite = new Sprite;
	sprite->image = res_mgr->GetTexture("crosshair_dot.png");
	sprite->size = Int2(16, 16);
	sprite->pos = (wnd_size - sprite->size) / 2;
	Add(sprite);

	hp_bar = new ProgressBar;
	hp_bar->image_back = res_mgr->GetTexture("bar_empty.png");
	hp_bar->image_front = res_mgr->GetTexture("hp_bar.png");
	hp_bar->size = Int2(128, 8);
	hp_bar->pos = Int2(0, wnd_size.y - hp_bar->size.y * 2);
	Add(hp_bar);

	food_bar = new ProgressBar;
	food_bar->image_back = res_mgr->GetTexture("bar_empty.png");
	food_bar->image_front = res_mgr->GetTexture("hp_bar.png");
	food_bar->size = Int2(128, 8);
	food_bar->pos = Int2(0, wnd_size.y - food_bar->size.y);
	Add(food_bar);

	spr_current_weapon = new Sprite;
	spr_current_weapon->image = t_no_weapon;
	spr_current_weapon->pos = Int2(130, wnd_size.y - 34);
	spr_current_weapon->size = Int2(32, 32);
	Add(spr_current_weapon);

	l_ammo = new Label;
	l_ammo->visible = false;
	l_ammo->pos = Int2(164, wnd_size.y - 28);
	l_ammo->size = Int2(200, 28);
	Add(l_ammo);

	l_paused = new Label;
	l_paused->size = wnd_size;
	l_paused->flags = Font::Center | Font::VCenter;
	l_paused->text = "GAME PAUSED\nEsc - continue, Enter - exit to menu";
	l_paused->visible = false;
	l_paused->font = gui->CreateFont("Arial", 32, 5);
	Add(l_paused);

	l_fps = new Label;
	l_fps->pos = Int2(6, 6);
	l_fps->color = Color::White;

	p_fps = new Panel;
	p_fps->Add(l_fps);
	p_fps->size = Int2(32, 32);
	Add(p_fps);


	circle = res_mgr->GetTexture("progress_circle.png");
}

void GameGui::Draw()
{
	Container::Draw();

	GroundItem* ground_item = game->player->item_before;
	if(ground_item)
	{
		Vec3 item_pos = ground_item->node->pos;
		item_pos.y += 0.5f;

		Int2 text_pos;
		if(!gui->To2dPoint(item_pos, text_pos))
			return;

		cstring name;
		if(ground_item->item->type == Item::Type::WEAPON)
		{
			if(ground_item->ammo_count == 0)
				name = Format("%s (empty)", ground_item->item->name);
			else
				name = Format("%s (%d/%d)", ground_item->item->name, ground_item->ammo_count, ground_item->item->count);
		}
		else
		{
			if(ground_item->count == 1)
				name = ground_item->item->name;
			else
				name = Format("%s (%d)", ground_item->item->name, ground_item->count);
		}

		const Int2& wnd_size = gui->GetWindowSize();
		Int2 text_size = gui->GetDefaultFont()->CalculateSize(name) + Int2(2, 2);
		text_pos -= text_size / 2;
		if(text_pos.x < 0)
			text_pos.x = 0;
		else if(text_pos.x + text_size.x >= wnd_size.x)
			text_pos.x = wnd_size.x - text_size.x;
		if(text_pos.y < 0)
			text_pos.y = 0;
		else if(text_pos.y + text_size.y >= wnd_size.y)
			text_pos.y = wnd_size.y - text_size.y;

		Rect rect = Rect::Create(text_pos, text_size);
		gui->DrawRect(nullptr, rect, Color(0, 163, 33, 128));
		gui->DrawText(name, nullptr, Color::Black, Font::Top | Font::Center, rect);
	}

	// 7 55 26 26
	const Int2& wnd_size = gui->GetWindowSize();
	const Color yellow(255, 255, 0); // todo
	const Color red(255, 0, 0); // todo
	Color color;
	if(progrez >= 0.75f)
		color = Color::White;
	else if(progrez >= 0.325f)
		//color = yellow;
		color = Color::Lerp(yellow, Color::White, (progrez - 0.325f) / 0.325f);
	else
		//color = red;
		color = Color::Lerp(red, yellow, progrez / 0.325f); // TODO
	gui->DrawSpriteCircle(circle, -PI / 2, -1.f, progrez, Vec2(7 + 13, wnd_size.y - 87 + 55 + 13), 13.f, color);

	// circles
	// + food (white->yellow->red)
	// + energy --||--
	// + sanity
	// + virus
	// + air
	// + radiation
}

void GameGui::Update(float dt)
{
	Container::Update(dt);

	InputManager* input = game->engine->GetInput();
	Player* player = game->player;

	// alt f4
	if(input->Down(Key::Alt) && input->Pressed(Key::F4))
		HandleExit();

	// fps panel
	if(input->Pressed(Key::F1))
		p_fps->visible = !p_fps->visible;
	if(p_fps->visible)
	{
		l_fps->text = Format("Fps: %g\nPos: %g, %g\nRot: %g", FLT10(game->engine->GetFps()), FLT10(player->node->pos.x), FLT10(player->node->pos.z),
			FLT100(Clip(player->node->rot + PI / 2)));
		l_fps->CalculateSize();
		Int2 new_size = Int2::Max(l_fps->size + Int2(12, 12), p_fps->size);
		if(new_size != p_fps->size)
			p_fps->size = new_size;
	}

	// paused label
	l_paused->visible = game->paused;

	// player gui
	hp_bar->progress = player->GetHpP();
	food_bar->progress = player->GetFoodP();
	if(player->HaveWeapon())
	{
		ItemSlot& weapon = player->GetWeapon();
		spr_current_weapon->image = weapon.item->tex;
		l_ammo->text = Format("%d | %d", weapon.count, player->GetAmmoLeft(weapon.item));
		l_ammo->visible = true;
	}
	else
	{
		spr_current_weapon->image = t_no_weapon;
		l_ammo->visible = false;
	}

	progrez = Clip(progrez + dt / 2, 1.25f);
}

void GameGui::HandleExit()
{
	DialogBox* dialog = new DialogBox;
	dialog->text = "Are you sure you want to quit?";
	dialog->event = [&](int id)
	{
		if(id == 0)
			game->engine->Shutdown();
	};
	dialog->AddButton("Exit", 0);
	dialog->AddButton("Cancel", 1);
	dialog->Show(gui);
}
