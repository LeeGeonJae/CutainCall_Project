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

			PacketHeader packet;
			DeSerializeBuffer(packet, sendValue.first);

			// todo 채원: 사이즈가 다를 때 해주는거 바꾸기
			if (packet.size != nSent)
				continue;

			if (nSent > 0)
			{
				delete[] sendValue.first;
			}

			// 소켓 버퍼가 가득 차서 전송이 불가능
			else if (nSent == 0)
			{

			}

			// 소켓 에러가 발생함. 우짤까
			else
			{

			}
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
	if (size <= PACKETID_START || size >= PACKETID_END)
		return nullptr;

	char* buf = new char[size + 1];

	buf[0] = size / 10 + '0';
	buf[1] = size % 10 + '0';
	buf[2] = id / 10 + '0';
	buf[3] = id % 10 + '0';

	if (msg == nullptr)
		return buf;

	memcpy(buf + 4, msg, size - 4);

	return buf;
}
void ServerNetworkManager::DeSerializeBuffer(PacketHeader& packet, char* buf)
{
	//short sizeInfo = 0;
	short sizeInfo = (buf[0] - '0') * 10 + buf[1] - '0';
	//	memcpy(&sizeInfo, buf, 2);

		//EPacketId idInfo;
	EPacketId idInfo = static_cast<EPacketId>((buf[2] - '0') * 10 + buf[3] - '0');
	//	memcpy(&idInfo, buf + 2, 2);

	packet.size = sizeInfo;
	packet.id = idInfo;
}

// 게임 로직들
void ServerNetworkManager::CheckId(std::shared_ptr<Session> session, char* pData, int len)
{
	/*PacketHeader* pHeader = reinterpret_cast<PacketHeader*>(pData);

	switch (pHeader->id)
	{
	case C2S_READY:
		short ready = static_cast<int>((pData[4] - '0') * 10 + (pData[5] - '0'));
		if (ready == 0)
			m_bReady = false;
		else
			m_bReady = true;
	}*/

	// 헤더 역직렬화
	PacketHeader packet;
	DeSerializeBuffer(packet, pData);

	switch (packet.id)
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
		ChangeTurn(session, pData);
		break;

	case C2S_BROADCAST_MSG:
		BroadcastMsg(session, pData);
		break;
	case C2S_CHARACTER_MOVE:
		SendCharacterPosition(pData);
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
	// 레디 풀기
	if (pMsg[4] - '0' == 0)
		session->SetReadyState(false);
	// 레디 하기
	else
		session->SetReadyState(true);
}

void ServerNetworkManager::SetTurn(std::shared_ptr<Session> session, char* pMsg)
{
	if (pMsg[4] == '0' && session->GetIsHostSession()) // 현재 루프의 세션이 호스트면 호스트한테 턴 세팅해주기
	{
		char msg[1] = { '0' };
		session->PushSendQueue(
			SerializeBuffer(sizeof(PacketS2C_SetTurn), S2C_SET_TURN, msg),
			sizeof(PacketS2C_SetTurn));
	}
	else if (pMsg[4] == '1' && !session->GetIsHostSession()) // 세션이 게스트면 게스트한테 턴 세팅해주기
	{
		char msg[1] = { '0' };
		session->PushSendQueue(
			SerializeBuffer(sizeof(PacketS2C_SetTurn), S2C_SET_TURN, msg),
			sizeof(PacketS2C_SetTurn));
	}
}

void ServerNetworkManager::EndTurn(std::shared_ptr<Session> session, char* pMsg)
{
	session->PushSendQueue(
		SerializeBuffer(sizeof(PacketS2C_EndTurn), S2C_END_TURN, nullptr),
		sizeof(PacketS2C_EndTurn));
}

void ServerNetworkManager::ChangeTurn(std::shared_ptr<Session> session, char* pMsg)
{
	BroadcastPacket(SerializeBuffer(sizeof(PacketS2C_ChangeTurn), S2C_CHANGE_TURN, nullptr), sizeof(PacketS2C_ChangeTurn));
}

void ServerNetworkManager::SendCharacterPosition(char* pMsg)
{
	// 호스트 서버면
	if (pMsg[4] == '0')
	{
		char host[2] = {'0', '0'}; // 좌
		BroadcastPacket(SerializeBuffer(sizeof(PacketS2C_CharacterMove), S2C_CHARACTER_MOVE, host), sizeof(PacketS2C_CharacterMove));
	}
	// 게스트 서버면
	else
	{
		char guest[2] = { '1', '0' }; // 좌
		BroadcastPacket(SerializeBuffer(sizeof(PacketS2C_CharacterMove), S2C_CHARACTER_MOVE, guest), sizeof(PacketS2C_CharacterMove));
	}
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

void ServerNetworkManager::ServerLoop()
{
	while (m_bRun)
	{
		Update();
		NetUpdate();
	}
}