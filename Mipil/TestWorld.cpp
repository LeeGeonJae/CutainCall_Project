#include "pch.h"
#include "TestWorld.h"

#include "StaticTestObject.h"
#include "TestPlayerObject.h"
#include "CubeMapObject.h"
#include "BlockGenerator.h"
#include "GridManager.h"

#include "../Engine/EventManager.h"
#include "../Engine/GameObject.h"
#include "../Engine/CameraComponent.h"
#include "../Engine/ControllerComponent.h"
#include "../Engine/FPSCameraController.h"
#include "../Engine/Model.h"
#include "../Engine/WorldManager.h"
#include "../Engine/SoundManager.h"
#include "../Engine/UITest.h"
#include "../Engine/UIText.h"
#include "../Engine/UITriggerPopUp.h"
#include "../Engine/UIFunctorPopUp.h"
#include "../Engine/UIHoverPopUpFunctor.h"
#include "../Engine/UIClickPopUpFunctor.h"
#include "StaticFbxObject.h"
#include "SkeletalFbxObject.h"
#include "UIMeshTestObject.h"

#include "../NetworkLibrary/MyProtocol.h"
#include "../D3DRenderer/UIInstance.h"

#include <memory>
#include <string>


TestWorld::TestWorld()
{
	EventManager::GetInstance()->RegisterListener(eEventType::CHANGE_WORLD, this);
	EventManager::GetInstance()->RegisterListener(eEventType::DELETE_OBJECT, this);

	WorldManager::GetInstance()->RegisterHandler(S2C_SET_TURN, std::bind(&TestWorld::SetTurn, this, std::placeholders::_1));
	WorldManager::GetInstance()->RegisterHandler(S2C_START_TURN, std::bind(&TestWorld::StartTurn, this, std::placeholders::_1));
	WorldManager::GetInstance()->RegisterHandler(S2C_END_TURN, std::bind(&TestWorld::EndTurn, this, std::placeholders::_1));
	WorldManager::GetInstance()->RegisterHandler(S2C_CHANGE_TURN, std::bind(&TestWorld::ChangeTurn, this, std::placeholders::_1));
	WorldManager::GetInstance()->RegisterHandler(S2C_IS_ALL_READY, std::bind(&TestWorld::ClientsAllReady, this, std::placeholders::_1));
	WorldManager::GetInstance()->RegisterHandler(S2C_CHARACTER_MOVE, std::bind(&TestWorld::CharacterMove, this, std::placeholders::_1));
}

TestWorld::~TestWorld()
{
	EventManager::GetInstance()->UnregisterListener(eEventType::CHANGE_WORLD, this);
	EventManager::GetInstance()->UnregisterListener(eEventType::DELETE_OBJECT, this);
}

