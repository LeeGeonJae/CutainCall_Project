#include "pch.h"
#include "TestWorld.h"

#include "../Engine/EventManager.h"
#include "../Engine/GameObject.h"
#include "../Engine/CameraComponent.h"
#include "../Engine/ControllerComponent.h"
#include "../Engine/FPSCameraController.h"
#include "../Engine/Model.h"
#include "../Engine/WorldManager.h"

#include <memory>
#include <string>

#include "../Engine/UITest.h"
#include "../Engine/UIMouseClickTest.h"
#include "../Engine/UIMouseClickTest1.h"
#include "../Engine/UIMouseHoverTest.h"
#include "../Engine/UIMouseHoverTest1.h"
#include "../Engine/UIText.h"

#include "StaticTestObject.h"
#include "PlayerObject.h"
#include "CubeMapObject.h"

#include "../NetworkLibrary/MyProtocol.h"
#include "../D3DRenderer/UIInstance.h"

void TestWorld::Initialize()
{
	WorldManager::GetInstance()->RegisterHandler(S2C_SET_TURN, std::bind(&TestWorld::SetTurn, this, std::placeholders::_1));
	WorldManager::GetInstance()->RegisterHandler(S2C_START_TURN, std::bind(&TestWorld::StartTurn, this, std::placeholders::_1));
	WorldManager::GetInstance()->RegisterHandler(S2C_END_TURN, std::bind(&TestWorld::EndTurn, this, std::placeholders::_1));
	WorldManager::GetInstance()->RegisterHandler(S2C_CHANGE_TURN, std::bind(&TestWorld::ChangeTurn, this, std::placeholders::_1));
	WorldManager::GetInstance()->RegisterHandler(S2C_IS_ALL_READY, std::bind(&TestWorld::ClientsAllReady, this, std::placeholders::_1));
	WorldManager::GetInstance()->RegisterHandler(S2C_CHARACTER_MOVE, std::bind(&TestWorld::PeerCharacterMove, this, std::placeholders::_1));

	m_Camera = CreateGameObject<GameObject>("Test_Camera", eObjectType::CAMERA).lock();
	auto cameraComponent = m_Camera->CreateComponent<CameraComponent>("Test_Camera_Component");
	cameraComponent.lock()->SetLocalPosition({ 50.f, 100.f, -500.f });
	m_Camera->SetRootComponent(cameraComponent.lock());
	auto controllerComponent = m_Camera->CreateComponent<ControllerComponent>("Test_Controller_Component");
	controllerComponent.lock()->CreateController<FPSCameraController>();

	auto TestUI = m_UIManager->CreateUI<UITest, UIMouseHoverTest, UIMouseHoverTest1, UIMouseClickTest, UIMouseClickTest1>(L"TestUI");
	TestUI->SetTexturePath(L"../Resources/Textures/Snake.bmp");
	TestUI->SetSize({ 100.f, 100.f });
	TestUI->SetPosition({ -700.f, -300.f });
	TestUI->SetScale({ 1.5f, 1.5f });

	auto TestTextUI1 = m_UIManager->CreateUI<UIText, void>(L"TestTextUI1");
	TestTextUI1->SetPosition({ -50.f, 0.f });
	TestTextUI1->SetFont(L"휴먼편지체");
	TestTextUI1->SetFontSize(30.f);
	TestTextUI1->SetColor(D2D1::ColorF::Lavender);
	TestTextUI1->SetText(L"되냐?");
	auto TestTextUI2 = m_UIManager->CreateUI<UIText, void>(L"TestTextUI2");
	TestTextUI2->SetPosition({ 50.f, 0.f });
	TestTextUI2->SetFont(L"휴먼편지체");
	TestTextUI2->SetFontSize(30.f);
	TestTextUI2->SetColor(D2D1::ColorF::Maroon);
	TestTextUI2->SetText(L"되네!");
	auto TestTextUI3 = m_UIManager->CreateUI<UIText, void>(L"TestTextUI3");
	TestTextUI3->SetPosition({ 0.f, 50.f });
	TestTextUI3->SetFont(L"Arial");
	TestTextUI3->SetFontSize(30.f);
	TestTextUI3->SetColor(D2D1::ColorF::MediumSeaGreen);
	TestTextUI3->SetText(L"진짜 되냐?");
	auto TestTextUI4 = m_UIManager->CreateUI<UIText, void>(L"TestTextUI4");
	TestTextUI4->SetPosition({ 0.f, -50.f });
	TestTextUI4->SetFont(L"Arial");
	TestTextUI4->SetFontSize(30.f);
	TestTextUI4->SetColor(D2D1::ColorF::Peru);
	TestTextUI4->SetText(L"진짜 되네!");
	TestUI->AddChildren(TestTextUI1, TestTextUI2, TestTextUI3, TestTextUI4);

	//CommonApp::m_pInstance->GetRenderer()->DrawTextRequest(L"되냐?", 0.f, 0.f, D2D1::ColorF::Crimson, L"Arial", 40.f);
	//CommonApp::m_pInstance->GetRenderer()->DrawTextRequest(L"되네!", 0.f, 60.f, D2D1::ColorF::Salmon, L"휴먼편지체", 70.f);

	std::shared_ptr<PlayerObject> hostPlayer = CreateGameObject<PlayerObject>("HostPlayer", eObjectType::PLAYER).lock();
	std::shared_ptr<PlayerObject> guestPlayer = CreateGameObject<PlayerObject>("GuestPlayer", eObjectType::PLAYER).lock();

	std::shared_ptr<StaticTestObject> TestObject = CreateGameObject<StaticTestObject>("TestObject", eObjectType::TEST).lock();
	TestObject->SetOwnerWorld(shared_from_this());

	if(WorldManager::GetInstance()->IsHostServer())
		hostPlayer->SetHostPlayer();
	else guestPlayer->SetHostPlayer();


	// CubeMap
	CreateGameObject<CubeMapObject>("CubeMap", eObjectType::TEST);

	//CommonApp::m_pInstance->GetRenderer()->SetProjectionMatrix(cameraComponent.lock()->GetProjectionMatrix());

	LOG_CONTENTS("TestWorld1 Init");

	char turn[1] = {'0'};
	WorldManager::GetInstance()->PushSendQueue(WorldManager::GetInstance()->SerializeBuffer(
		sizeof(PacketC2S_SetTurn), C2S_SET_TURN, turn),
		sizeof(PacketC2S_SetTurn));

	__super::Initialize();


	hostPlayer->GetRootComponent().lock()->SetLocalPosition({ 0.f, 0.f, 0.f });
	guestPlayer->GetRootComponent().lock()->SetLocalPosition({ 100.f, 0.f, 0.f });
	TestObject->GetRootComponent().lock()->SetLocalPosition({ -100.f, 0.f, 0.f });
	TestObject->GetRootComponent().lock()->SetLocalScale({ 1.f, 1.f, 1.f });

	// 건재 : UI 애니메이션 테스트
	{
		for (int i = 0; i < 13; i++)
		{
			CB_UIAnimationKeyframe keyframe;
			keyframe.m_Offset.x = 100.f * i;
			keyframe.m_Offset.y = 400.f;
			keyframe.m_TextureSize.x = 1300.f;
			keyframe.m_TextureSize.y = 600.f;
			keyframe.m_Size.x = 100.f;
			keyframe.m_Size.y = 100.f;
			keyframe.m_AnimationTime = 0.1f;
			TestUI->GetUIInstance().lock()->SetIsLoop(false);
			TestUI->AddKeyFrame(keyframe);
		}
	}

	m_bGameRun = false;
}

