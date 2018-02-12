#include "Pch.h"
#include "Game.h"
#include <Windows.h>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	Game* game = new Game;
	int result = game->Run();
	delete game;
	return result;
}
