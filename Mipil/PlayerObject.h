#pragma once
#include "../Engine/EventListener.h"
#include "../Engine/GameObject.h"
#include "../Engine/InputManager.h"

class SkeletalMeshComponent;
class FSMComponent;
class PlayerTransition;

class PlayerObject
	: public GameObject
	, public InputProcesser
	, public EventListener
{
public:
	~PlayerObject();


	void SetHostPlayer() { m_bHostPlayer = true; }
	bool IsHostPlayer() { return m_bHostPlayer; }


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
	std::shared_ptr<FSMComponent> m_fsmComponent;

	PlayerTransition* m_transition;
	bool m_bMoving = true;
	bool m_bHostPlayer = false;

	bool m_bRun = true;

};

