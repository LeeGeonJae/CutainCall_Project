#pragma once
#include "../Engine/World.h"

class TestPlayerObject;
class GridManager;

class PuzzleWorld 
    : public World
{
public:
	PuzzleWorld();
	~PuzzleWorld();

	void Initialize() override;
	void Update(float deltaTime) override;

public:
	void OnEnter() override {};
	void OnExit() override {};

protected:
//Network
	void CharacterMove(char* pData);
	void ClientsAllReady(char* pData);
	void ProceedAction(char* pData);
	void SetTurn(char* pData);
	void EndAction(char* pData);

protected:
	bool m_bGameRun = false;
	std::shared_ptr<GameObject> m_Camera;
	std::shared_ptr<TestPlayerObject> m_hostPlayer;
	std::shared_ptr<TestPlayerObject> m_guestPlayer;
	std::shared_ptr<GridManager> m_gridManager;
};

