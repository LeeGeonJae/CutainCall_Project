#include "pch.h"
//#include "SoundManager.h"
//#include "fmod_errors.h"
//
//#define ERRCHECK(_result) ERRCHECK_fn(_result, __FILE__, __LINE__)
//
//void (*Common_Private_Error)(FMOD_RESULT, const char*, int);
//
//void ERRCHECK_fn(FMOD_RESULT result, const char* file, int line)
//{
//	if (result != FMOD_OK)
//	{
//		if (Common_Private_Error)
//		{
//			Common_Private_Error(result, file, line);
//		}
//		LOG_ENGINE("%s(%d): FMOD error %d - %s", file, line, result, FMOD_ErrorString(result));
//	}
//}
//
//SoundManager::~SoundManager()
//{
//	m_system->close();
//	m_system->release();
//}
//
//std::weak_ptr<FMOD::Sound> SoundManager::LoadSound(std::string filePath, FMOD_MODE mode)
//{
//	FMOD::Sound* sound;
//
//	FMOD_RESULT result;
//	result = m_system->createSound(filePath.c_str(), mode, nullptr, &sound);
//	ERRCHECK(result);
//	m_soundMap[filePath] = std::make_shared<FMOD::Sound>(sound);
//}
//
//void SoundManager::playSound(std::weak_ptr<FMOD::Sound> sound)
//{
//}
//
//void SoundManager::playSound(std::string filePath)
//{
//}
//
//void SoundManager::Initailize()
//{
//	auto sys = m_system.get();
//	FMOD_RESULT result;
//	result = FMOD::System_Create(&sys);
//	ERRCHECK(result);
//	result = m_system->init(32, FMOD_INIT_NORMAL, nullptr);
//	ERRCHECK(result);
//}
