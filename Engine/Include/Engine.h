#pragma once

#include "WindowHandler.h"

struct Engine : WindowHandler
{
	Engine(GameHandler* handler);
	~Engine();
	int Run();
	void Shutdown();
	void ShowError(cstring msg);

	InputManager* GetInput() { return input; }
	ResourceManager* GetResourceManager() { return res_mgr; }
	Scene* GetScene() { return scene; }
	Window* GetWindow() { return window; }

private:
	void Init();
	void Loop();

	GameHandler* handler;
	InputManager* input;
	Window* window;
	Render* render;
	ResourceManager* res_mgr;
	Scene* scene;
	float fps;
	bool shutdown;
};
