#pragma once

struct GameHandler
{
	virtual void OnInit() = 0;
	virtual void OnUpdate(float dt) = 0;
};
