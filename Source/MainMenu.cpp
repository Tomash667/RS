#include "Pch.h"
#include "MainMenu.h"


void MainMenu::HandleEvent(int id)
{
	switch((Event)id)
	{
	case Event::NewGame:
	case Event::ContinueGame:
	case Event::Exit:
		break;
	}
}
