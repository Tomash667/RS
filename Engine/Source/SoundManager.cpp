#include "Pch.h"
#include "SoundManager.h"
#include "Sound.h"
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

Sound* SoundManager::Load(cstring path, bool is_music)
{
	int flags = FMOD_HARDWARE | FMOD_LOWMEM;
	if(is_music)
		flags |= FMOD_2D;
	else
		flags |= FMOD_3D | FMOD_LOOP_OFF;
	FMOD::Sound* sound;
	FMOD_RESULT result = system->createStream(path, flags, nullptr, &sound);
	if(result != FMOD_OK)
		throw Format("Failed to load %s '%s' (%d).", is_music ? "music" : "sound", path, result);
	Sound* snd = new Sound;
	snd->type = Resource::Sound;
	snd->sound = sound;
	snd->is_music = is_music;
	return snd;
}
