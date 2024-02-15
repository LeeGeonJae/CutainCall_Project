#include "pch.h"
#include "GameApp.h"

#include <directxtk/SimpleMath.h>
#include <shellapi.h>

#include "TestWorld.h"
#include "TestWorld2.h"
#include "TestWorld3.h"
#include "MainWorld.h"
#include "Stage1World.h"

#include "../Engine/Model.h"
#include "../Engine/GameObject.h"
#include "../Engine/InputManager.h"
#include "../Engine/EventManager.h"
#include "../Engine/NetworkManager.h"

#ifdef _DEBUG
#include <vld.h>
#endif

#pragma comment (lib, "d3d11.lib")
#pragma comment(lib, "d2d1.lib")

#ifdef ENGINE_DEBUG
#pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console")
#endif

using namespace DirectX;
using namespace DirectX::SimpleMath;

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// ���� ���ڿ��� �ʱ�ȭ�մϴ�.
	GameApp App(hInstance);

	LPWSTR commandLine = GetCommandLineW();

	// ����� �Ľ�
	int argc;
	LPWSTR* argv = CommandLineToArgvW(commandLine, &argc);

	if (argc <= 1)
	{
		printf("�����Ķ���� ������ ������\n");
		App.SetUseNetwork(false);
	}
	else if (argc == 2)	// 1 ip
	{
		printf("Guest\n");
		int requiredSize = WideCharToMultiByte(CP_UTF8, 0, argv[1], -1, nullptr, 0, nullptr, nullptr);
		char* utf8String = new char[requiredSize];
		WideCharToMultiByte(CP_UTF8, 0, argv[1], -1, utf8String, requiredSize, nullptr, nullptr);

		App.SetIP(utf8String);
		App.SetUseNetwork(true);
		delete[] utf8String;
	}
	else if (argc == 3)// 1 host 2 ip
	{
		printf("Host\n");
		App.SetHost(true);

		int requiredSize = WideCharToMultiByte(CP_UTF8, 0, argv[2], -1, nullptr, 0, nullptr, nullptr);
		char* utf8String = new char[requiredSize];
		WideCharToMultiByte(CP_UTF8, 0, argv[2], -1, utf8String, requiredSize, nullptr, nullptr);

		App.SetUseNetwork(true);
		App.SetIP(utf8String);
		delete[] utf8String;
	}
	// �޸� ����
	LocalFree(argv);

	App.Initialize();
	App.Run();
	App.Finalize();

	return (int)1;
}

GameApp::GameApp(HINSTANCE hInstance)
	: CommonApp(hInstance)
{
}

bool GameApp::Initialize()
{
	bool res = __super::Initialize();

	std::shared_ptr<TestWorld> testWorld = WorldManager::GetInstance()->RegisterWorld<TestWorld>("testWorld", eWorldType::TEST).lock();
	std::shared_ptr<TestWorld2> testWorld2 = WorldManager::GetInstance()->RegisterWorld<TestWorld2>("testWorld2", eWorldType::TEST2).lock();
	std::shared_ptr<TestWorld3> testWorld3 = WorldManager::GetInstance()->RegisterWorld<TestWorld3>("testWorld3", eWorldType::TEST3).lock();
	std::shared_ptr<MainWorld> mainWorld = WorldManager::GetInstance()->RegisterWorld<MainWorld>("mainWorld", eWorldType::MAIN).lock();
	std::shared_ptr<Stage1World> stage1 = WorldManager::GetInstance()->RegisterWorld<Stage1World>("stage1", eWorldType::STAGE1).lock();


	EventManager::GetInstance()->RegisterListener(eEventType::CHANGE_WORLD, testWorld2.get());
	EventManager::GetInstance()->RegisterListener(eEventType::CHANGE_WORLD, mainWorld.get());

	WorldManager::GetInstance()->SetDefaultWorld(testWorld);
	//WorldManager::GetInstance()->SetDefaultWorld(stage1);
	//WorldManager::GetInstance()->SetDefaultWorld(testWorld);
	WorldManager::GetInstance()->Initialize();

	/// �̺�Ʈ �Ŵ����� ���� ����
	EventManager::GetInstance()->Initialize();

	return res;
}

void GameApp::Update()
{
	WorldManager::GetInstance()->Update(m_deltaTime);

	CommonApp::Update();
}

void GameApp::Render()
{
	__super::Render();

	/// �̺�Ʈ �Ŵ����� ���� ����
	EventManager::GetInstance()->Run();
}

void GameApp::Finalize()
{
	if (m_bHost)
		m_server.CleanUp();
	m_client.CleanUp();

	WorldManager::GetInstance()->Finalize();
	EventManager::GetInstance()->Finalize();
	CommonApp::Finalize();
}

LRESULT GameApp::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	return __super::WndProc(hWnd, message, wParam, lParam);
}
