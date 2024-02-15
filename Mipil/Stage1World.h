#pragma once
#include "PuzzleWorld.h"

#include "../Engine/InputManager.h"
#include "../Engine/EventListener.h"

class TestPlayerObject;
class GridManager;

class Stage1World
    : public PuzzleWorld
    , public EventListener
    , public InputProcesser
{
public:
	Stage1World();
	~Stage1World();

	void Initialize() override;
	void Update(float deltaTime) override;

public:
	void OnEnter() override;
	void OnExit() override;

public:
	void HandleEvent(Event* event) override;

	void OnInputProcess(
		const Keyboard::State& KeyState,
		const Keyboard::KeyboardStateTracker& KeyTracker,
		const Mouse::State& MouseState,
		const Mouse::ButtonStateTracker& MouseTracker) override;

private:
	std::shared_ptr<GameObject> m_Camera;
	std::shared_ptr<TestPlayerObject> m_hostPlayer;
	std::shared_ptr<TestPlayerObject> m_guestPlayer;
	std::shared_ptr<GridManager> m_gridManager;
};

