#pragma once

#include <GameHandler.h>

//enum GameState
//{
//	GS_MAIN_MENU,
//	GS_GAME
//};

struct Game : GameHandler
{
	Game();
	~Game();
	int Run();
	void OnInit() override;
	void LoadResources();
	void OnUpdate(float dt) override;
	void UpdateGame(float dt);
	void CleanWorld();

	Engine* engine;
	GameGui* game_gui;
	Camera* camera;
	Player* player;
	vector<GroundItem*> items;
	bool paused;
};