void TestWorld::Update(float deltaTime)
{
	World::Update(deltaTime);

	/*if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
		EventManager::GetInstance()->SendEvent(eEventType::CHANGE_WORLD, this, eWorldType::TEST2);*/


		// 모든 클라가 레디라면!
	if (m_bGameRun)
	{
		EventManager::GetInstance()->SendEvent(eEventType::CHANGE_WORLD, this, eWorldType::TEST2);
	}
}

// 턴 설정해주기
void TestWorld::SetTurn(char* pData)
{
	if (pData[4] == '0')
	{
		m_bTurn = true;
		printf("1P 턴이당\n");
	}
	else if (pData[4] == '1')
	{
		m_bTurn = true;
		printf("2P 턴이당\n");
	}

	delete[] pData;
}

void TestWorld::StartTurn(char* pData)
{
	delete[] pData;
	// 누구 턴인지 BroadCast하게 해줌.
	char* broadCastTurn = new char[SND_BUF_SIZE];
	const char* msg = "1P Turn";
	memcpy(broadCastTurn, msg, SND_BUF_SIZE);

	WorldManager::GetInstance()->PushSendQueue(WorldManager::GetInstance()->SerializeBuffer(
		sizeof(PacketC2S_BroadcastMsg), C2S_BROADCAST_MSG, broadCastTurn),
		sizeof(PacketC2S_BroadcastMsg));

	// character가 이동한 값을 받아와서 보내주긔
	/*char* move = new char[SND_BUF_SIZE];
	const char* msg = "100";
	memcpy(move, msg, SND_BUF_SIZE);

	WorldManager::GetInstance()->PushSendQueue(WorldManager::GetInstance()->SerializeBuffer(
		sizeof(PacketC2S_CharacterMove), C2S_CHARACTER_MOVE, move),
		sizeof(PacketC2S_CharacterMove));*/
}

