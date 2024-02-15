#include "pch.h"
#include "EventManager.h"
#include "UIGameStartClick.h"
#include "World.h"
#include "WorldManager.h"

void UIGameStartClick::Execute(UIObject& onwer, float deltaTime)
{
	LOG_ENGINE("GameStart UI Click");
	EventManager::GetInstance()->SendEvent(eEventType::CHANGE_WORLD, this, eWorldType::TEST);
}

void UIGameStartClick::HandleEvent(Event* event)
{
	if (event->eventID == eEventType::CHANGE_WORLD)
	{
		WorldManager::GetInstance()->ChangeWorld(event->GetData<eWorldType>());
	}
}
