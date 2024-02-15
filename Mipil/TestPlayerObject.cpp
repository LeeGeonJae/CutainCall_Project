#include "pch.h"
#include "TestPlayerObject.h"

#include "GridManager.h"
#include "PlayerFSM.h"
#include "TestWorld.h"

#include "../Engine/SkeletalMeshComponent.h"
#include "../Engine/FSMComponent.h"
#include "../Engine/EventManager.h"
#include "../Engine/WorldManager.h"
#include "../NetworkLibrary/MyProtocol.h"

TestPlayerObject::TestPlayerObject()
{
	InputManager::GetInstance()->AddInputProcesser(this);

	EventManager::GetInstance()->RegisterListener(eEventType::CHANGE_ANIMATION, this);
	EventManager::GetInstance()->RegisterListener(eEventType::MOVE_ON_GRID, this);

	//WorldManager::GetInstance()->RegisterHandler(S2C_SET_TURN, std::bind(&TestPlayerObject::SetTurn, this, std::placeholders::_1));
}

TestPlayerObject::~TestPlayerObject()
{
	InputManager::GetInstance()->RemoveInputProcesser(this);

	EventManager::GetInstance()->UnregisterListener(eEventType::CHANGE_ANIMATION, this);
	EventManager::GetInstance()->UnregisterListener(eEventType::MOVE_ON_GRID, this);

	/// GridManager
	/*m_gridManager = std::make_shared<GridManager>();
	m_gridManager->CreateMap(10, 10, 5, -900.f, 0.f, -900.f, 200.f);
	m_gridManager->SetMapState(GetGameObjects(eObjectType::LEVEL));
	std::shared_ptr<GameObject> sharedThis = shared_from_this();
	m_gridManager->SetMapState(sharedThis);*/
}

void TestPlayerObject::Initialize()
{
	std::shared_ptr<PlayerFSM> fsm = std::make_shared<PlayerFSM>();

	m_meshComponent = CreateComponent<SkeletalMeshComponent>("SkeletalMeshComponent").lock();
	m_meshComponent->SetDefaultAnimation(fsm->GetAnimationName("State_Idle"));
	m_meshComponent->AddAnimation(fsm->GetAnimationName("State_TurnWait"));
	m_meshComponent->SetLocalPosition({ 50.f, 0.f, 50.f });
	SetRootComponent(m_meshComponent);

	m_fsmComponent = CreateComponent<FSMComponent>("SkeletalFSMComponent").lock();
	fsm->SetOwnerObject(this);
	m_fsmComponent->AttachFSM(fsm);

	m_bTurn = false;

	GameObject::Initialize();
}

void TestPlayerObject::Update(float deltaTime)
{
	GameObject::Update(deltaTime);
}

void TestPlayerObject::OnInputProcess(const Keyboard::State& KeyState, const Keyboard::KeyboardStateTracker& KeyTracker, const Mouse::State& MouseState, const Mouse::ButtonStateTracker& MouseTracker)
{
	TestWorld* world = dynamic_cast<TestWorld*>(m_pOwnerWorld.lock().get());
	if (!world)
		return;

	if (KeyTracker.IsKeyPressed(DirectX::Keyboard::Enter))
	{
		m_bTurn = !m_bTurn;
		EventManager::GetInstance()->SendEvent(eEventType::CHANGE_TURN, nullptr, m_bTurn);
	}

	if (KeyTracker.IsKeyPressed(DirectX::Keyboard::W))
	{
		world->MovePlayer(this, grid::Z_UP);
		EventManager::GetInstance()->SendEvent(eEventType::GET_THIS_ACTION, nullptr, grid::Z_UP);
		m_bTurn = true;
		EventManager::GetInstance()->SendEvent(eEventType::CHANGE_TURN, nullptr, m_bTurn);
		EventManager::GetInstance()->SendEvent(eEventType::CHANGE_TURN, nullptr, world->IsGround(GetPosition()));
	}
	if (KeyTracker.IsKeyPressed(DirectX::Keyboard::S))
	{
		world->MovePlayer(this, grid::Z_DOWN);
		EventManager::GetInstance()->SendEvent(eEventType::GET_THIS_ACTION, nullptr, grid::Z_DOWN);
		m_bTurn = true;
		EventManager::GetInstance()->SendEvent(eEventType::CHANGE_TURN, nullptr, m_bTurn);
		EventManager::GetInstance()->SendEvent(eEventType::CHANGE_TURN, nullptr, world->IsGround(GetPosition()));
	}
	if (KeyTracker.IsKeyPressed(DirectX::Keyboard::D))
	{
		world->MovePlayer(this, grid::X_UP);
		EventManager::GetInstance()->SendEvent(eEventType::GET_THIS_ACTION, nullptr, grid::X_UP);
		m_bTurn = true;
		EventManager::GetInstance()->SendEvent(eEventType::CHANGE_TURN, nullptr, m_bTurn);
		EventManager::GetInstance()->SendEvent(eEventType::CHANGE_TURN, nullptr, world->IsGround(GetPosition()));
	}
	if (KeyTracker.IsKeyPressed(DirectX::Keyboard::A))
	{
		world->MovePlayer(this, grid::X_DOWN);
		EventManager::GetInstance()->SendEvent(eEventType::GET_THIS_ACTION, nullptr, grid::X_DOWN);
		m_bTurn = true;
		EventManager::GetInstance()->SendEvent(eEventType::CHANGE_TURN, nullptr, m_bTurn);
		EventManager::GetInstance()->SendEvent(eEventType::CHANGE_TURN, nullptr, world->IsGround(GetPosition()));
	}
	if (KeyTracker.IsKeyPressed(DirectX::Keyboard::Q))
	{
		world->MovePlayer(this, grid::Y_UP);
		EventManager::GetInstance()->SendEvent(eEventType::GET_THIS_ACTION, nullptr, grid::Y_UP);
		m_bTurn = true;
		EventManager::GetInstance()->SendEvent(eEventType::CHANGE_TURN, nullptr, m_bTurn);
		EventManager::GetInstance()->SendEvent(eEventType::CHANGE_TURN, nullptr, world->IsGround(GetPosition()));
	}
	if (KeyTracker.IsKeyPressed(DirectX::Keyboard::E))
	{
		world->MovePlayer(this, grid::STAY);
		EventManager::GetInstance()->SendEvent(eEventType::GET_THIS_ACTION, nullptr, grid::STAY);
		m_bTurn = true;
		EventManager::GetInstance()->SendEvent(eEventType::CHANGE_TURN, nullptr, m_bTurn);
		EventManager::GetInstance()->SendEvent(eEventType::CHANGE_TURN, nullptr, world->IsGround(GetPosition()));
	}
}

