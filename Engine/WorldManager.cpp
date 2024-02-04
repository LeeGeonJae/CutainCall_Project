#include "pch.h"
#include "WorldManager.h"

#include "CommonApp.h"


WorldManager::WorldManager()
{
	m_worlds.reserve(static_cast<UINT>(eWorldType::END));
}

void WorldManager::ChangeWorld(eWorldType worldType)
{
	CommonApp::m_pInstance->GetRenderer()->ClearWorldResource();

	m_prevWorld = m_currentWorld;
	assert(m_worlds[static_cast<int>(worldType)]);
	m_currentWorld = m_worlds[static_cast<int>(worldType)];

	m_currentWorld->OnEnter();
	m_prevWorld->OnExit();
}

void WorldManager::ProcessPacket(char* pData, PacketHeader& packet) 
{
	EPacketId id = packet.id;
	auto it = m_handlers.find(id);
	if (it != m_handlers.end()) 
	{
		(it->second)(pData);
	}
}

void WorldManager::Initialize()
{
	m_currentWorld->OnEnter();
}

void WorldManager::Update(float deltaTime)
{
	std::pair<char*, int> value;
	while (m_recvQueue.try_pop(value))
	{
		PacketHeader packet;
		DeSerializeBuffer(packet, value.first);

		ProcessPacket(value.first, packet);
	}

	m_currentWorld->Update(deltaTime);
}

void WorldManager::Finalize()
{
	for(auto& world:m_worlds)
	{
		world.reset();
	}

	while (!m_recvQueue.empty())
	{
		std::pair<char*, int> value;

		if (m_recvQueue.try_pop(value))
		{
			delete[] value.first;
		}
	}

	while (!m_sendQueue.empty())
	{
		std::pair<char*, int> value;
		if (m_sendQueue.try_pop(value))
		{
			delete[] value.first;
		}
	}
	Singleton<WorldManager>::Finalize();
}

// Serialize 진행하기
char* WorldManager::SerializeBuffer(int size, EPacketId id, char* msg)
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

void WorldManager::DeSerializeBuffer(PacketHeader& packet, char* buf)
{
	packet.size = (buf[0] - '0') * 10 + (buf[1] - '0') % 10;
	packet.id = static_cast<EPacketId>((buf[2] - '0') * 10 + (buf[3] - '0') % 10);
}
