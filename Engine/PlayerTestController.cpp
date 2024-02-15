#include "pch.h"
#include "PlayerTestController.h"
#include "RigidDynamicComponent.h"

void PlayerTestController::HandleInput(const Keyboard::State& KeyState, const Keyboard::KeyboardStateTracker& KeyTracker, const Mouse::State& MouseState, const Mouse::ButtonStateTracker& MouseTracker)
{
	if (m_rigidDynamic == nullptr)
	{
		m_rigidDynamic = m_owner.lock()->GetComponentByTypeName<RigidDynamicComponent>().lock()->GetRigidDynamic();
	}

	if (KeyTracker.IsKeyPressed(Keyboard::Keys::Space))
	{
		m_rigidDynamic->addForce(PxVec3(0.0f, 10000.f, 0.0f), PxForceMode::eIMPULSE);
	}

	if (KeyState.IsKeyDown(DirectX::Keyboard::Keys::Up))
	{
		m_rigidDynamic->addForce(PxVec3(0.0f, 0.f, .2f), PxForceMode::eVELOCITY_CHANGE);
	}

	else if (KeyState.IsKeyDown(DirectX::Keyboard::Keys::Down))
	{
		m_rigidDynamic->addForce(PxVec3(0.0f, 0.f, -.2f), PxForceMode::eVELOCITY_CHANGE);
	}

	if (KeyState.IsKeyDown(DirectX::Keyboard::Keys::Right))
	{
		m_rigidDynamic->addForce(PxVec3(.2f, 0.f, 0.0f), PxForceMode::eVELOCITY_CHANGE);
	}

	else if (KeyState.IsKeyDown(DirectX::Keyboard::Keys::Left))
	{
		m_rigidDynamic->addForce(PxVec3(-.2f, 0.f, 0.0f), PxForceMode::eVELOCITY_CHANGE);
	}


}

void PlayerTestController::Update(float deltaTime)
{
}