void TestWorld::Initialize()
{
	m_Camera = CreateGameObject<GameObject>("Test_Camera", eObjectType::CAMERA).lock();
	auto cameraComponent = m_Camera->CreateComponent<CameraComponent>("Test_Camera_Component");
	cameraComponent.lock()->SetLocalPosition({ 50.f, 100.f, -500.f });
	m_Camera->SetRootComponent(cameraComponent.lock());
	auto controllerComponent = m_Camera->CreateComponent<ControllerComponent>("Test_Controller_Component");
	controllerComponent.lock()->CreateController<FPSCameraController>();

	auto TestPopUpUI = m_UIManager->CreateUI<UIFunctorPopUp>(L"UITestPopUp");
	TestPopUpUI->SetTexturePath(L"../Resources/Textures/testpanel.png");
	TestPopUpUI->SetSize({ 400.f, 400.f });
	TestPopUpUI->SetPosition({ 0.f, 0.f });

	auto PopUpFunctor = std::make_shared<UIClickPopUpFunctor>();
	PopUpFunctor->SetTargetUI(TestPopUpUI);

	auto TestUI = m_UIManager->CreateUI<UITest>(L"TestUI");
	TestUI->SetTexturePath(L"../Resources/Textures/Snake.bmp");
	TestUI->SetSize({ 100.f, 100.f });
	TestUI->SetPosition({ -700.f, -300.f });
	TestUI->SetScale({ 1.5f, 1.5f });
	TestUI->SetMouseClickFunctor(std::move(PopUpFunctor));

	auto FadeInTestUI = m_UIManager->CreateUI<UITest>(L"FadeInTestUI");
	FadeInTestUI->SetSize({ 1920.f, 1080.f });
	FadeInTestUI->SetPosition({ 0.f, 0.f });
	FadeInTestUI->SetScale({ 1.0f, 1.0f });
	m_tFadeInOut = FadeInTestUI;

	auto TestTextUI1 = m_UIManager->CreateUI<UIText, void>(L"TestTextUI1");
	TestTextUI1->SetPosition({ -50.f, 0.f });
	TestTextUI1->SetFont(L"�޸�����ü");
	TestTextUI1->SetFontSize(30.f);
	TestTextUI1->SetColor(D2D1::ColorF::Lavender);
	TestTextUI1->SetText(L"�ǳ�?");
	auto TestTextUI2 = m_UIManager->CreateUI<UIText, void>(L"TestTextUI2");
	TestTextUI2->SetPosition({ 50.f, 0.f });
	TestTextUI2->SetFont(L"�޸�����ü");
	TestTextUI2->SetFontSize(30.f);
	TestTextUI2->SetColor(D2D1::ColorF::Maroon);
	TestTextUI2->SetText(L"�ǳ�!");
	auto TestTextUI3 = m_UIManager->CreateUI<UIText, void>(L"TestTextUI3");
	TestTextUI3->SetPosition({ 0.f, 50.f });
	TestTextUI3->SetFont(L"Arial");
	TestTextUI3->SetFontSize(30.f);
	TestTextUI3->SetColor(D2D1::ColorF::MediumSeaGreen);
	TestTextUI3->SetText(L"��¥ �ǳ�?");
	auto TestTextUI4 = m_UIManager->CreateUI<UIText, void>(L"TestTextUI4");
	TestTextUI4->SetPosition({ 0.f, -50.f });
	TestTextUI4->SetFont(L"Arial");
	TestTextUI4->SetFontSize(30.f);
	TestTextUI4->SetColor(D2D1::ColorF::Peru);
	TestTextUI4->SetText(L"��¥ �ǳ�!");
	TestPopUpUI->AddChildren(TestTextUI1, TestTextUI2, TestTextUI3, TestTextUI4);

	//SoundManager::GetInstance()->LoadSound("../Resources/Sound/bgm.mp3", FMOD_LOOP_NORMAL);
	//SoundManager::GetInstance()->LoadSound("../Resources/Sound/jump.mp3");
	//SoundManager::GetInstance()->SetVolume("../Resources/Sound/jump.mp3", 1.5f);
	//SoundManager::GetInstance()->LoadSound("../Resources/Sound/effect.mp3");
	//SoundManager::GetInstance()->SetVolume("../Resources/Sound/effect.mp3", 0.1f);
	//SoundManager::GetInstance()->PlaySound("../Resources/Sound/bgm.mp3");
	//SoundManager::GetInstance()->SetVolume("../Resources/Sound/bgm.mp3", 0.1f);

	std::shared_ptr<TestPlayerObject> hostPlayer = CreateGameObject<TestPlayerObject>("HostPlayer", eObjectType::PLAYER).lock();
//	std::shared_ptr<TestPlayerObject> guestPlayer = CreateGameObject<TestPlayerObject>("GuestPlayer", eObjectType::PLAYER).lock();

//	std::shared_ptr<StaticTestObject> TestObject = CreateGameObject<StaticTestObject>("TestObject", eObjectType::TEST).lock();
	std::shared_ptr<UIMeshTestObject> uiMeshObject = CreateGameObject<UIMeshTestObject>("UIMeshObject", eObjectType::TEST).lock();

	//if (WorldManager::GetInstance()->IsHostServer())
	//	hostPlayer->SetHostPlayer(true);
	//else guestPlayer->SetHostPlayer(true);

	// Test FBX Object
	//std::shared_ptr<StaticFbxObject> fbxTest = CreateGameObject<StaticFbxObject>("FBX_TEST", eObjectType::TEST).lock();
	//std::shared_ptr<SkeletalFbxObject> fbxBearTest = CreateGameObject<SkeletalFbxObject>("FBX_TEST", eObjectType::TEST).lock();

	// CubeMap
	CreateGameObject<CubeMapObject>("CubeMap", eObjectType::TEST);

	// BlockGenerator
	std::shared_ptr<BlockGenerator> blockGenerator = std::make_shared<BlockGenerator>(shared_from_this(), "../Resources/Map/block_test2.yaml");
	blockGenerator->Generate();

	//CommonApp::m_pInstance->GetRenderer()->SetProjectionMatrix(cameraComponent.lock()->GetProjectionMatrix());

	LOG_CONTENTS("TestWorld1 Init");

	/*PacketC2S_SetTurn setTurn;
	setTurn.who = '0';
	WorldManager::GetInstance()->PushSendQueue(WorldManager::GetInstance()->SerializeBuffer(
		sizeof(PacketC2S_SetTurn), C2S_SET_TURN, &setTurn.who),
		sizeof(PacketC2S_SetTurn));*/

	__super::Initialize();

	/// --------------------------------------------
	/// GridManager
	m_gridManager = std::make_shared<GridManager>();
	m_gridManager->CreateMap(10, 10, 5, 0.f, 0.f, 0.f, 100.f);
	m_gridManager->SetMapState(GetGameObjects(eObjectType::LEVEL));
	m_gridManager->SetMapState(GetGameObjects(eObjectType::PLAYER));


//	TestObject->GetRootComponent().lock()->SetLocalPosition({ -100.f, 0.f, 0.f });
//	TestObject->GetRootComponent().lock()->SetLocalScale({ 1.f, 1.f, 1.f });

	// ����
	//fbxTest->GetRootComponent().lock()->SetLocalPosition({ 150.f, 0.f, 50.f });
	//fbxTest->GetRootComponent().lock()->SetLocalScale({ 1.f, 1.f, 1.f });

	//fbxBearTest->GetRootComponent().lock()->SetLocalPosition({ -150.f, 0.f, -50.f });
	//fbxBearTest->GetRootComponent().lock()->SetLocalScale({ 1.f, 1.f, 1.f });

	// ���� : UI �ִϸ��̼� �׽�Ʈ
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
			keyframe.m_AnimationTime = 0.2f;
			TestUI->GetUIInstance().lock()->SetIsLoop(false);
			TestUI->AddKeyFrame(keyframe);
		}

		//
		FadeInTestUI->GetUIInstance().lock()->SetIsFade();
		FadeInTestUI->GetUIInstance().lock()->SetFadeDurationTime(1.5f);
	}

	m_bGameRun = false;
	m_bTurn = false;
	m_fixedDelta = 0.0f;
}

