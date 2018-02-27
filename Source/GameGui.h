#pragma once

#include <GuiControls.h>

struct GameGui : Container
{
	GameGui(Game* game);
	void Init();
	void Draw() override;
	void Update(float dt) override;

private:
	Game* game;
	ProgressBar* hp_bar, *food_bar;
	Sprite* spr_current_weapon;
	Label* l_ammo, *l_fps, *l_paused;
	Panel* p_fps;
	Texture* t_no_weapon;
};
