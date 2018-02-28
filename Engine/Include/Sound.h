#pragma once

#include "Resource.h"

struct Sound final : Resource
{
	FMOD::Sound* sound;
	bool is_music;
};
