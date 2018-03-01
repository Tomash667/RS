#pragma once

struct MainMenu
{
	enum class Event
	{
		ContinueGame,
		NewGame,
		Exit
	};

	void HandleEvent(int id);
};
