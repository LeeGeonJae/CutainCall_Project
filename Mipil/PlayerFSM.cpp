#include "pch.h"
#include "PlayerFSM.h"

PlayerFSM::PlayerFSM()
{
	AddState("State_Idle", &m_idleState);
	AddState("State_Run", &m_runState);
	AddAnimation("State_Idle", "../Resources/FBX/SkinningTest.fbx");
	AddAnimation("State_Run", "../Resources/FBX/Zombie_Run.fbx");

	m_idleState.DecideFSMType();
	m_runState.DecideFSMType();

	SetDefaultState("State_Run");
}

PlayerTransition* PlayerFSM::GetParentTransition() const
{
	return m_idleState.GetParentTransition();
}
