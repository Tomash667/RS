#include "Pch.h"
#include "GameGui.h"
#include "Game.h"
#include "Player.h"
#include <Gui.h>
#include "GroundItem.h"
#include "Item.h"
#include <Font.h>
#include "SceneNode.h"


GameGui::GameGui(Game* game) : game(game)
{
	assert(game);
}

void GameGui::Draw()
{
	GroundItem* ground_item = game->player->item_before;
	if(ground_item)
	{
		Vec3 item_pos = ground_item->node->pos;
		item_pos.y += 0.5f;

		Int2 text_pos;
		if(!gui->To2dPoint(item_pos, text_pos))
			return;

		cstring name;
		if(ground_item->item->type == IT_WEAPON)
		{
			if(ground_item->count == 0)
				name = Format("%s (empty)", ground_item->item->name);
			else
				name = Format("%s (%d/%d)", ground_item->item->name, ground_item->count, ground_item->item->count);
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
		gui->DrawRect(rect, Color(0, 163, 33, 128));
		gui->DrawText(name, nullptr, Color::Black, Font::Top | Font::Center, rect);
	}
}
