#pragma once

struct SoundManager
{
	SoundManager();
	~SoundManager();
	void Init();
	Sound* Load(cstring path, bool is_music);

private:
	FMOD::System* system;
};
