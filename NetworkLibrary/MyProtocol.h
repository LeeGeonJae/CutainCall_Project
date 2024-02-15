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

	// �ൿ�� �����ִ��� Ȯ��
	C2S_CHECK_ACTION,
	S2C_CHECK_ACTION,

	// ��� �ൿ�� ������.
	C2S_END_ACTION,
	S2C_END_ACTION,

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
	char result;
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
	char host;
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
	// �� �����ϱ�
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

struct PacketS2C_StartGame : PacketHeader
{
	// ������ ����
};

struct PacketC2S_CheckAction : PacketHeader
{
	// �ൿ�� �����ִ��� Ȯ��
};

struct PacketS2C_CheckAction : PacketHeader
{
	// �ൿ�� �����ִ��� Ȯ��
};

struct PacketC2S_EndAction : PacketHeader
{
	// ������ ��
};

struct PacketS2C_EndAction : PacketHeader
{
	// ������ ��
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