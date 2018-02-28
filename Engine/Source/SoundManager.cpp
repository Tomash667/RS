#include "Pch.h"
#include "SoundManager.h"
#include <fmod.hpp>


SoundManager::SoundManager() : system(nullptr)
{
}

SoundManager::~SoundManager()
{
	if(system)
		system->release();
}

void SoundManager::Init()
{
	FMOD_RESULT result = FMOD::System_Create(&system);
	if(result != FMOD_OK)
		throw Format("Failed to create FMOD system (%d).", result);

	result = system->init(128, FMOD_INIT_NORMAL, nullptr);
	if(result != FMOD_OK)
		throw Format("Failed to initialize FMOD system (%d).", result);
}

void SoundManager::Load()
{
	cstring path;
	//system->createStream(path, FMOD_HARDWARE | FMOD_LOWMEM
}
