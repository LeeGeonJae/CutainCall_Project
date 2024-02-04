#pragma once

#include "../Engine/Client.h"
#include  "../Engine/CommonApp.h"
#include "../Engine/Server.h"
#include "../Engine/WorldManager.h"

#include <memory>
#include <thread>

class WorldManager;
class Model;
class TestWorld;

/// <summary>
///	���� �����Ǵ� ���� ���ø����̼�
/// �Ŵ������� Init, Run, Render�Ѵ�.
/// </summary>
class GameApp
	: public CommonApp
{
	///Test
private:
	// Todo: ä�� : Test
	//ServerNetworkManager m_server;
	//ClientNetworkManager m_client;
	//// �����Ķ���ͷ� ���� ip �ּ�
	//char* m_pIPAdd;

public:
	GameApp(HINSTANCE hInstance);
	~GameApp() = default;

public:
	// __suer::init ������ ���̴� ���� �̸� �����ؾ���
	bool Initialize() final;
	void Update() final;
	void Render() final;
	void Finalize() final;

	void SetHost(bool isHost) { m_bHost = isHost	;}
	void SetIP(std::string ip) { m_ip = ip; }
	void SetUseNetwork(bool useNetwork) { m_bUseNetwork = useNetwork; }

private:
	LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) override;
};

