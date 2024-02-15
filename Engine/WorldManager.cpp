#include "pch.h"
#include "WorldManager.h"

#include "CommonApp.h"

#include <Winsock2.h>

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
char* WorldManager::SerializeBuffer(short size, EPacketId id, char* msg)
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

void WorldManager::DeSerializeBuffer(PacketHeader& packet, char* buf)
{
	short sizeInfo = 0;
	memcpy(&sizeInfo, buf, sizeof(short));

	EPacketId idInfo;
	memcpy(&idInfo, buf + sizeof(short), sizeof(EPacketId));

	packet.size = sizeInfo;
	packet.id = idInfo;
}
