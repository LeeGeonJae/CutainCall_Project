#include "pch.h"
#include "PlayerObject.h"

#include "GridManager.h"
#include "TestWorld.h"

#include "../Engine/SkeletalMeshComponent.h"
#include "../Engine/EventManager.h"
#include "../Engine/WorldManager.h"

PlayerObject::PlayerObject()
{
	InputManager::GetInstance()->AddInputProcesser(this);

	EventManager::GetInstance()->RegisterListener(eEventType::CHANGE_ANIMATION, this);
	EventManager::GetInstance()->RegisterListener(eEventType::MOVE_ON_GRID, this);
}

PlayerObject::~PlayerObject()
{
	InputManager::GetInstance()->RemoveInputProcesser(this);

	EventManager::GetInstance()->UnregisterListener(eEventType::CHANGE_ANIMATION, this);
	EventManager::GetInstance()->UnregisterListener(eEventType::MOVE_ON_GRID, this);
}

void PlayerObject::Initialize()
{

	m_meshComponent = CreateComponent<SkeletalMeshComponent>("SkeletalMeshComponent").lock();
	m_meshComponent->SetLocalPosition({ 50.f, 0.f, 50.f });
	SetRootComponent(m_meshComponent);

	//m_bTurn = false;

	GameObject::Initialize();
}

void PlayerObject::Update(float deltaTime)
{
	GameObject::Update(deltaTime);
}

void PlayerObject::OnInputProcess(const Keyboard::State& KeyState, const Keyboard::KeyboardStateTracker& KeyTracker, const Mouse::State& MouseState, const Mouse::ButtonStateTracker& MouseTracker)
{
}

void PlayerObject::HandleEvent(Event* event)
{
}
