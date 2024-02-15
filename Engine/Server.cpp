#include "pch.h"
#include "Server.h"

#include "Client.h"
#include "Session.h"

#include "../NetworkLibrary/ClientSocket.h"
#include "../NetworkLibrary/ServerSocket.h"
#include "../NetworkLibrary/WinSock.h"
#include "../Engine/WorldManager.h"

void ServerNetworkManager::Start(const char* add)
{
	WSAData wsaData;

	if (::WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		return;

	m_pListenSocket = std::make_shared<ServerSocket>();

	if (!m_pListenSocket->Create(SOCK_STREAM, FD_ACCEPT)
		|| !m_pListenSocket->Bind(7777, add)
		|| !m_pListenSocket->Listen())
		return;

	// 종료이벤트 핸들
	m_hQuitEvent = CreateEvent(NULL, false, false, NULL);
	m_bRun = true;
}

void ServerNetworkManager::Stop()
{
	if (m_hQuitEvent != INVALID_HANDLE_VALUE)
		SetEvent(m_hQuitEvent);
}

void ServerNetworkManager::CleanUp()
{
	if (m_hQuitEvent != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hQuitEvent);
	}
	::WSACleanup();
}

void ServerNetworkManager::Update()
{
	std::vector<WSAEVENT> wsaEvents;

	wsaEvents.push_back(m_pListenSocket->GetEvent());

	for (auto& pClient : m_peerClients)
	{
		if (pClient == nullptr)
		{

		}
		else
		{
			std::shared_ptr<ClientSocket> soc = pClient->GetSocket();
			wsaEvents.push_back(soc->GetEvent());
		}
	}

	if (WAIT_TIMEOUT != WaitForSingleObject(m_hQuitEvent, 0))
	{
		m_bRun = false;
		return;
	}

	int index = ::WSAWaitForMultipleEvents(wsaEvents.size(), &wsaEvents[0], FALSE, 1, FALSE);

	if (index == WSA_WAIT_FAILED)
	{
		printf("WSAWaitForMultipleEvents Error %d", WSAGetLastError());
		return;
	}

	if (index == WSA_WAIT_TIMEOUT) return;

	index -= WSA_WAIT_EVENT_0;

	std::shared_ptr<WinSock> pSocket = nullptr;

	if (wsaEvents[index] == m_pListenSocket->GetEvent())
		pSocket = m_pListenSocket;
	else 
		pSocket = m_peerClients[index - 1]->GetSocket();

	WSANETWORKEVENTS networkEvents;

	if (::WSAEnumNetworkEvents(pSocket->GetHandle(), wsaEvents[index], &networkEvents) == SOCKET_ERROR)
	{
		printf("WSAEnumNetworkEvents Error %d", WSAGetLastError());

		return;
	}

	if (networkEvents.lNetworkEvents & FD_ACCEPT)
	{
		if (networkEvents.iErrorCode[FD_ACCEPT_BIT] != 0)
		{
			onNetError(networkEvents.iErrorCode[FD_ACCEPT_BIT], "Accept");
			return;
		}

		OnAccept();
	}

	if (networkEvents.lNetworkEvents & FD_READ)
	{
		if (networkEvents.iErrorCode[FD_READ_BIT] != 0)
		{
			onNetError(networkEvents.iErrorCode[FD_READ_BIT], "Recv", pSocket);
			return;
		}

		OnReceive(m_peerClients[index - 1]);
	}

	if (networkEvents.lNetworkEvents & FD_WRITE)
	{
		if (networkEvents.iErrorCode[FD_WRITE_BIT] != 0)
		{
			onNetError(networkEvents.iErrorCode[FD_WRITE_BIT], "Write", pSocket);
			return;
		}

		OnSend(pSocket);
	}

	if (networkEvents.lNetworkEvents & FD_CLOSE)
	{
		if (networkEvents.iErrorCode[FD_CLOSE_BIT] != 0)
		{
			onNetError(networkEvents.iErrorCode[FD_CLOSE_BIT], "Close", pSocket);
		}
		OnClose(m_peerClients[index - 1]);
	}
}

