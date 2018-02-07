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
	Logger::global = new Logger("log.txt");
}

Engine::~Engine()
{
	delete scene;
	delete res_mgr;
	delete render;
	delete window;
	delete input;
	delete Logger::global;
}

int Engine::Run(StartupOptions* options)
{
	bool own_options = false;
	if(!options)
	{
		options = new StartupOptions;
		own_options = true;
	}

	try
	{
		Init(*options);
	}
	catch(cstring err)
	{
		if(own_options)
			delete options;
		ShowError(Format("Failed to initialize game: %s", err));
		return 1;
	}
	if(own_options)
		delete options;

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

void Engine::Init(StartupOptions& options)
{
	Info("Init engine.");
	input = new InputManager;

	window = new Window(this, input);
	window->SetTitle(options.title);
	window->Init();

	render = new Render(window);
	render->Init();

	res_mgr = new ResourceManager(render);

	scene = new Scene(render);
	scene->Init();

	Info("Init game.");
	handler->OnInit();
}

void Engine::Loop()
{
	Info("Start game loop.");

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
	Info("Shutting down engine.");
}

void Engine::ShowError(cstring msg)
{
	assert(msg);
	Error(msg);
	HWND hwnd = window ? (HWND)window->GetHandle() : nullptr;
	MessageBox(hwnd, msg, nullptr, MB_OK | MB_ICONERROR);
}
