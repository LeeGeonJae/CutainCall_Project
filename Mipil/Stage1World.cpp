#include "pch.h"
#include "Stage1World.h"

#include "GridManager.h"
#include "TestPlayerObject.h"

#include "../Engine/CameraComponent.h"
#include "../Engine/ControllerComponent.h"
#include "../Engine/FPSCameraController.h"
#include "../Engine/GameObject.h"
#include "../Engine/UIButton.h"
#include "../Engine/UICameraMove.h"
#include "../Engine/UIDynamicObject.h"
#include "../Engine/UIObject.h"
#include "../Engine/UIPanel.h"
#include "../Engine/UIText.h"
#include "../Engine/UITimerFunctor.h"
#include "../Engine/UITriggerPopUp.h"
#include "../Engine/WorldManager.h"

Stage1World::Stage1World()
{
}

Stage1World::~Stage1World()
{
}

void Stage1World::Initialize()
{
	LOG_CONTENTS("Stage1 Init");

	/*m_Camera = CreateGameObject<GameObject>("Test_Camera", eObjectType::CAMERA).lock();
	auto cameraComponent = m_Camera->CreateComponent<CameraComponent>("Test_Camera_Component");
	cameraComponent.lock()->SetLocalPosition({ 50.f, 100.f, -500.f });
	m_Camera->SetRootComponent(cameraComponent.lock());
	auto controllerComponent = m_Camera->CreateComponent<ControllerComponent>("Test_Controller_Component");
	controllerComponent.lock()->CreateController<FPSCameraController>();

	m_bGameRun = false;
	m_hostPlayer = CreateGameObject<TestPlayerObject>("HostPlayer", eObjectType::PLAYER).lock();
	m_guestPlayer = CreateGameObject<TestPlayerObject>("GuestPlayer", eObjectType::PLAYER).lock();*/
	
	// 행동입력 패널
	auto EnterActionUI = m_UIManager->CreateUI<UIPanel>(L"EnterActionUI");
	EnterActionUI->SetTexturePath(L"../Resources/Textures/UIEnterAction.png");
	EnterActionUI->SetSize({ 1920.f, 1080.f });
	EnterActionUI->SetPosition({ 0.f,  0.f });
	EnterActionUI->SetScale({ 1.f, 1.f });

	// 홀로그램 패널(행동입력 패널 자식)
	auto HologramPanelUI = m_UIManager->CreateUI<UIPanel>(L"HologramPanelUI");
	HologramPanelUI->SetTexturePath(L"../Resources/Textures/UIHologram.png");
	HologramPanelUI->SetSize({ 1900.f, 1000.f });
	HologramPanelUI->SetPosition({ 0.f,  0.f });
	HologramPanelUI->SetScale({ 1.f, 1.f });

	// 타이머 UI(행동입력 패널 자식)
	auto TimerPanelUI = m_UIManager->CreateUI<UIPanel>(L"TimerPanelUI");
	TimerPanelUI->SetTexturePath(L"../Resources/Textures/testpanel.png");
	TimerPanelUI->SetSize({ 150.f, 80.f });
	TimerPanelUI->SetPosition({ 0.f,  500.f });
	TimerPanelUI->SetScale({ 1.f, 1.f });

	auto TimerUI = m_UIManager->CreateUI<UIText>(L"TimerUI");
	TimerUI->SetPosition({ 0.f, 0.f });
	TimerUI->SetFont(L"휴먼편지체");
	TimerUI->SetFontSize(30.f);
	TimerUI->SetColor(D2D1::ColorF::Black);
	TimerUI->SetText(std::to_wstring(0.0f));
	TimerPanelUI->AddChildren(TimerUI);

	// 인게임 패널
	auto StagePanelUI = m_UIManager->CreateUI<UIPanel>(L"StagePanelUI");
	StagePanelUI->SetTexturePath(L"../Resources/Textures/UIStagePanel.png");
	StagePanelUI->SetSize({ 1920.f, 1080.f });
	StagePanelUI->SetPosition({ 0.f,  0.f });
	StagePanelUI->SetScale({ 1.f, 1.f });

	// 카메라 이동 활성화 설명문
	auto CameraMoveCommentUI = m_UIManager->CreateUI<UICameraMove>(L"CameraMoveCommentUI");
	CameraMoveCommentUI->SetTexturePath(L"../Resources/Textures/UIMoveCameraComment.png");
	CameraMoveCommentUI->SetSize({ 150.f, 80.f });
	CameraMoveCommentUI->SetPosition({ 900.f,  500.f });
	CameraMoveCommentUI->SetScale({ 1.f, 1.f });

	StagePanelUI->AddChildren(CameraMoveCommentUI);
	StagePanelUI->SetAllVisible(false);
	EnterActionUI->AddChildren(TimerPanelUI, HologramPanelUI);

	__super::Initialize();

	//m_hostPlayer->SetHostPlayer(true);
	//m_guestPlayer->SetHostPlayer(false);

	//// 처음 시작할 때 HostPlayer한테 턴주기
	//m_hostPlayer->SetTurn(true);

	//m_hostPlayer->GetRootComponent().lock()->SetLocalPosition({ 0.f, 0.f, 0.f });
	//m_guestPlayer->GetRootComponent().lock()->SetLocalPosition({ 200.f, 0.f, 0.f });

	///// GridManager
	//m_gridManager = std::make_shared<GridManager>();
	//m_gridManager->CreateMap(10, 10, 5, -900.f, 0.f, -900.f, 200.f);
	//m_gridManager->SetMapState(GetGameObjects(eObjectType::LEVEL));
	//m_gridManager->SetMapState(GetGameObjects(eObjectType::PLAYER));
}

