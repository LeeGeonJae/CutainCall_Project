#include "pch.h"
#include "Client.h"
#include "WorldManager.h"

#include "../NetworkLibrary/ClientSocket.h"
#include "../NetworkLibrary/WinSock.h"

void ClientNetworkManager::Start(const char* add)
{
    WSAData wsaData;

    if (::WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        return;

    m_clientSocket = std::make_shared<ClientSocket>();
    if (!m_clientSocket->Create())
        return;

    m_clientSocket->Connect(add, 7777);

    m_hQuitEvent = CreateEvent(NULL, false, false, NULL);
    m_bRun = true;
}

void ClientNetworkManager::Update()
{
    if (WAIT_TIMEOUT != WaitForSingleObject(m_hQuitEvent, 0))
    {
        m_bRun = false;
        return;
    }

    WSAEVENT clientEvent = m_clientSocket->GetEvent();
    int index = ::WSAWaitForMultipleEvents(1, &clientEvent, FALSE, 1, FALSE);

    if (index == WSA_WAIT_FAILED)
        return;

    index -= WSA_WAIT_EVENT_0;

    if (index == WSA_WAIT_TIMEOUT)
        return;

    WSANETWORKEVENTS networkEvents;
    if (::WSAEnumNetworkEvents(m_clientSocket->GetHandle(), clientEvent, &networkEvents) == SOCKET_ERROR)
        return;

    if (networkEvents.lNetworkEvents & FD_CONNECT)
    {
        if (networkEvents.iErrorCode[FD_CONNECT_BIT] != 0)
        {
            OnNetError(networkEvents.iErrorCode[FD_CONNECT_BIT], "Connect", m_clientSocket);
            return;
        }
        return;
    }

    if (networkEvents.lNetworkEvents & FD_READ)
    {
        if (networkEvents.iErrorCode[FD_READ_BIT] != 0)
        {
            OnNetError(networkEvents.iErrorCode[FD_READ_BIT], "Read", m_clientSocket);
            return;
        }
        OnReceive();
    }

    if (networkEvents.lNetworkEvents & FD_WRITE)
    {
        if (networkEvents.iErrorCode[FD_WRITE_BIT] != 0)
        {
            OnNetError(networkEvents.iErrorCode[FD_WRITE_BIT], "Write", m_clientSocket);

            printf("dddd\n");
            return;
        }

        OnSend();
    }

    if (networkEvents.lNetworkEvents & FD_CLOSE)
    {
        if (networkEvents.iErrorCode[FD_CLOSE_BIT] != 0)
        {
            return;
        }
    }
}

void ClientNetworkManager::NetUpdate()
{
    // todo ä��: RecvQueue�� WorldManager���� �����ϴ� ���� ������, �׸��� World �ٲ� ������ queue �ʱ�ȭ
    while(!WorldManager::GetInstance()->IsSendQueueEmpty())
    {
        std::pair<char*, int> value;

        if (!WorldManager::GetInstance()->PopSendQueue(value))
            break;

        int nSent = m_clientSocket->Send(value.first, value.second);

        short size = (value.first[0] - '0') * 10 + (value.first[1] - '0') % 10;

        // todo ä��: ����� �ٸ� �� ���ִ°� �ٲٱ�
        if (size != nSent)
            continue;

        if (nSent > 0)
        {
            delete[] value.first;
        }
        else if (nSent == 0)
        {
            // send �� �� ���� �������� �ƴ��� ���� �����?
            // �ٵ� �̷��� �Ǹ� ������ ���� �غ�Ǵ��� ��.
        }
    }
}

void ClientNetworkManager::OnReceive()
{
    int recvBytes = m_clientSocket->Recv(m_recvBuffer, m_recvBytes);

    if (recvBytes == SOCKET_ERROR)
    {
        return;
    }
    // ������ ���� ��
    if (recvBytes == 0)
    {
        return;
    }
    
    m_recvBytes += recvBytes;

    Read(m_recvBuffer);
}

void ClientNetworkManager::OnSend()
{

}

void ClientNetworkManager::OnClose()
{
}

void ClientNetworkManager::Stop()
{
    if (m_hQuitEvent != INVALID_HANDLE_VALUE)
        SetEvent(m_hQuitEvent);
}

void ClientNetworkManager::CleanUp()
{
    if (m_hQuitEvent != INVALID_HANDLE_VALUE)
    {
        CloseHandle(m_hQuitEvent);
    }

    ::WSACleanup();
}

void ClientNetworkManager::Read(char* buf)
{
    if (m_recvBytes < 2)
        return;

    short sizeInfo = 0;
    sscanf_s(buf, "%2hd", &sizeInfo);
    //memcpy(&sizeInfo, buf, 2);

    if (sizeInfo == m_recvBytes)
    {
        // �Ϸ�� ��Ʈ�� ���۸� ���Ӱ� �����ؼ� memcpy �� queue�� push
        char* packet = new char[RCV_BUF_SIZE];
        memcpy(packet, buf, m_recvBytes);
        WorldManager::GetInstance()->PushRecvQueue(packet, sizeInfo);
        m_recvBytes = 0;
        printf("readBuffer: %s \n", m_recvBuffer);
    }
    else if (sizeInfo > m_recvBytes)
    {
        printf("�����̴�");
    }
    else if (sizeInfo < m_recvBytes)
    {
        //todo ä�� : ���� �ٲ��ݽô�. mem..move..
        char* packet = new char[m_recvBytes];
        WorldManager::GetInstance()->PushRecvQueue(packet, sizeInfo);
        m_recvBytes -= sizeInfo;
    }
}

void ClientNetworkManager::Write(char buf[], int size)
{
}

void ClientNetworkManager::OnNetError(int errorCode, const char* errorMsg, std::shared_ptr<WinSock> pSocket)
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

void ClientNetworkManager::ClientLoop()
{
    while (m_bRun)
    {
        Update();
        NetUpdate();
    }
}