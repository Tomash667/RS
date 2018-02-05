#include "Pch.h"
#include "Engine.h"
#include "GameHandler.h"
#include "InputManager.h"
#include "Window.h"
#include "Timer.h"

Engine::Engine(GameHandler* handler) : handler(handler), window(nullptr), shutdown(false)
{
	assert(handler);
}

Engine::~Engine()
{
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
		return 1;
	}

	try
	{
		Loop();
	}
	catch(cstring err)
	{
		return 2;
	}

	return 0;
}

void Engine::Init()
{
	input = new InputManager;

	window = new Window(this, input);
	window->Init();

	handler->OnInit();
}

void Engine::Loop()
{
	Timer timer;
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

		//scene->Draw();
		input->Update();
	}
}

void Engine::Shutdown()
{
	shutdown = true;
}