void Stage1World::Update(float deltaTime)
{
	static float fixed = 0.0f;
	static float fixed1 = 0.0f;

	fixed += deltaTime;

	if (fixed >= 1.0f)
	{
		fixed1 += 1.0f;
		fixed -= 1.0f;
	}

	std::wstring wstrValue = std::to_wstring(fixed1);
	auto ui = m_UIManager->GetUIObject<UIText>(L"TimerUI");
	ui.lock()->SetText(wstrValue);

	__super::Update(deltaTime);
	
}

void Stage1World::OnEnter()
{
	InputManager::GetInstance()->AddInputProcesser(this);
	World::OnEnter();
}

void Stage1World::OnExit()
{
	InputManager::GetInstance()->RemoveInputProcesser(this);
	World::OnExit();
}

void Stage1World::HandleEvent(Event* event)
{
}

void Stage1World::OnInputProcess(const Keyboard::State& KeyState, const Keyboard::KeyboardStateTracker& KeyTracker,
	const Mouse::State& MouseState, const Mouse::ButtonStateTracker& MouseTracker)
{
	if (KeyTracker.IsKeyPressed(DirectX::Keyboard::Keys::Tab))
	{
		// 홀로그램 끄기
		auto ui = m_UIManager->GetUIObject<UIPanel>(L"HologramPanelUI");
		if (ui.lock()->GetVisible())
		{
			ui.lock()->SetAllVisible(false);
		}
		else if (!ui.lock()->GetVisible())
		{
			ui.lock()->SetAllVisible(true);
		}
	}

	if (KeyTracker.IsKeyPressed(DirectX::Keyboard::Keys::Escape))
	{
		// 채원 Test UI 바꿔치기
		auto enterActionUI = m_UIManager->GetUIObject<UIPanel>(L"EnterActionUI");
		if (enterActionUI.lock()->GetVisible())
		{
			enterActionUI.lock()->SetAllVisible(false);

		}
		else if (!enterActionUI.lock()->GetVisible())
		{
			enterActionUI.lock()->SetAllVisible(true);
		}

		auto StageUI = m_UIManager->GetUIObject<UIPanel>(L"StagePanelUI");
		if (StageUI.lock()->GetVisible())
		{
			StageUI.lock()->SetAllVisible(false);

		}
		else if (!StageUI.lock()->GetVisible())
		{
			StageUI.lock()->SetAllVisible(true);
		}
	}

	if (KeyTracker.IsKeyPressed(DirectX::Keyboard::Keys::Enter))
	{
		if (m_bGameRun)
		{
			WorldManager::GetInstance()->PushSendQueue(
				WorldManager::GetInstance()->SerializeBuffer(sizeof(PacketC2S_StartTurn), C2S_START_TURN, nullptr),
				sizeof(PacketC2S_StartTurn));
		}
	}

	if (KeyTracker.IsKeyPressed(DirectX::Keyboard::Keys::Delete))
	{
		// 준비 버튼
		PacketC2S_READY ready;
		ready.clickedReady = '1';

		WorldManager::GetInstance()->PushSendQueue(
			WorldManager::GetInstance()->SerializeBuffer(sizeof(PacketC2S_READY), C2S_READY, &ready.clickedReady),
			sizeof(PacketC2S_READY));
	}
}
