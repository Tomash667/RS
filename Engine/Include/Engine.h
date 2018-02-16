#pragma once

#include "StartupOptions.h"
#include "WindowHandler.h"

struct Engine : WindowHandler
{
	Engine(GameHandler* handler);
	~Engine();
	int Run(StartupOptions* options = nullptr);
	void Shutdown();
	void ShowError(cstring msg);

	float GetFps() { return fps; }
	Gui* GetGui() { return gui; }
	InputManager* GetInput() { return input; }
	Render* GetRender() { return render; }
	ResourceManager* GetResourceManager() { return res_mgr; }
	Scene* GetScene() { return scene; }
	Window* GetWindow() { return window; }

private:
	void Init(StartupOptions& options);
	void Loop();

	GameHandler* handler;
	InputManager* input;
	Window* window;
	Render* render;
	ResourceManager* res_mgr;
	Scene* scene;
	Gui* gui;
	float fps;
	bool shutdown;
};
