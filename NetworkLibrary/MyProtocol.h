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

	// 서버에게 내가 host라고 전달해주기(그러면 Session에서 m_bHost를 true로 만들어줌)
	C2S_IAM_HOST_MSG,

	// 클라가 레디 버튼 눌렀을 때
	C2S_READY,

	// 모든 클라가 레디인지 확인
	C2S_IS_ALL_READY,
	S2C_IS_ALL_READY,

	// 누구턴인지 턴 세팅해주기
	C2S_SET_TURN,
	S2C_SET_TURN,

	// 턴 시작이다
	C2S_START_TURN,
	S2C_START_TURN,

	// 턴 끝이다
	C2S_END_TURN,
	S2C_END_TURN,

	// 턴 바꾸기
	C2S_CHANGE_TURN,
	S2C_CHANGE_TURN,

	// 행동이 남아있는지 확인
	C2S_CHECK_ACTION,
	S2C_CHECK_ACTION,

	// 모든 행동이 끝났다.
	C2S_END_ACTION,
	S2C_END_ACTION,

	// 캐릭터가 어디로 이동하는지 서버에 알리기
	C2S_CHARACTER_MOVE,
	S2C_CHARACTER_MOVE,

	// 캐릭터가 어디로 이동하는지 Broadcast
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

// 세션에게 호스트인지 알려주기 위해서
struct PacketC2S_IamHost : PacketHeader
{
	char host;
};
// 클라에서 레디 버튼 눌렀을 때
struct PacketC2S_READY : PacketHeader
{
	char clickedReady;
};
// 클라에서 게임 시작 전 모든 클라가 레디상태인지 요청
struct PacketC2S_IsAllReady : PacketHeader
{
};
// 서버가 클라한테 모두 레디면 true 아니면 false 보내준다.
struct PacketS2C_IsAllReady : PacketHeader
{
	char isReady;
};

struct PacketC2S_SetTurn : PacketHeader
{
	// '0'이면 host, '1'이면 클라이언트
	char who;
};

struct PacketS2C_SetTurn : PacketHeader
{
	// '0'이면 host, '1'이면 클라이언트
	char setTurn;
};

struct PacketC2S_StartTurn : PacketHeader
{
	// 턴 시작하기
};

struct PacketC2S_EndTurn : PacketHeader
{
	// 호스트가 종료 요청했는지, 게스트가 종료 요청했는지
};

struct PacketS2C_EndTurn : PacketHeader
{
	// 행동이 끝나면 턴 종료하기(client에 있는 turn 불값을 false로 바꿔준다.
};

struct PacketC2S_ChangeTurn : PacketHeader
{
	// 호스트가 종료 요청했는지, 게스트가 종료 요청했는지
};

struct PacketS2C_ChangeTurn : PacketHeader
{
	// 행동이 끝나면 턴 종료하기(client에 있는 turn 불값을 false로 바꿔준다.
};

struct PacketS2C_StartGame : PacketHeader
{
	// 게임을 시작
};

struct PacketC2S_CheckAction : PacketHeader
{
	// 행동이 남아있는지 확인
};

struct PacketS2C_CheckAction : PacketHeader
{
	// 행동이 남아있는지 확인
};

struct PacketC2S_EndAction : PacketHeader
{
	// 게임을 끝
};

struct PacketS2C_EndAction : PacketHeader
{
	// 게임을 끝
};

struct PacketC2S_CharacterMove : PacketHeader
{
	// 어떤 플레이어
	char who;
	// 어디로 움직였는지(상, 하, 좌, 우)
	char direction;
};

struct PacketS2C_CharacterMove : PacketHeader
{
	// 어떤 플레이어
	char who;
	// 어디로 움직였는지(상, 하, 좌, 우)
	char direction;
};

#pragma pack (pop)