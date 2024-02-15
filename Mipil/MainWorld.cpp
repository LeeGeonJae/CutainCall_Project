#include "pch.h"
#include "MainWorld.h"

#include "../Engine/EventManager.h"
#include "../Engine/UIButton.h"
#include "../Engine/UIClickPopUpFunctor.h"
#include "../Engine/UIEndGameClick.h"
#include "../Engine/UIFunctorPopUp.h"
#include "../Engine/UIGameStartClick.h"
#include "../Engine/UIHoverPopUpFunctor.h"
#include "../Engine/UIMainWorldButtonHover.h"
#include "../Engine/WorldManager.h"

void MainWorld::Initialize()
{
	LOG_CONTENTS("MainWorld Init");

	// 게임 시작 버튼 UI와 Functor
	auto GameStartFunctor = std::make_shared<UIMainWorldButtonHover>();
	GameStartFunctor->SetHoverTexturePath(L"../Resources/Textures/UIGameStartHover.png");

	auto GameStartUI = m_UIManager->CreateUI<UIButton, UIGameStartClick>(L"GameStartUI");
	GameStartUI->SetTexturePath(L"../Resources/Textures/UIGameStart.png");
	GameStartUI->SetSize({ 500.f, 100.f });
	GameStartUI->SetPosition({ 0.f, -100.f });
	GameStartUI->SetScale({ 1.f, 1.f });
	GameStartUI->SetMouseHoverFunctor(GameStartFunctor);

	// 크레딧 버튼 UI와 Functor
	auto CreditFunctor = std::make_shared<UIMainWorldButtonHover>();
	CreditFunctor->SetHoverTexturePath(L"../Resources/Textures/UICreditHover.png");

	auto CreditUI = m_UIManager->CreateUI<UIButton>(L"CreditUI");
	CreditUI->SetTexturePath(L"../Resources/Textures/UICredit.png");
	CreditUI->SetSize({ 500.f, 100.f });
	CreditUI->SetPosition({ 0.f, -250.f });
	CreditUI->SetScale({ 1.f, 1.f });
	CreditUI->SetMouseHoverFunctor(CreditFunctor);

	// 게임 종료 버튼 UI와 Functor
	auto EndGameFunctor = std::make_shared<UIMainWorldButtonHover>();
	EndGameFunctor->SetHoverTexturePath(L"../Resources/Textures/UIEndGameHover.png");

	auto EndGameUI = m_UIManager->CreateUI<UIButton>(L"EndGameUI");
	EndGameUI->SetTexturePath(L"../Resources/Textures/UIEndGame.png");
	EndGameUI->SetSize({ 500.f, 100.f });
	EndGameUI->SetPosition({ 0.f, -400.f });
	EndGameUI->SetScale({ 1.f, 1.f });
	EndGameUI->SetMouseHoverFunctor(EndGameFunctor);
	
	// 게임 종료 팝업 UI와 Functor
	auto EndGamePopUpUI = m_UIManager->CreateUI<UIFunctorPopUp>(L"EndGamePopUpUI");
	EndGamePopUpUI->SetTexturePath(L"../Resources/Textures/UIEndGamePopUp.png");
	EndGamePopUpUI->SetSize({ 1000.f, 1000.f });
	EndGamePopUpUI->SetPosition({ 0.f, 0.f });
	EndGamePopUpUI->SetScale({ 1.f, 1.f });

	auto EndGameClick = std::make_shared <UIClickPopUpFunctor>();
	EndGameClick->SetTargetUI(EndGamePopUpUI);

	auto EndGameOkUI = m_UIManager->CreateUI<UIButton>(L"EndGameOkUI");
	EndGameOkUI->SetTexturePath(L"../Resources/Textures/UIEndGameOk.png");
	EndGameOkUI->SetSize({ 200.f, 100.f });
	EndGameOkUI->SetPosition({ -300.f, 0.f });
	EndGameOkUI->SetScale({ 1.f, 1.f });

	auto EndGameNoUI = m_UIManager->CreateUI<UIButton>(L"EndGameNoUI");
	EndGameNoUI->SetTexturePath(L"../Resources/Textures/UIEndGameNo.png");
	EndGameNoUI->SetSize({ 200.f, 100.f });
	EndGameNoUI->SetPosition({ 300.f, 0.f });
	EndGameNoUI->SetScale({ 1.f, 1.f });
	EndGamePopUpUI->AddChildren(EndGameOkUI, EndGameNoUI);

	EndGameUI->SetMouseClickFunctor(EndGameClick);

	__super::Initialize();
}

void MainWorld::Update(float deltaTime)
{
	World::Update(deltaTime);
}

void MainWorld::OnEnter()
{
	World::OnEnter();
}

void MainWorld::OnExit()
{
	World::OnExit();
}

void MainWorld::HandleEvent(Event* event)
{
	if (event->eventID == eEventType::CHANGE_WORLD)
	{
		WorldManager::GetInstance()->ChangeWorld(event->GetData<eWorldType>());
	}
}
