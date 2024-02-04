#include "pch.h"
#include "ControllerComponent.h"
#include "InputManager.h"
#include "WorldManager.h"

ControllerComponent::~ControllerComponent()
{
	InputManager::GetInstance()->RemoveInputProcesser(this);
}

void ControllerComponent::Initialize()
{
	InputManager::GetInstance()->AddInputProcesser(this);
}

void ControllerComponent::Update(float deltaTime)
{
	//if (m_IsUpdate)
	//{
		for (auto& it : m_InputOperations)
		{
			it->Update(deltaTime);
		}
	//}
}

void ControllerComponent::OnInputProcess(const Keyboard::State& KeyState, const Keyboard::KeyboardStateTracker& KeyTracker, const Mouse::State& MouseState, const Mouse::ButtonStateTracker& MouseTracker)
{
	//m_IsUpdate = (GetOwner().lock()->GetOwnerWorld().lock() == WorldManager::GetInstance()->GetCurWorld().lock());

	//if (m_IsUpdate)
	//{
		for (auto& it : m_InputOperations)
		{
			it->HandleInput(KeyState, KeyTracker, MouseState, MouseTracker);
		}
	//}
}