void TestWorld::Update(float deltaTime)
{
	World::Update(deltaTime);

	if (GetAsyncKeyState(VK_F2) & 0x8000)
		EventManager::GetInstance()->SendEvent(eEventType::CHANGE_WORLD, this, eWorldType::TEST2);

	if (GetAsyncKeyState(VK_F3) & 0x8000)
		EventManager::GetInstance()->SendEvent(eEventType::CHANGE_WORLD, this, eWorldType::TEST3);

	if (m_tFadeInOut != nullptr)
	{
		m_tFadeInOut->GetUIInstance().lock()->UpdateFadeInOut(deltaTime);
	}

	// ��� Ŭ�� ������!
	if (m_bGameRun)
	{
		m_fixedDelta += deltaTime;
		if (m_fixedDelta > 5.f)
		{
			EventManager::GetInstance()->SendEvent(eEventType::CHANGE_WORLD, this, eWorldType::TEST2);
		}

	}
}

// �� �������ֱ�
void TestWorld::SetTurn(char* pData)
{
	if (pData[4] == '0')
	{
		m_bTurn = true;
		printf("1P ���̴�\n");
	}
	else if (pData[4] == '1')
	{
		m_bTurn = true;
		printf("2P ���̴�\n");
	}

	delete[] pData;
}

void TestWorld::StartTurn(char* pData)
{
	delete[] pData;
	// ���� ������ BroadCast�ϰ� ����.
	char* broadCastTurn = new char[SND_BUF_SIZE];
	const char* msg = "1P Turn";
	memcpy(broadCastTurn, msg, SND_BUF_SIZE);

	WorldManager::GetInstance()->PushSendQueue(WorldManager::GetInstance()->SerializeBuffer(
		sizeof(PacketC2S_BroadcastMsg), C2S_BROADCAST_MSG, broadCastTurn),
		sizeof(PacketC2S_BroadcastMsg));
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
	PacketS2C_IsAllReady* pAllReady = reinterpret_cast<PacketS2C_IsAllReady*>(pData);
	assert(pAllReady != nullptr);

	m_bGameRun = pAllReady->isReady == '0' ? false : true;

	if (m_bGameRun)
	{
		auto ui = m_UIManager->GetUIObject<UITriggerPopUp>(L"UITestPopUp");
		//ui.lock()->TriggerPop();
	}
	delete[] pData;
}