void TestWorld::EndTurn(char* pData)
{
	if (m_bTurn)
		m_bTurn = false;

	delete[] pData;
}

void TestWorld::ChangeTurn(char* pData)
{
	m_bTurn = m_bTurn ? false : true;

	delete[] pData;
}

void TestWorld::ClientsAllReady(char* pData)
{
	m_bGameRun = pData[4] == '0' ? false : true;
	delete[] pData;
}

void TestWorld::PeerCharacterMove(char* pData)
{
	for (auto& object : m_gameObjects[static_cast<int>(eObjectType::PLAYER)])
	{
		std::shared_ptr<PlayerObject> pObj = std::dynamic_pointer_cast<PlayerObject>(object);
		if (pObj != nullptr)
		{
			// 호스트 서버에서 플레이어가 움직였다
			if (pData[4] == '0')
			{
				if (WorldManager::GetInstance()->IsHostServer() && pObj->IsHostPlayer())
				{
					// z축 이동
					if (pData[5] == '0')
					{
						Math::Vector3 pos = pObj->GetRootComponent().lock()->GetLocalPosition();
						pObj->GetRootComponent().lock()->SetLocalPosition({ pos.x, pos.y, pos.z + 50.f });
					}
				}
				else if (!WorldManager::GetInstance()->IsHostServer() && !pObj->IsHostPlayer())
				{
					// z축 이동
					if (pData[5] == '0')
					{
						Math::Vector3 pos = pObj->GetRootComponent().lock()->GetLocalPosition();
						pObj->GetRootComponent().lock()->SetLocalPosition({ pos.x, pos.y, pos.z + 50.f });
					}
				}
			}
			// 게스트 서버에서 플레이어가 움직였다
			else
			{
				if (!WorldManager::GetInstance()->IsHostServer() && pObj->IsHostPlayer())
				{
					// z축 이동
					if (pData[5] == '0')
					{
						Math::Vector3 pos = pObj->GetRootComponent().lock()->GetLocalPosition();
						pObj->GetRootComponent().lock()->SetLocalPosition({ pos.x, pos.y, pos.z + 50.f });
					}
				}
				else if (WorldManager::GetInstance()->IsHostServer() && !pObj->IsHostPlayer())
				{
					// z축 이동
					if (pData[5] == '0')
					{
						Math::Vector3 pos = pObj->GetRootComponent().lock()->GetLocalPosition();
						pObj->GetRootComponent().lock()->SetLocalPosition({ pos.x, pos.y, pos.z + 50.f });
					}
				}
			}
		}
	}
	delete[] pData;
}

void TestWorld::OnEnter()
{
	InputManager::GetInstance()->AddInputProcesser(this);
	World::OnEnter();
}

