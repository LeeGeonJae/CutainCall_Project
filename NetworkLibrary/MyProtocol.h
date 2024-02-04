// ��ǻ��, ���α׷����� ����ü ���� ��Ģ�� �ٸ� �� �ִ�.
// ����ü�� �����ϰ� #pragma pack�� ���� ���ϴ´�� ������ �� �ִ�.

// ��Ŷ�� ũ�� �ʰ� ���� ũ��ε� ����� ���� ���Ƽ�
// ��Ŷ ���Ǹ� ����ü�� �ϰ� 1����Ʈ ��ŷ�� �ϴ� ��찡 ����.
// ������ ������ ����.
// ������ ��� ������ Ÿ���� �������� �ʴ�.
// �� �� �����Ӱ� ����ϱ� ���� �ø�������� ��Ŷ ����� ����ų� ���� ���̺귯���� ����Ѵ�.
// ������ ���̺귯���δ� FlatBuffer�� �ִ�.
#pragma once
#include <cstddef>
#include <string>

#pragma pack (push, 1)

#define MAX_LEN 100

enum EPacketId	: short
{
	PACKETID_START,
	
	C2S_BROADCAST_MSG,
	S2C_BROADCAST_MSG,

	// �������� ���� host��� �������ֱ�(�׷��� Session���� m_bHost�� true�� �������)
	C2S_IAM_HOST_MSG,

	// Ŭ�� ���� ��ư ������ ��
	C2S_READY,
	// ��� Ŭ�� �������� Ȯ��
	C2S_IS_ALL_READY,
	S2C_IS_ALL_READY,

	// ���������� �� �������ֱ�
	C2S_SET_TURN,
	S2C_SET_TURN,
	// �� �����̴�
	C2S_START_TURN,
	S2C_START_TURN,
	// �� ���̴�
	C2S_END_TURN,
	S2C_END_TURN,

	// �� �ٲٱ�
	C2S_CHANGE_TURN,
	S2C_CHANGE_TURN,

	// ĳ���Ͱ� ���� �̵��ϴ��� ������ �˸���
	C2S_CHARACTER_MOVE,
	S2C_CHARACTER_MOVE,
	// ĳ���Ͱ� ���� �̵��ϴ��� Broadcast
	S2C_BROADCAST_MOVE,

	PACKETID_END,
};

struct PacketHeader
{
	short size;
	EPacketId id;
};

struct PacketC2S_Access : PacketHeader
{
	char nickname[MAX_LEN];
};

struct PacketS2C_Access : PacketHeader
{
	std::byte result;
};

struct PacketC2S_BroadcastMsg : PacketHeader
{
	char clientMessage[MAX_LEN];	
};

struct PacketS2C_BroadcastMsg : PacketHeader
{
	char serverMessage[MAX_LEN];
};

// ���ǿ��� ȣ��Ʈ���� �˷��ֱ� ���ؼ�
struct PacketC2S_IamHost : PacketHeader
{
	std::byte isHost;
};
// Ŭ�󿡼� ���� ��ư ������ ��
struct PacketC2S_READY : PacketHeader
{
	char clickedReady;
};
// Ŭ�󿡼� ���� ���� �� ��� Ŭ�� ����������� ��û
struct PacketC2S_IsAllReady : PacketHeader
{
};
// ������ Ŭ������ ��� ����� true �ƴϸ� false �����ش�.
struct PacketS2C_IsAllReady : PacketHeader
{
	char isReady;
};

struct PacketC2S_SetTurn : PacketHeader
{
	// '0'�̸� host, '1'�̸� Ŭ���̾�Ʈ
	char who;
};

struct PacketS2C_SetTurn : PacketHeader
{
	// '0'�̸� host, '1'�̸� Ŭ���̾�Ʈ
	char setTurn;
};

struct PacketC2S_StartTurn : PacketHeader
{
	// �Ѵ� ����� �� �����ϱ�
	char startTurn;
};

struct PacketC2S_EndTurn : PacketHeader
{
	// ȣ��Ʈ�� ���� ��û�ߴ���, �Խ�Ʈ�� ���� ��û�ߴ���
};

struct PacketS2C_EndTurn : PacketHeader
{
	// �ൿ�� ������ �� �����ϱ�(client�� �ִ� turn �Ұ��� false�� �ٲ��ش�.
};

struct PacketC2S_ChangeTurn : PacketHeader
{
	// ȣ��Ʈ�� ���� ��û�ߴ���, �Խ�Ʈ�� ���� ��û�ߴ���
};

struct PacketS2C_ChangeTurn : PacketHeader
{
	// �ൿ�� ������ �� �����ϱ�(client�� �ִ� turn �Ұ��� false�� �ٲ��ش�.
};

struct PacketC2S_CharacterMove : PacketHeader
{
	// � �÷��̾�
	char who;
	// ���� ����������(��, ��, ��, ��)
	char direction;
	
	
};

struct PacketS2C_CharacterMove : PacketHeader
{
	// � �÷��̾�
	char who;
	// ���� ����������(��, ��, ��, ��)
	char direction;
};

#pragma pack (pop)