void TestPlayerObject::HandleEvent(Event* event)
{
	switch (event->eventID)
	{
	case eEventType::CHANGE_ANIMATION:
		m_meshComponent->ChangeAnimation(event->GetData<const char*>());
		return;
	case eEventType::MOVE_ON_GRID:
		SetPosition(GetPosition() + event->GetData<Vector3>());	// TEST : 후에 MovementComponent 통해 바꾸는 것으로 수정
		return;
	default:
		return;
	}
}

void TestPlayerObject::ProceedAction()
{
	if (m_bHostPlayer)		// host가 움직
	{
		PacketC2S_CharacterMove cm;
		cm.who = '0'; // host가 움직이려함.
		cm.direction = '0'; // 일단 Z_UP

		WorldManager::GetInstance()->PushSendQueue(
			WorldManager::GetInstance()->SerializeBuffer(sizeof(PacketC2S_CharacterMove), C2S_CHARACTER_MOVE, &cm.who),
			sizeof(PacketC2S_CharacterMove));
	}

	else if (!m_bHostPlayer)	// guest가 움직
	{
		PacketC2S_CharacterMove cm;
		cm.who = '1'; // guest가 움직이려함.
		cm.direction = '0'; // 일단 Z_DOWN

		WorldManager::GetInstance()->PushSendQueue(
			WorldManager::GetInstance()->SerializeBuffer(sizeof(PacketC2S_CharacterMove), C2S_CHARACTER_MOVE, &cm.who),
			sizeof(PacketC2S_CharacterMove));
	}
	m_index++;
}

void TestPlayerObject::CheckAction()
{
	// 끝내기
	if (m_index >= 4)
	{
		WorldManager::GetInstance()->PushSendQueue(
			WorldManager::GetInstance()->SerializeBuffer(sizeof(PacketC2S_EndAction), C2S_END_ACTION, nullptr),
			sizeof(PacketC2S_EndAction));
	}
}
void TestPlayerObject::ChangeTurn()
{
	// 턴 체인지 해주기
	PacketC2S_SetTurn st;

	if (m_bTurn)
	{
		if (m_bHostPlayer)	// 호스트가 턴일 때 게스트 턴으로 SetTurn해주기
		{
			st.who = '1';
			WorldManager::GetInstance()->PushSendQueue(
				WorldManager::GetInstance()->SerializeBuffer(sizeof(PacketC2S_SetTurn), C2S_SET_TURN, &st.who),
				sizeof(PacketC2S_SetTurn));
		}
		else
		{
			st.who = '0';
			WorldManager::GetInstance()->PushSendQueue(
				WorldManager::GetInstance()->SerializeBuffer(sizeof(PacketC2S_SetTurn), C2S_SET_TURN, &st.who),
				sizeof(PacketC2S_SetTurn));
		}
	}
}

