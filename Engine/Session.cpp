#include "pch.h"
#include "Session.h"

#include "Client.h"

#include "../NetworkLibrary/ClientSocket.h"
#include "../NetworkLibrary/MyProtocol.h"
#include <iostream>

SessionId Session::s_sessionIdCounter = 0;

Session::Session()
{
}

Session::~Session()
{
}

void Session::Write(char* pData, int len)
{
}

void Session::Read(char* pData)
{
	if (m_recvBytes < 2)
		return;

	//short sizeInfo = (pData[0] - '0') * 10 + pData[1] - '0';
	short sizeInfo;
	sscanf_s(pData, "%2hd", &sizeInfo);
	//memcpy(&sizeInfo, pData, 4);

	if (sizeInfo == m_recvBytes)
	{
		// 완료된 스트림 버퍼를 새롭게 생성해서 memcpy 후 queue에 push
		char* packet = new char[RCV_BUF_SIZE];
		memcpy(packet, pData, m_recvBytes);
		m_recvQueue.push({ packet, m_recvBytes });
		m_recvBytes = 0;
		printf("readBuffer: %s \n", m_recvBuffer);
	}
	else if (sizeInfo > m_recvBytes)
	{
		printf("아직이다");
	}
	else if (sizeInfo < m_recvBytes)
	{
		//todo 채원 : 여기 바꿔줍시당
		char* packet = new char[RCV_BUF_SIZE];
		memcpy(packet, pData, sizeInfo);
		m_recvQueue.push({ packet, sizeInfo });
		m_recvBytes -= sizeInfo;
		memmove(m_recvBuffer, m_recvBuffer + sizeInfo, RCV_BUF_SIZE);
		Read(m_recvBuffer);
	}
}

void Session::ReadUpdate()
{
	std::shared_ptr<ClientSocket> soc = m_pPeerClient->GetSocket();
	int nRead = soc->Recv(m_recvBuffer, m_recvBytes);

	if (nRead == -1)
	{
		return;
	}
	m_recvBytes += nRead;

	Read(m_recvBuffer);
}

void Session::SetClient(std::shared_ptr<ClientNetworkManager> pClient)
{
	m_sessionId = s_sessionIdCounter++;

	m_pPeerClient = pClient;

	m_pPeerClient->SetSessionId(m_sessionId);
}

int Session::Send(char* packet, int len)
{
	std::shared_ptr<ClientSocket> soc = m_pPeerClient->GetSocket();

	int nSent = soc->Send(packet, len);

	return nSent;
}

void Session::Finalize()
{
	while (!m_recvQueue.empty())
	{
		std::pair<char*, int> value;

		if (m_recvQueue.try_pop(value))
			delete[] value.first;
	}

	while (!m_sendQueue.empty())
	{
		std::pair<char*, int> value;
		if (m_sendQueue.try_pop(value))
			delete[] value.first;
	}
}