void ServerNetworkManager::NetUpdate()
{
	for (auto& session : m_sessions)
	{
		std::pair<char*, int> recvValue;
		while (session.second->PopRecvQueue(recvValue))
		{
			CheckId(session.second, recvValue.first, recvValue.second);
			delete[] recvValue.first;
		}

		std::pair<char*, int> sendValue;
		while (session.second->PopSendQueue(sendValue))
		{
			int nSent = session.second->Send(sendValue.first, sendValue.second);
			delete[] sendValue.first;
		}
	}
}

void ServerNetworkManager::OnAccept()
{
	std::shared_ptr<ClientNetworkManager> pClient = std::make_shared<ClientNetworkManager>();
	pClient->SetSocket(std::make_shared<ClientSocket>());
	std::shared_ptr<Session> pSession = std::make_shared<Session>();

	std::shared_ptr<ClientSocket> clientSocket = pClient->GetSocket();

	if (m_pListenSocket->OnAccept(clientSocket))
	{
		pSession->SetClient(pClient);

		m_peerClients.push_back(pClient);
		m_sessions[pSession->GetSessionId()] = pSession;

		++m_ClientCount;
	}
	else
	{
	}
}

void ServerNetworkManager::OnReceive(std::shared_ptr<ClientNetworkManager> pClient)
{
	if (pClient->GetSocket() == nullptr) return;

	printf("onReceive  %s : %d\n", pClient->GetSocket()->GetIP().c_str(), pClient->GetSocket()->GetPort());


	std::shared_ptr<Session> pSession = m_sessions[pClient->GetSessionId()];
	if (pSession == nullptr) return;

	pSession->ReadUpdate();

	if (pSession->IsRecvQueueEmpty())
	{
		return;
	}
}

void ServerNetworkManager::OnSend(std::shared_ptr<WinSock> pSocket)
{
	printf("onSend  %s : %d\n", pSocket->GetIP().c_str(), pSocket->GetPort());
}

void ServerNetworkManager::OnClose(std::shared_ptr<ClientNetworkManager> pClient)
{
	if (pClient->GetSocket() == nullptr) return;

	m_sessions[pClient->GetSessionId()].get()->Finalize();

	m_sessions.erase(pClient->GetSessionId());

	for (auto iter = m_peerClients.begin(); iter != m_peerClients.end(); ++iter)
	{
		if (iter->get() == pClient.get())
		{
			m_peerClients.erase(iter);
			break;
		}
	}
	m_ClientCount--;
	printf("연결된 클라이언트 수 : %d \n", m_ClientCount);
}

void ServerNetworkManager::onNetError(int errorCode, const char* errorMsg, std::shared_ptr<WinSock> pSocket)
{
	if (errorMsg)
	{
		printf("\n onNetError %s \t", errorMsg);
	}

	if (pSocket)
	{
		printf("\n onNetError  %s : %d", pSocket->GetIP().c_str(), pSocket->GetPort());
	}

	printf("NetErrorCode  %d  \n", errorCode);
}

char* ServerNetworkManager::SerializeBuffer(int size, EPacketId id, char* msg)
{
	if (id <= PACKETID_START || id >= PACKETID_END)
		return nullptr;

	char* buf = new char[SND_BUF_SIZE];

	PacketHeader ph;
	ph.size = size;
	ph.id = id;

	memcpy(buf, &ph, sizeof(PacketHeader));

	if (msg == nullptr)
		return buf;

	memcpy(buf + sizeof(PacketHeader), msg, size - sizeof(PacketHeader));

	return buf;
}
void ServerNetworkManager::DeSerializeBuffer(PacketHeader& packet, char* buf)
{
	short sizeInfo = 0;
	memcpy(&sizeInfo, buf, sizeof(short));

	EPacketId idInfo;
	memcpy(&idInfo, buf + sizeof(short), sizeof(EPacketId));

	packet.size = sizeInfo;
	packet.id = idInfo;
}

