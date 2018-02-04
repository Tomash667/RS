#pragma once

struct Engine
{
	Engine(GameHandler* handler);
	~Engine();
	int Run();

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
	ResourceManager* res_mgr;
	Scene* scene;
};
