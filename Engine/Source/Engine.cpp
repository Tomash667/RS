#include "Pch.h"
#include "Engine.h"
#include "GameHandler.h"
#include "InputManager.h"
#include "Window.h"
#include "Render.h"
#include "ResourceManager.h"
#include "Scene.h"
#include "Timer.h"

Engine::Engine(GameHandler* handler) : handler(handler), input(nullptr), window(nullptr), render(nullptr), res_mgr(nullptr), scene(nullptr), shutdown(false),
fps(0)
{
	assert(handler);
}

Engine::~Engine()
{
	delete scene;
	delete res_mgr;
	delete render;
	delete window;
	delete input;
}

int Engine::Run()
{
	try
	{
		Init();
	}
	catch(cstring err)
	{
		ShowError(Format("Failed to initialize game: %s", err));
		return 1;
	}

	try
	{
		Loop();
	}
	catch(cstring err)
	{
		ShowError(Format("Fatal game error: %s", err));
		return 2;
	}

	return 0;
}

void Engine::Init()
{
	input = new InputManager;

	window = new Window(this, input);
	window->Init();

	render = new Render(window);
	render->Init();

	res_mgr = new ResourceManager(render);

	scene = new Scene(render);

	handler->OnInit();
}

void Engine::Loop()
{
	Timer timer;
	uint frames = 0;
	float frame_time = 0.f;

	while(window->Tick())
	{
		float dt = timer.Tick();

		// calculate fps
		frames++;
		frame_time += dt;
		if(frame_time >= 1.f)
		{
			fps = frames / frame_time;
			frames = 0;
			frame_time = 0.f;
		}

		// update game
		handler->OnUpdate(dt);
		if(shutdown)
			return;

		scene->Draw();
		input->Update();
	}
}

void Engine::Shutdown()
{
	shutdown = true;
}

void Engine::ShowError(cstring msg)
{
	assert(msg);
	// TODO
}
