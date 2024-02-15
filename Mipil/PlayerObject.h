#pragma once
#include "../Engine/EventListener.h"
#include "../Engine/GameObject.h"
#include "../Engine/InputManager.h"

class SkeletalMeshComponent;
class GridManager;

class PlayerObject 
    : public GameObject
    , public InputProcesser
    , public EventListener
{
public:
    PlayerObject();
    ~PlayerObject();

    void Initialize() override;
    void Update(float deltaTime) override;

public:
    void OnInputProcess(
        const Keyboard::State& KeyState,
        const Keyboard::KeyboardStateTracker& KeyTracker,
        const Mouse::State& MouseState,
        const Mouse::ButtonStateTracker& MouseTracker) override;

    void HandleEvent(Event* event) override;

private:
    std::shared_ptr<SkeletalMeshComponent> m_meshComponent;
    std::shared_ptr<GridManager> m_gridManager;
};