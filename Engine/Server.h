#pragma once
#include <unordered_map>
#include <vector>

#include "../NetworkLibrary/MyProtocol.h"

class ServerSocket;
class Session;
class ClientNetworkManager;
class WinSock;

typedef int SessionId;

/// <summary>
/// ����
/// Ŭ���̾�Ʈ�� ������ ��� ������ Ŭ���̾�Ʈ�� ������ �����Ѵ�.
/// ���� ����, �κ� ���� ������ �ֱ�.
/// </summary>

class ServerNetworkManager
{
public:
	ServerNetworkManager() = default;
	~ServerNetworkManager() = default;

	void Start(const char* add);
	void Stop();
	void CleanUp();

	void Update();
	void NetUpdate();

	void OnAccept();
	void OnReceive(std::shared_ptr<ClientNetworkManager> pClient);
	void OnSend(std::shared_ptr<WinSock> pSocket);
	void OnClose(std::shared_ptr<ClientNetworkManager> pClient);
	void onNetError(int errorCode, const char* errorMsg, std::shared_ptr<WinSock> pSocket = nullptr);

	char* SerializeBuffer(int size, EPacketId id, char* msg);
	void DeSerializeBuffer(PacketHeader& packet, char* buf);

	void CheckId(std::shared_ptr<Session> session, char* pData, int len);
	void BroadcastPacket(char* pData, int len);

	// ���� ���� ���� �Լ���
public:
	void ClientSetReady(std::shared_ptr<Session> session, char* pMsg);
	void SetTurn(std::shared_ptr<Session> session, char* pMsg);
	void EndTurn(std::shared_ptr<Session> session, char* pMsg);
	void ChangeTurn();
	void StartTurn(std::shared_ptr<Session> session, char* pMsg);
	void BroadcastCheckAction();
	void BroadcastMsg(std::shared_ptr<Session> session, char* pMsg);
	void SendCharacterPosition(std::shared_ptr<Session> session, char* pMsg);
	void EndAction(std::shared_ptr<Session> session, char* pMsg);
	void IsAllReady();
	void IsAllEnd();

	void ServerLoop();

private:
	std::shared_ptr<ServerSocket> m_pListenSocket;

	std::vector<std::shared_ptr<ClientNetworkManager>> m_peerClients;
	std::unordered_map<SessionId, std::shared_ptr<Session>> m_sessions;

	int m_ClientCount = 0;

	HANDLE	m_hQuitEvent;

	bool m_bAllReady = false;
	bool m_bAllEnd = false;
	bool m_bRun = false;
	char m_currentTurn;
};

