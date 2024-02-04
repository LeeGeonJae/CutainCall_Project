#include "pch.h"
#include "TestWorld2.h"

#include "../Engine/EventManager.h"
#include "../Engine/WorldManager.h"

void TestWorld2::Initialize()
{
	LOG_CONTENTS("TestWorld2 Init");
	__super::Initialize();
}

void TestWorld2::Update(float deltaTime)
{
	World::Update(deltaTime);

	if (GetAsyncKeyState(VK_LEFT) & 0x8000)
		EventManager::GetInstance()->SendEvent(eEventType::CHANGE_WORLD, this, eWorldType::TEST);
}

void TestWorld2::OnEnter()
{
	World::OnEnter();
}

void TestWorld2::OnExit()
{
	World::OnExit();
}

void TestWorld2::HandleEvent(Event* event)
{
	if (event->eventID == eEventType::CHANGE_WORLD)
	{
		WorldManager::GetInstance()->ChangeWorld(event->GetData<eWorldType>());
	}
}
