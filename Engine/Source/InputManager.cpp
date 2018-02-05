#include "Pch.h"
#include "InputManager.h"

InputManager::InputManager() : mouse_dif(Int2::Zero)
{
	for(uint i = 0; i < 255; ++i)
		keystate[i] = IS_UP;
}

void InputManager::Process(Key key, bool down)
{
	auto& k = keystate[(int)key];
	if(key != Key::Escape)
	{
		if(down)
		{
			if(k <= IS_RELEASED)
				k = IS_PRESSED;
			//keyrepeat[key] = true;
		}
		else
		{
			if(k == IS_PRESSED)
				to_release.push_back(key);
			else if(k == IS_DOWN)
				k = IS_RELEASED;
		}
	}
	else
		k = IS_PRESSED;
}

void InputManager::Update()
{
	byte printscreen = keystate[(int)Key::PrintScreen];
	for(uint i = 0; i < 256; ++i)
	{
		if(keystate[i] & 1)
			--keystate[i];
		//keyrepeat[i] = false;
	}
	//for(uint i = 0; i < 5; ++i)
	//	doubleclk[i] = false;
	if(printscreen == IS_PRESSED)
		keystate[(int)Key::PrintScreen] = IS_RELEASED;
	for(Key k : to_release)
		keystate[(int)k] = IS_RELEASED;
	to_release.clear();
}

void InputManager::ReleaseKeys()
{
	for(uint i = 0; i < 255; ++i)
	{
		if(keystate[i] & 0x2)
			keystate[i] = IS_RELEASED;
	}
	//for(uint i = 0; i < 5; ++i)
	//	doubleclk[i] = false;
	to_release.clear();
}
