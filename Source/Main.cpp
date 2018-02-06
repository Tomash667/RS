#include "Pch.h"
#include "Game.h"

int main()
{
	Game* game = new Game;
	int result = game->Run();
	delete game;
	return result;
}
