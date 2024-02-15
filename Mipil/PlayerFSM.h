#pragma once

#include "../Engine/FSM.h"

#include "PlayerState.h"

class TestPlayerObject;

class PlayerFSM
	: public FSM
{
public:
	PlayerFSM();


	TestPlayerObject* GetOwnerObject() const { return m_pOwner; }

	void SetOwnerObject(TestPlayerObject* player) { m_pOwner = player; }


private:
	State_Idle m_idle;
	State_Floating m_floating;
	State_TurnWait m_turnWait;
	State_TurnMove m_turnMove;
	State_TurnUp m_turnUp;
	State_TurnFloating m_turnFloating;
	State_Crash m_crash;
	State_GetItem m_getItem;

	TestPlayerObject* m_pOwner;
};

