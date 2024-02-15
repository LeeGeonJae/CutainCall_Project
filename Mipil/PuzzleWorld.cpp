#include "pch.h"
#include "PuzzleWorld.h"
#include "GridManager.h"
#include "TestPlayerObject.h"

#include "../Engine/CameraComponent.h"
#include "../Engine/ControllerComponent.h"
#include "../Engine/FPSCameraController.h"
#include "../Engine/GameObject.h"
#include "../Engine/WorldManager.h"

PuzzleWorld::PuzzleWorld()
{
	WorldManager::GetInstance()->RegisterHandler(S2C_CHARACTER_MOVE, std::bind(&PuzzleWorld::CharacterMove, this, std::placeholders::_1));
	WorldManager::GetInstance()->RegisterHandler(S2C_IS_ALL_READY, std::bind(&PuzzleWorld::ClientsAllReady, this, std::placeholders::_1));
	WorldManager::GetInstance()->RegisterHandler(S2C_END_ACTION, std::bind(&PuzzleWorld::EndAction, this, std::placeholders::_1));
	WorldManager::GetInstance()->RegisterHandler(S2C_START_TURN, std::bind(&PuzzleWorld::ProceedAction, this, std::placeholders::_1));
	WorldManager::GetInstance()->RegisterHandler(S2C_SET_TURN, std::bind(&PuzzleWorld::SetTurn, this, std::placeholders::_1));
}

PuzzleWorld::~PuzzleWorld()
{
}

void PuzzleWorld::Initialize()
{
	m_Camera = CreateGameObject<GameObject>("Test_Camera", eObjectType::CAMERA).lock();
	auto cameraComponent = m_Camera->CreateComponent<CameraComponent>("Test_Camera_Component");
	cameraComponent.lock()->SetLocalPosition({ 50.f, 100.f, -500.f });
	m_Camera->SetRootComponent(cameraComponent.lock());
	auto controllerComponent = m_Camera->CreateComponent<ControllerComponent>("Test_Controller_Component");
	controllerComponent.lock()->CreateController<FPSCameraController>();

	m_bGameRun = false;
	m_hostPlayer = CreateGameObject<TestPlayerObject>("HostPlayer", eObjectType::PLAYER).lock();
	m_guestPlayer = CreateGameObject<TestPlayerObject>("GuestPlayer", eObjectType::PLAYER).lock();

	__super::Initialize();

	m_hostPlayer->SetHostPlayer(true);
	m_guestPlayer->SetHostPlayer(false);

	// 처음 시작할 때 HostPlayer한테 턴주기
	m_hostPlayer->SetTurn(true);

	m_hostPlayer->GetRootComponent().lock()->SetLocalPosition({ 0.f, 0.f, 0.f });
	m_guestPlayer->GetRootComponent().lock()->SetLocalPosition({ 200.f, 0.f, 0.f });

	/// GridManager
	m_gridManager = std::make_shared<GridManager>();
	m_gridManager->CreateMap(10, 10, 5, -900.f, 0.f, -900.f, 200.f);
	m_gridManager->SetMapState(GetGameObjects(eObjectType::LEVEL));
	m_gridManager->SetMapState(GetGameObjects(eObjectType::PLAYER));
}

void PuzzleWorld::Update(float deltaTime)
{
	__super::Update(deltaTime);
}

void PuzzleWorld::CharacterMove(char* pData)
{
	PacketS2C_CharacterMove* pCharacterMove = reinterpret_cast<PacketS2C_CharacterMove*>(pData);
	assert(pCharacterMove != nullptr);

	if (pCharacterMove->who == '0')	// host의 플레이어가 움직이고 싶을 때
	{
		m_gridManager->MoveOnGrid(m_hostPlayer.get(), grid::Z_UP);
		LOG_CONTENTS("host Player girdMove");

	}
	else if (pCharacterMove->who == '1')
	{
		m_gridManager->MoveOnGrid(m_guestPlayer.get(), grid::Z_UP);
		LOG_CONTENTS("guest Player girdMove");
	}

	m_hostPlayer->CheckAction();
	m_guestPlayer->CheckAction();

	if (m_hostPlayer->GetTurn())
		m_hostPlayer->ChangeTurn();
	else
		m_guestPlayer->ChangeTurn();

	delete[] pData;
}

void PuzzleWorld::ClientsAllReady(char* pData)
{
	PacketS2C_IsAllReady* pAllReady = reinterpret_cast<PacketS2C_IsAllReady*>(pData);
	assert(pAllReady != nullptr);

	m_bGameRun = pAllReady->isReady == '0' ? false : true;

	delete[] pData;
}

void PuzzleWorld::ProceedAction(char* pData)
{
	if (m_hostPlayer->GetTurn())
	{
		m_hostPlayer->ProceedAction();
	}
	else if (m_guestPlayer->GetTurn())
	{
		m_guestPlayer->ProceedAction();
	}
	delete pData;
}

void PuzzleWorld::SetTurn(char* pData)
{
	PacketS2C_SetTurn* pSetTurn = reinterpret_cast<PacketS2C_SetTurn*>(pData);

	if (pSetTurn->setTurn == '0')
	{
		m_hostPlayer->SetTurn(true);
		m_guestPlayer->SetTurn(false);
	}
	else if (pSetTurn->setTurn == '1')
	{
		m_hostPlayer->SetTurn(false);
		m_guestPlayer->SetTurn(true);
	}

	delete pData;
}

void PuzzleWorld::EndAction(char* pData)
{
	m_bGameRun = false;

	delete pData;
}
