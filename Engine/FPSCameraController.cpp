#include "pch.h"
#include "FPSCameraController.h"
#include "InputManager.h"

void FPSCameraController::HandleInput(const Keyboard::State& KeyState, const Keyboard::KeyboardStateTracker& KeyTracker, const Mouse::State& MouseState, const Mouse::ButtonStateTracker& MouseTracker)
{
	auto& ownerLocalMatrix = m_owner.lock()->GetRootComponent().lock()->GetLocalTransform();
	Math::Vector3 forward = ownerLocalMatrix.Forward();
	Math::Vector3 right = ownerLocalMatrix.Right();

	if (KeyTracker.IsKeyPressed(Keyboard::Keys::Home))
	{
		m_owner.lock()->GetRootComponent().lock()->SetLocalPosition(Math::Vector3::Zero);
		m_pitch = 0.f;
		m_yaw = 0.f;
	}

	if (KeyState.IsKeyDown(DirectX::Keyboard::Keys::W))
	{
		AddInputVector(-forward);
	}

	else if (KeyState.IsKeyDown(DirectX::Keyboard::Keys::S))
	{
		AddInputVector(forward);
	}

	if (KeyState.IsKeyDown(DirectX::Keyboard::Keys::A))
	{
		AddInputVector(-right);
	}

	else if (KeyState.IsKeyDown(DirectX::Keyboard::Keys::D))
	{
		AddInputVector(right);
	}

	if (MouseState.xButton1)
	{
		AddInputVector(Math::Vector3::Down);
	}

	else if (MouseState.xButton2)
	{
		AddInputVector(Math::Vector3::Up);
	}

	InputManager::GetInstance()->GetMouse()->SetMode(MouseState.rightButton ? Mouse::MODE_RELATIVE : Mouse::MODE_ABSOLUTE);

	if (MouseState.positionMode == Mouse::MODE_RELATIVE)
	{
		Math::Vector3 delta = Math::Vector3(float(MouseState.x), float(MouseState.y), 0.f) * m_rotationSpeed;
		AddPitch(delta.y);
		AddYaw(delta.x);
	}
}

void FPSCameraController::Update(float deltaTime)
{
	const auto& ownerRootComponent = m_owner.lock()->GetRootComponent().lock();

	if (m_InputVector.Length() > 0.0f)
	{
		ownerRootComponent->SetLocalPosition(ownerRootComponent->GetLocalPosition() + (m_InputVector * m_movementSpeed * deltaTime));
		m_InputVector = Math::Vector3::Zero;
	}
	ownerRootComponent->SetLocalRotation(Math::Vector3(XMConvertToDegrees(m_pitch), XMConvertToDegrees(m_yaw), 0.0f));
}

void FPSCameraController::AddInputVector(const Math::Vector3& input)
{
	m_InputVector += input;
	m_InputVector.Normalize();
}

void FPSCameraController::AddPitch(const float value)
{
	m_pitch += value;
	if (m_pitch > XM_PI)
	{
		m_pitch -= XM_2PI;
	}
	else if (m_pitch < -XM_PI)
	{
		m_pitch += XM_2PI;
	}
}

void FPSCameraController::AddYaw(const float value)
{
	m_yaw += value;
	if (m_yaw > XM_PI)
	{
		m_yaw -= XM_2PI;
	}
	else if (m_yaw < -XM_PI)
	{
		m_yaw += XM_2PI;
	}
}
