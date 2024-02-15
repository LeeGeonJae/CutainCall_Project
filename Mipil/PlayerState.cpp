#include "pch.h"
#include "PlayerState.h"

#include "PlayerFSM.h"
#include "TestPlayerObject.h"

#include "../Engine/EventManager.h"

/// PlayerState
void PlayerState::OnEnter()
{
	EventManager::GetInstance()->SendEvent(eEventType::CHANGE_ANIMATION, m_pOwnerFSM->GetOwnerObject(), m_pParentFSM->GetAnimationName(m_name).c_str());
	LOG_CONTENTS(m_name.c_str());
}

void PlayerState::DecideFSMType()
{
	m_pOwnerFSM = dynamic_cast<PlayerFSM*>(m_pParentFSM);
}


/// State_Idle
State_Idle::State_Idle()
{
	AddTransition(&m_toTurnWait);
	AddTransition(&m_toTurnMove);
	AddTransition(&m_toTurnUp);
}


/// State_Floating
State_Floating::State_Floating()
{
	AddTransition(&m_toTurnWait);
	AddTransition(&m_toTurnFloating);
	AddTransition(&m_toTurnMove);
	AddTransition(&m_toTurnUp);
}


/// State_TurnWait
State_TurnWait::State_TurnWait()
{
	AddTransition(&m_toIdle);
}


/// State_TurnMove
State_TurnMove::State_TurnMove()
{
	AddTransition(&m_toIdle);
	AddTransition(&m_toCrash);
	AddTransition(&m_toGetItem);
}


/// State_TurnUp
State_TurnUp::State_TurnUp()
{
	AddTransition(&m_toIdle);
	AddTransition(&m_toFloating);
	AddTransition(&m_toCrash);
	AddTransition(&m_toGetItem);
}


/// State_TurnFloating
State_TurnFloating::State_TurnFloating()
{
	AddTransition(&m_toFloating);
}


/// State_Crash
State_Crash::State_Crash()
{
	AddTransition(&m_toTurnMove);
	AddTransition(&m_toTurnUp);
}

void State_Crash::OnEnter()
{
	PlayerState::OnEnter();
}


/// State_GetItem
State_GetItem::State_GetItem()
{
	AddTransition(&m_toTurnMove);
	AddTransition(&m_toTurnUp);
}

void State_GetItem::OnEnter()
{
	PlayerState::OnEnter();
}