#pragma once
#include "Controller.h"
#include "PxPhysicsAPI.h"

class PlayerTestController :
    public Controller
{
public:
    // Controller을(를) 통해 상속됨
    void HandleInput(const Keyboard::State& KeyState, const Keyboard::KeyboardStateTracker& KeyTracker, const Mouse::State& MouseState, const Mouse::ButtonStateTracker& MouseTracker) override;
    void Update(float deltaTime) override;

private:
    physx::PxRigidDynamic* m_rigidDynamic;
};