void TestWorld::CharacterMove(char* pData)
{
	PacketS2C_CharacterMove* pCharacterMove = reinterpret_cast<PacketS2C_CharacterMove*>(pData);
	assert(pCharacterMove != nullptr);

	for (auto& object : m_gameObjects[static_cast<int>(eObjectType::PLAYER)])
	{
		std::shared_ptr<TestPlayerObject> pObj = std::dynamic_pointer_cast<TestPlayerObject>(object);
		if (pObj != nullptr)
		{
			// ȣ��Ʈ �������� �÷��̾ ��������
			if (pCharacterMove->who == '0')
			{
				if (WorldManager::GetInstance()->IsHostServer() && pObj->IsHostPlayer())
				{
					// z�� �̵�
					if (pCharacterMove->direction == '0')
					{
						Math::Vector3 pos = pObj->GetRootComponent().lock()->GetLocalPosition();
						pObj->GetRootComponent().lock()->SetLocalPosition({ pos.x, pos.y, pos.z + 50.f });
					}
				}
				else if (!WorldManager::GetInstance()->IsHostServer() && !pObj->IsHostPlayer())
				{
					// z�� �̵�
					if (pCharacterMove->direction == '0')
					{
						Math::Vector3 pos = pObj->GetRootComponent().lock()->GetLocalPosition();
						pObj->GetRootComponent().lock()->SetLocalPosition({ pos.x, pos.y, pos.z + 50.f });
					}
				}
			}
			// �Խ�Ʈ �������� �÷��̾ ��������
			else
			{
				if (!WorldManager::GetInstance()->IsHostServer() && pObj->IsHostPlayer())
				{
					// z�� �̵�
					if (pCharacterMove->direction == '0')
					{
						Math::Vector3 pos = pObj->GetRootComponent().lock()->GetLocalPosition();
						pObj->GetRootComponent().lock()->SetLocalPosition({ pos.x, pos.y, pos.z + 50.f });
					}
				}
				else if (WorldManager::GetInstance()->IsHostServer() && !pObj->IsHostPlayer())
				{
					// z�� �̵�
					if (pCharacterMove->direction == '0')
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

void TestWorld::MovePlayer(TestPlayerObject* player, Vector3 vec)
{
	m_gridManager->MoveOnGrid(player, vec);
}

bool TestWorld::IsGround(Vector3 pos)
{
	return m_gridManager->IsGround(pos);
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
		// �ش� Ÿ�Կ��� ��� ������ ������Ʈ �����ϴ� �׽�Ʈ �ڵ�
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
			CreateGameObjectRuntime<TestPlayerObject>("SkeletalMesh", eObjectType::PLAYER);
		}
		if (KeyTracker.IsKeyPressed(DirectX::Keyboard::Keys::Down))
		{
			// �ش� Ÿ�Կ��� ��� ������ ������Ʈ �����ϴ� �׽�Ʈ �ڵ�
			EventManager::GetInstance()->SendEvent(eEventType::DELETE_OBJECT, this, eObjectType::PLAYER);
			EventManager::GetInstance()->SendEvent(eEventType::DELETE_OBJECT, this, eObjectType::PLAYER);
		}

		if (KeyTracker.IsKeyPressed(DirectX::Keyboard::Keys::Enter))
		{
			// �� �Ѱ��ֱ�
			WorldManager::GetInstance()->PushSendQueue(
				WorldManager::GetInstance()->SerializeBuffer(sizeof(PacketC2S_ChangeTurn), C2S_CHANGE_TURN, nullptr),
				sizeof(PacketC2S_ChangeTurn));
		}
	}

	// ���� : ���̵� �� �ƿ� �׽�Ʈ
	if (KeyTracker.IsKeyPressed(DirectX::Keyboard::Keys::Z))
	{
		m_tFadeInOut->GetUIInstance().lock()->FadeOutStart();
	}
	if (KeyTracker.IsKeyPressed(DirectX::Keyboard::Keys::X))
	{
		m_tFadeInOut->GetUIInstance().lock()->FadeInStart();
	}

	if (KeyTracker.IsKeyPressed(DirectX::Keyboard::Keys::Delete))
	{
		// �غ� ��ư
		PacketC2S_READY ready;
		ready.clickedReady = '1';

		WorldManager::GetInstance()->PushSendQueue(
			WorldManager::GetInstance()->SerializeBuffer(sizeof(PacketC2S_READY), C2S_READY, &ready.clickedReady),
			sizeof(PacketC2S_READY));
	}

	if (KeyTracker.IsKeyPressed(DirectX::Keyboard::Keys::Space))
	{
		SoundManager::GetInstance()->PlaySound("../Resources/Sound/jump.mp3");
	}

	if (KeyTracker.IsKeyPressed(DirectX::Keyboard::Keys::V))
	{
		SoundManager::GetInstance()->PlaySound("../Resources/Sound/effect.mp3");
	}

	if (KeyTracker.IsKeyPressed(DirectX::Keyboard::Keys::B))				// ���͸��� �ٲ�� �� �׽�Ʈ�� �ڵ�
	{
		weak_ptr<UIMeshTestObject> object = dynamic_pointer_cast<UIMeshTestObject>(GetGameObject("UIMeshObject"));
		object.lock()->ChangedMaterial();
	}
	if (KeyTracker.IsKeyPressed(DirectX::Keyboard::Keys::N))				// ���͸��� �ٲ�� �� �׽�Ʈ�� �ڵ�
	{
		weak_ptr<UIMeshTestObject> object = dynamic_pointer_cast<UIMeshTestObject>(GetGameObject("UIMeshObject"));
		object.lock()->PlayParticle();
		object.lock()->ChangedUIMaterial();
	}

}
