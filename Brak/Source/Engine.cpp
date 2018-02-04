#include "Pch.h"
#include "Engine.h"
#include "GameHandler.h"
#include "Window.h"

Engine::Engine(GameHandler* handler) : handler(handler), window(nullptr)
{
	assert(handler);
}

Engine::~Engine()
{
	delete window;
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
	window = new Window;
	window->Init();

	handler->OnInit();
}

void Engine::Loop()
{
	while(true)
	{
		handler->OnUpdate(0.f);
		Sleep(1);
	}
}
