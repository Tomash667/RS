#pragma once

#include <GameHandler.h>

struct PlayerController;
struct AIController;
struct Unit;

enum GameState
{
	GS_MAIN_MENU,
	GS_GAME
};

struct Game : GameHandler
{
	Game();
	~Game();
	int Run();
	void OnInit() override;
	void OnUpdate(float dt) override;

	/*void LoadResources();
	void CleanWorld();
	void GenerateWorld();

	GameState state;
	PlayerController* pc;
	vector<AIController*> ais;
	vector<Unit*> units;*/

	Engine* engine;
	Camera* camera;
	SceneNode* player;
	float cam_rot;
};
