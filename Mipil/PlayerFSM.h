#pragma once
#include "../Engine/FSM.h"

#include "IdleState.h"
#include "RunState.h"

class PlayerObject;

enum class ePlayerState
{
	IDLE,
	RUN,

	END
};

class PlayerFSM
	: public FSM
{
public:
	PlayerFSM();


	PlayerObject* GetOwnerObject() const { return m_pOwner; }
	PlayerTransition* GetParentTransition() const;

	void SetOwnerObject(PlayerObject* player) { m_pOwner = player; }


private:
	IdleState m_idleState;
	RunState m_runState;

	PlayerObject* m_pOwner;
};