// 게임 로직들
void ServerNetworkManager::CheckId(std::shared_ptr<Session> session, char* pData, int len)
{
	PacketHeader pHeader;
	DeSerializeBuffer(pHeader, pData);

	switch (pHeader.id)
	{
	case C2S_READY:
		ClientSetReady(session, pData);
		IsAllReady();
		break;

	case C2S_IAM_HOST_MSG:
		session->SetHostSession();
		break;

	case C2S_SET_TURN:
		SetTurn(session, pData);
		break;

	case C2S_END_TURN:
		EndTurn(session, pData);
		break;

	case C2S_CHANGE_TURN:
		ChangeTurn();
		break;

	case C2S_START_TURN:
		StartTurn(session, pData);
		break;

	case C2S_CHECK_ACTION:
		BroadcastCheckAction();
		break;

	case C2S_BROADCAST_MSG:
		BroadcastMsg(session, pData);
		break;

	case C2S_CHARACTER_MOVE:
		SendCharacterPosition(session, pData);
		break;

	case C2S_END_ACTION:
		EndAction(session, pData);
		IsAllEnd();
		break;
	
	default:
		break;
	}
}

void ServerNetworkManager::BroadcastPacket(char* pData, int len)
{
	for (auto& session : m_sessions)
	{
		char* sendData = new char[SND_BUF_SIZE];
		if(pData != nullptr)
			memcpy(sendData, pData, SND_BUF_SIZE);
		session.second->PushSendQueue(sendData, len);
	}
	delete pData;
}

void ServerNetworkManager::BroadcastMsg(std::shared_ptr<Session> session, char* pMsg)
{
	char* broadcastMsg = new char[SND_BUF_SIZE];
	BroadcastPacket(SerializeBuffer(sizeof(PacketS2C_BroadcastMsg), S2C_BROADCAST_MSG, broadcastMsg), sizeof(PacketS2C_BroadcastMsg));
}

void ServerNetworkManager::ClientSetReady(std::shared_ptr<Session> session, char* pMsg)
{
	PacketC2S_READY* pSetReady = reinterpret_cast<PacketC2S_READY*>(pMsg);
	assert(pSetReady != nullptr);

	// 레디 풀기
	if (pSetReady->clickedReady - '0' == 0)
		session->SetReadyState(false);
	// 레디 하기
	else
		session->SetReadyState(true);
}

void ServerNetworkManager::SetTurn(std::shared_ptr<Session> session, char* pMsg)
{
	PacketC2S_SetTurn* pSetTurn = reinterpret_cast<PacketC2S_SetTurn*>(pMsg);
	assert(pSetTurn != nullptr);

	if (pSetTurn->who == '0') // 호스트 턴
	{
		PacketS2C_SetTurn st;
		st.setTurn = '0';
		session->PushSendQueue(
			SerializeBuffer(sizeof(PacketS2C_SetTurn), S2C_SET_TURN, &st.setTurn),
			sizeof(PacketS2C_SetTurn));
	}
	else if (pSetTurn->who == '1') // 세션이 게스트면 게스트한테 턴 세팅해주기
	{
		PacketS2C_SetTurn st;
		st.setTurn = '1';
		session->PushSendQueue(
			SerializeBuffer(sizeof(PacketS2C_SetTurn), S2C_SET_TURN, &st.setTurn),
			sizeof(PacketS2C_SetTurn));
	}
}

void ServerNetworkManager::EndTurn(std::shared_ptr<Session> session, char* pMsg)
{
	session->PushSendQueue(
		SerializeBuffer(sizeof(PacketS2C_EndTurn), S2C_END_TURN, nullptr),
		sizeof(PacketS2C_EndTurn));
}

void ServerNetworkManager::ChangeTurn()
{
	BroadcastPacket(SerializeBuffer(sizeof(PacketS2C_ChangeTurn), S2C_CHANGE_TURN, nullptr), sizeof(PacketS2C_ChangeTurn));
}

void ServerNetworkManager::StartTurn(std::shared_ptr<Session> session, char* pMsg)
{
	/*session->PushSendQueue(
		SerializeBuffer(sizeof(PacketS2C_StartGame), S2C_START_GAME, nullptr),
		sizeof(PacketS2C_StartGame));*/
	BroadcastPacket(SerializeBuffer(sizeof(PacketS2C_StartGame), S2C_START_TURN, nullptr),
		sizeof(PacketS2C_StartGame));
}

