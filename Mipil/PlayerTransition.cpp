#include "pch.h"
#include "PlayerTransition.h"

#include "../Engine/EventManager.h"

PlayerTransition::~PlayerTransition()
{
	EventManager::GetInstance()->UnregisterListener(eEventType::TRANSPER_MOVEMENT, this);
}

void PlayerTransition::HandleEvent(Event* event)
{
	if(event->eventID == eEventType::TRANSPER_MOVEMENT)
	{
		m_bMoving = event->GetData<bool>();
	}
}
