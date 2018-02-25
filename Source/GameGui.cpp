#include "Pch.h"
#include "GameGui.h"
#include "Game.h"
#include "Player.h"
#include <Gui.h>
#include "GroundItem.h"
#include "Item.h"
#include <Font.h>

GameGui::GameGui(Game* game) : game(game)
{
	assert(game);
}

void GameGui::Draw()
{
	GroundItem* item = game->player->item_before;
	if(item)
	{
		Rect rect(0, 0, 1024, 768);
		gui->DrawText(item->item->name, nullptr, Color::Black, Font::Top | Font::Center, rect);
	}
}
