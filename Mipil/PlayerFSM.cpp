#include "pch.h"
#include "PlayerFSM.h"

PlayerFSM::PlayerFSM()
{
	AddState("State_Idle", &m_idle);
	AddState("State_Floating", &m_floating);
	AddState("State_TurnWait", &m_turnWait);
	AddState("State_TurnMove", &m_turnMove);
	AddState("State_TurnUp", &m_turnUp);
	AddState("State_TurnFloating", &m_turnFloating);
	AddState("State_Crash", &m_crash);
	AddState("State_GetItem", &m_getItem);
	AddAnimation("State_Idle", "../Resources/FBX/SkinningTest.fbx");
	AddAnimation("State_Floating", "../Resources/FBX/SkinningTest.fbx");
	AddAnimation("State_TurnWait", "../Resources/FBX/Zombie_Run.fbx");
	AddAnimation("State_TurnMove", "../Resources/FBX/Zombie_Run.fbx");
	AddAnimation("State_TurnUp", "../Resources/FBX/Zombie_Run.fbx");
	AddAnimation("State_TurnFloating", "../Resources/FBX/Zombie_Run.fbx");
	AddAnimation("State_Crash", "../Resources/FBX/SkinningTest.fbx");
	AddAnimation("State_GetItem", "../Resources/FBX/SkinningTest.fbx");

	m_idle.DecideFSMType();
	m_floating.DecideFSMType();
	m_turnWait.DecideFSMType();
	m_turnMove.DecideFSMType();
	m_turnUp.DecideFSMType();
	m_turnFloating.DecideFSMType();
	m_crash.DecideFSMType();
	m_getItem.DecideFSMType();

	SetDefaultState("State_Idle");
}