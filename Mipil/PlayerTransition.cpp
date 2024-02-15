#include "pch.h"
#include "PlayerTransition.h"

#include "../Engine/EventManager.h"

PlayerTransition::PlayerTransition()
{
	EventManager::GetInstance()->RegisterListener(eEventType::CHANGE_TURN, this);
	EventManager::GetInstance()->RegisterListener(eEventType::GET_THIS_ACTION, this);
	EventManager::GetInstance()->RegisterListener(eEventType::CHECK_GROUND, this);
	EventManager::GetInstance()->RegisterListener(eEventType::CHECK_CRASH, this);
	EventManager::GetInstance()->RegisterListener(eEventType::CHECK_GET_ITEM, this);
}

PlayerTransition::~PlayerTransition()
{
	EventManager::GetInstance()->UnregisterListener(eEventType::CHANGE_TURN, this);
	EventManager::GetInstance()->UnregisterListener(eEventType::GET_THIS_ACTION, this);
	EventManager::GetInstance()->UnregisterListener(eEventType::CHECK_GROUND, this);
	EventManager::GetInstance()->UnregisterListener(eEventType::CHECK_CRASH, this);
	EventManager::GetInstance()->UnregisterListener(eEventType::CHECK_GET_ITEM, this);
}

void PlayerTransition::HandleEvent(Event* event)
{
	switch (event->eventID)
	{
	case eEventType::CHANGE_TURN:
		m_bTurn = event->GetData<bool>();
		return;
	case eEventType::GET_THIS_ACTION:
		m_action = event->GetData<Vector3>();
		return;
	case eEventType::CHECK_GROUND:
		m_bGround = event->GetData<bool>();
		return;
	case eEventType::CHECK_CRASH:
		m_bOnCrash = event->GetData<bool>();
		return;
	case eEventType::CHECK_GET_ITEM:
		m_bOnGetItem = event->GetData<bool>();
		return;
	default:
		assert(true);
		break;
	}
}

bool PlayerTransition::IsMoveState(Vector3 vec)
{
	if (vec == grid::X_DOWN || vec == grid::X_UP || vec == grid::Z_DOWN || vec == grid::Z_UP)
		return true;
	else
		return false;
}
