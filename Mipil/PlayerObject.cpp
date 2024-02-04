#include "pch.h"
#include "PlayerObject.h"

#include "PlayerFSM.h"

#include "../Engine/SkeletalMeshComponent.h"
#include "../Engine/FSMComponent.h"
#include "../Engine/EventManager.h"
#include "../Engine/WorldManager.h"
#include "../NetworkLibrary/MyProtocol.h"

PlayerObject::~PlayerObject()
{
	InputManager::GetInstance()->RemoveInputProcesser(this);

	EventManager::GetInstance()->UnregisterListener(eEventType::CHANGE_ANIMATION, this);
}

void PlayerObject::Initialize()
{
	InputManager::GetInstance()->AddInputProcesser(this);

	std::shared_ptr<PlayerFSM> fsm = std::make_shared<PlayerFSM>();

	m_meshComponent = CreateComponent<SkeletalMeshComponent>("SkeletalMeshComponent").lock();
	m_meshComponent->SetDefaultAnimation(fsm->GetAnimationName("State_Run"));
	m_meshComponent->AddAnimation(fsm->GetAnimationName("State_Idle"));
	m_meshComponent->SetLocalPosition({ 100.f, 0.f, 0.f });
	SetRootComponent(m_meshComponent);

	m_fsmComponent = CreateComponent<FSMComponent>("SkeletalFSMComponent").lock();
	fsm->SetOwnerObject(this);
	m_fsmComponent->AttachFSM(fsm);
	m_transition = fsm->GetParentTransition();

	EventManager::GetInstance()->RegisterListener(eEventType::CHANGE_ANIMATION, this);

	GameObject::Initialize();
}

void PlayerObject::Update(float deltaTime)
{
	EventManager::GetInstance()->SendImmediateEvent(eEventType::TRANSPER_MOVEMENT, m_bMoving);

	GameObject::Update(deltaTime);
}

void PlayerObject::OnInputProcess(const Keyboard::State& KeyState, const Keyboard::KeyboardStateTracker& KeyTracker, const Mouse::State& MouseState, const Mouse::ButtonStateTracker& MouseTracker)
{
	if (m_bHostPlayer)
	{
		if (KeyTracker.IsKeyPressed(DirectX::Keyboard::N))
		{
			m_bMoving = !m_bMoving;
		}

	}
	
	if (!m_bHostPlayer)
	{
		if (KeyTracker.IsKeyPressed(DirectX::Keyboard::M))
		{
			m_bMoving = !m_bMoving;
		}
	}
}

void PlayerObject::HandleEvent(Event* event)
{
	if (event->eventID == eEventType::CHANGE_ANIMATION)
	{
		m_meshComponent->ChangeAnimation(event->GetData<const char*>());
	}
}

