#pragma once
#include "../NetworkLibrary/MyProtocol.h"

#include <concurrent_queue.h>

class ClientSocket;
class ClientNetworkManager;

typedef int SessionId;

class Session
{
public:
	Session();
	~Session();

	void Write(char* pData, int len);
	// recv해서 받은 버퍼가 유효한 패킷인지 확인하기.
	void Read(char* pData);

	void ReadUpdate();

	void SetClient(std::shared_ptr<ClientNetworkManager> pClient);

	bool GetReadyState() { return m_bReady; }
	void SetReadyState(bool ready) { m_bReady = ready; }

	SessionId GetSessionId() { return m_sessionId; }

	bool PopRecvQueue(std::pair<char*, int>& value) { return m_recvQueue.try_pop(value); }
	bool PopSendQueue(std::pair<char*, int>& value) 
	{ 
		return m_sendQueue.try_pop(value);
	}

	void PushSendQueue(char* c, int len) { m_sendQueue.push({ c, len }); }
	void PushRecvQueue(char* c, int len) { m_recvQueue.push({ c, len }); }

	bool IsSendQueueEmpty() { return m_sendQueue.empty(); }
	bool IsRecvQueueEmpty() { return m_recvQueue.empty(); }

	void SetHostSession() { m_bHost = true; }
	bool GetIsHostSession() { return m_bHost; }

	int Send(char* packet, int len);

	void Finalize();

private:
	// 버퍼랑 버퍼의 length값 갖고 있는 큐
	Concurrency::concurrent_queue<std::pair<char*, int>> m_sendQueue;
	Concurrency::concurrent_queue<std::pair<char*, int>> m_recvQueue;

	char m_sendBuffer[SND_BUF_SIZE];
	char m_recvBuffer[RCV_BUF_SIZE];

	int m_recvBytes = 0;
	int m_sendBytes = 0;

	std::shared_ptr<ClientNetworkManager> m_pPeerClient = nullptr;
	SessionId m_sessionId = 0;
	static SessionId s_sessionIdCounter;

	bool m_bHost = false;
	bool m_bReady = false;

	std::string m_NickName = {};
};

