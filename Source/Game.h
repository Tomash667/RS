#pragma once

#include <GameHandler.h>

struct GameGui;
struct GroundItem;
struct Player;

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
	void InitGui();
	void LoadResources();
	void OnUpdate(float dt) override;
	void UpdateGame(float dt);
	void CleanWorld();

	Engine* engine;
	GameGui* game_gui;
	Camera* camera;
	Player* player;
	vector<GroundItem*> items;

	//====================================
	// GAME GUI
	//====================================
	Label* label_paused;
	Label* label_fps;
	Panel* panel_fps;
};
