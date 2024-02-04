#pragma once

#include "../Engine/InputManager.h"
#include "../Engine/World.h"
#include "../Engine/EventListener.h"

class Model;

class TestWorld
	: public World
	, public EventListener
	, public InputProcesser
{
///TEST
private:
	std::shared_ptr<Model> m_model;
	std::shared_ptr<GameObject> m_Camera;

	bool m_bTurn = false;
	bool m_bGameRun = false;

public:
	void Initialize() override;
	void Update(float deltaTime) override;

public:
	void SetTurn(char* pData);
	void StartTurn(char* pData);
	void EndTurn(char* pData);
	void ChangeTurn(char* pData);

	void ClientsAllReady(char* pData);
	void PeerCharacterMove(char* pData);

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
};