void TestWorld::OnExit()
{
	InputManager::GetInstance()->RemoveInputProcesser(this);

	World::OnExit();
}

void TestWorld::HandleEvent(Event* event)
{
	switch (event->eventID)
	{
	case eEventType::CHANGE_WORLD:
		WorldManager::GetInstance()->ChangeWorld(event->GetData<eWorldType>());
		break;
	case eEventType::DELETE_OBJECT:
		// 해당 타입에서 방금 생성한 오브젝트 삭제하는 테스트 코드
		if (m_gameObjects[static_cast<int>(event->GetData<eObjectType>())].size() <= 0)
			break;
		std::string objName = m_gameObjects[static_cast<int>(event->GetData<eObjectType>())][m_gameObjects[static_cast<int>(event->GetData<eObjectType>())].size() - 1]->GetName();
		if (!objName.empty())
			DeleteGameObject(objName);
		break;
	}
}

void TestWorld::OnInputProcess(const Keyboard::State& KeyState, const Keyboard::KeyboardStateTracker& KeyTracker,
	const Mouse::State& MouseState, const Mouse::ButtonStateTracker& MouseTracker)
{
	if (m_bTurn)
	{
		if (KeyTracker.IsKeyPressed(DirectX::Keyboard::Keys::Up))
		{
			CreateGameObjectRuntime<StaticTestObject>("StaticMesh", eObjectType::PLAYER);
			CreateGameObjectRuntime<PlayerObject>("SkeletalMesh", eObjectType::PLAYER);
		}
		if (KeyTracker.IsKeyPressed(DirectX::Keyboard::Keys::Down))
		{
			// 해당 타입에서 방금 생성한 오브젝트 삭제하는 테스트 코드
			EventManager::GetInstance()->SendEvent(eEventType::DELETE_OBJECT, this, eObjectType::PLAYER);
			EventManager::GetInstance()->SendEvent(eEventType::DELETE_OBJECT, this, eObjectType::PLAYER);
		}

		if (KeyTracker.IsKeyPressed(DirectX::Keyboard::Keys::Enter))
		{
			// 턴 넘겨주기
			WorldManager::GetInstance()->PushSendQueue(
				WorldManager::GetInstance()->SerializeBuffer(sizeof(PacketC2S_ChangeTurn), C2S_CHANGE_TURN, nullptr),
				sizeof(PacketC2S_ChangeTurn));
		}
	}

	if (KeyTracker.IsKeyPressed(DirectX::Keyboard::Keys::Delete))
	{
		// 준비 버튼
		char msg[1] = { '1' };

		WorldManager::GetInstance()->PushSendQueue(
			WorldManager::GetInstance()->SerializeBuffer(sizeof(PacketC2S_READY), C2S_READY, msg),
			sizeof(PacketC2S_READY));
	}

	if (KeyTracker.IsKeyPressed(DirectX::Keyboard::O))
	{
		Math::Vector3 xyz;
		for (auto& object : m_gameObjects[static_cast<int>(eObjectType::PLAYER)])
		{
			std::shared_ptr<PlayerObject> pObj = std::dynamic_pointer_cast<PlayerObject>(object);
			if (pObj != nullptr)
			{
				if (pObj->IsHostPlayer())
				{
					xyz = pObj->GetRootComponent().lock()->GetLocalPosition();

					if (WorldManager::GetInstance()->IsHostServer())
					{
						// todo 채원 : 고쳐
						char msg[2] = { '0', '0' };
						WorldManager::GetInstance()->PushSendQueue(WorldManager::GetInstance()->SerializeBuffer(
							sizeof(PacketC2S_CharacterMove), C2S_CHARACTER_MOVE, msg),
							sizeof(PacketC2S_CharacterMove));
					}
					else
					{
						char msg[2] = { '1', '0' };
						WorldManager::GetInstance()->PushSendQueue(WorldManager::GetInstance()->SerializeBuffer(
							sizeof(PacketC2S_CharacterMove), C2S_CHARACTER_MOVE, msg),
							sizeof(PacketC2S_CharacterMove));
					}
				}
			}
		}
	}
}
