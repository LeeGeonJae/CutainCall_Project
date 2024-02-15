#pragma once

#include "../Engine/InputManager.h"
#include "../Engine/World.h"
#include "../Engine/EventListener.h"

class GridManager;
enum class eMoveType;
class TestPlayerObject;

class TestWorld
	: public World
	, public EventListener
	, public InputProcesser
{
public:
	TestWorld();
	~TestWorld();

	void Initialize() override;
	void Update(float deltaTime) override;

public:
	// Network
	void SetTurn(char* pData);
	void StartTurn(char* pData);
	void EndTurn(char* pData);
	void ChangeTurn(char* pData);
	void ClientsAllReady(char* pData);
	void CharacterMove(char* pData);

	// Gird Test ¿ë
	void MovePlayer(TestPlayerObject* player, Vector3 vec);
	bool IsGround(Vector3 pos);

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


	///TEST
private:
	std::shared_ptr<GameObject> m_Camera;

	bool m_bTurn = false;
	bool m_bGameRun = false;
	float m_fixedDelta = 0.0f;

	std::shared_ptr<UITest> m_tFadeInOut;
	float m_tFadeDurationTime;

	std::shared_ptr<GridManager> m_gridManager;
};

