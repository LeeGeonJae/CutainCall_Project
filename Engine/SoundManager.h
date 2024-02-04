#pragma once
#include "fmod.hpp"
#include "Singleton.h"

class SoundManager
	: public Singleton<SoundManager>
{
public:
	~SoundManager();

	std::weak_ptr<FMOD::Sound> LoadSound(std::string filePath, FMOD_MODE mode = FMOD_DEFAULT);

	void playSound(std::weak_ptr<FMOD::Sound> sound);
	void playSound(std::string filePath);

	void Initailize();

private:
	std::unique_ptr<FMOD::System> m_system;
	std::unordered_map<std::string, std::shared_ptr<FMOD::Sound>> m_soundMap;
};

