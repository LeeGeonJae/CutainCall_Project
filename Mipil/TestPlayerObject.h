#pragma once
#include "../Engine/EventListener.h"
#include "../Engine/GameObject.h"
#include "../Engine/InputManager.h"

class SkeletalMeshComponent;
class FSMComponent;
class PlayerTransition_test;

// 플레이어 입력을 받을 때 상하좌우/상승/가만히로 받는데
// 액션 타입을 따로 둘 것인지, grid::이걸로 퉁칠건지
// 나중에 필요하면 액션 타입 따로 두는걸로
//enum class eActionType
//{
//	WAIT,
//	MOVE,
//	UP,
//
//	END
//};

class TestPlayerObject
	: public GameObject
	, public InputProcesser
	, public EventListener
{
public:
	TestPlayerObject();
	~TestPlayerObject();

	void SetHostPlayer(bool isHost) { m_bHostPlayer = isHost; }
	bool IsHostPlayer() { return m_bHostPlayer; }

	void SetTurn(bool isTurn) { m_bTurn = isTurn; }
	bool GetTurn() { return m_bTurn; }

	void Initialize() override;
	void Update(float deltaTime) override;

public:
	void OnInputProcess(
		const Keyboard::State& KeyState,
		const Keyboard::KeyboardStateTracker& KeyTracker,
		const Mouse::State& MouseState,
		const Mouse::ButtonStateTracker& MouseTracker) override;

	void HandleEvent(Event* event) override;

// 게임 로직
public:
	void ProceedAction();
	void CheckAction();
	void ChangeTurn();

private:
	std::shared_ptr<SkeletalMeshComponent> m_meshComponent;
	std::shared_ptr<FSMComponent> m_fsmComponent;
	//std::shared_ptr<GridManager> m_gridManager;

	// 호스트 서버의 플레이어인지 아닌지
	bool m_bHostPlayer = false;
	// host 턴인지 클라턴인지 확인
	bool m_bTurn = false;

	Vector3 m_action[4] = { grid::Z_UP, grid::Z_UP ,grid::Z_UP ,grid::Z_UP };

	int m_index = 0;
};

