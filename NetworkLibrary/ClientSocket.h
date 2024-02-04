#pragma once
#include "WinSock.h"

class ClientSocket : public WinSock
{
public:
	ClientSocket() = default;
	~ClientSocket() = default;

	virtual bool OnAccept(std::shared_ptr<WinSock> pSocket) override { return false; }
	virtual bool OnConnect() override { return false; }
	virtual bool OnClose() override { return false; }
	virtual bool OnSend() override { return false; }
	virtual bool OnReceive() override { return false; }
};