void ServerNetworkManager::BroadcastCheckAction()
{
	BroadcastPacket(SerializeBuffer(sizeof(PacketS2C_CheckAction), S2C_CHECK_ACTION, nullptr), sizeof(PacketS2C_CheckAction));
}

void ServerNetworkManager::SendCharacterPosition(std::shared_ptr<Session> session, char* pMsg)
{
	PacketC2S_CharacterMove* pCharacterMove = reinterpret_cast<PacketC2S_CharacterMove*>(pMsg);
	assert(pCharacterMove != nullptr);

	// 호스트 서버면
	if (pCharacterMove->who == '0')
	{
		PacketS2C_CharacterMove cm;
		cm.who = '0';		// host
		cm.direction = '0';	// z축으로

		session->PushSendQueue(
			SerializeBuffer(sizeof(PacketS2C_CharacterMove), S2C_CHARACTER_MOVE, &cm.who), sizeof(PacketS2C_CharacterMove));
		//BroadcastPacket(SerializeBuffer(sizeof(PacketS2C_CharacterMove), S2C_CHARACTER_MOVE, &cm.who), sizeof(PacketS2C_CharacterMove));
	}
	// 게스트 서버면
	else if(pCharacterMove->who == '1')
	{
		PacketS2C_CharacterMove cm;
		cm.who = '1';		// clinent
		cm.direction = '0';	// z축으로

		session->PushSendQueue(
			SerializeBuffer(sizeof(PacketS2C_CharacterMove), S2C_CHARACTER_MOVE, &cm.who), sizeof(PacketS2C_CharacterMove));
		//BroadcastPacket(SerializeBuffer(sizeof(PacketS2C_CharacterMove), S2C_CHARACTER_MOVE, &cm.who), sizeof(PacketS2C_CharacterMove));
	}
}

void ServerNetworkManager::EndAction(std::shared_ptr<Session> session, char* pMsg)
{
	session->SetEndState(true);
}

// 모두가 Ready인지 체크
void ServerNetworkManager::IsAllReady()
{
	for (auto& pClient : m_peerClients)
	{
		if (pClient == nullptr)
		{
			continue;
		}
		else
		{
			std::shared_ptr<ClientSocket> soc = pClient->GetSocket();
			std::shared_ptr<Session> pSession = m_sessions[pClient->GetSessionId()];

			if (pSession == nullptr) return;
			if (!pSession->GetReadyState())
			{
				char notReady[sizeof(char)] = { '0' };
				BroadcastPacket(SerializeBuffer(sizeof(PacketS2C_IsAllReady), S2C_IS_ALL_READY, notReady), sizeof(PacketS2C_IsAllReady));
				return;
			}
		}
	}

	char ready[sizeof(char)] = { '1' };
	BroadcastPacket(SerializeBuffer(sizeof(PacketS2C_IsAllReady), S2C_IS_ALL_READY, ready), sizeof(PacketS2C_IsAllReady));

	for (auto& pClient : m_peerClients)
	{
		std::shared_ptr<ClientSocket> soc = pClient->GetSocket();
		std::shared_ptr<Session> pSession = m_sessions[pClient->GetSessionId()];

		if (pSession == nullptr) return;

		pSession->SetReadyState(false);
	}
}

void ServerNetworkManager::IsAllEnd()
{
	for (auto& pClient : m_peerClients)
	{
		if (pClient == nullptr)
		{
			continue;
		}
		else
		{
			std::shared_ptr<ClientSocket> soc = pClient->GetSocket();
			std::shared_ptr<Session> pSession = m_sessions[pClient->GetSessionId()];

			if (pSession == nullptr) return;
			if (!pSession->GetEndState()) return;
		}
	}

	BroadcastPacket(SerializeBuffer(sizeof(PacketS2C_EndAction), S2C_END_ACTION, nullptr), sizeof(PacketS2C_EndAction));

	for (auto& pClient : m_peerClients)
	{
		std::shared_ptr<ClientSocket> soc = pClient->GetSocket();
		std::shared_ptr<Session> pSession = m_sessions[pClient->GetSessionId()];

		if (pSession == nullptr) return;

		pSession->SetEndState(false);
	}
}

void ServerNetworkManager::ServerLoop()
{
	while (m_bRun)
	{
		Update();
		NetUpdate();
	}
}