#pragma once

#include <GuiControls.h>

struct Game;

struct GameGui : Control
{
	GameGui(Game* game);
	void Draw() override;

private:
	Game* game;
};
