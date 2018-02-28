#pragma once

struct SoundManager
{
	SoundManager();
	~SoundManager();
	void Init();
	void Load();

private:
	FMOD::System* system;
};
