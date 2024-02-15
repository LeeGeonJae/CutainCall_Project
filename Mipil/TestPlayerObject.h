#pragma once
#include "../Engine/EventListener.h"
#include "../Engine/GameObject.h"
#include "../Engine/InputManager.h"

class SkeletalMeshComponent;
class FSMComponent;
class PlayerTransition_test;

// �÷��̾� �Է��� ���� �� �����¿�/���/�������� �޴µ�
// �׼� Ÿ���� ���� �� ������, grid::�̰ɷ� ��ĥ����
// ���߿� �ʿ��ϸ� �׼� Ÿ�� ���� �δ°ɷ�
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

// ���� ����
public:
	void ProceedAction();
	void CheckAction();
	void ChangeTurn();

private:
	std::shared_ptr<SkeletalMeshComponent> m_meshComponent;
	std::shared_ptr<FSMComponent> m_fsmComponent;
	//std::shared_ptr<GridManager> m_gridManager;

	// ȣ��Ʈ ������ �÷��̾����� �ƴ���
	bool m_bHostPlayer = false;
	// host ������ Ŭ�������� Ȯ��
	bool m_bTurn = false;

	Vector3 m_action[4] = { grid::Z_UP, grid::Z_UP ,grid::Z_UP ,grid::Z_UP };

	int m_index = 0;
};

