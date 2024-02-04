// 컴퓨터, 프로그램마다 구조체 정렬 규칙이 다를 수 있다.
// 구조체를 정의하고 #pragma pack을 통해 원하는대로 정렬할 수 있다.

// 패킷이 크지 않고 고정 크기로도 충분할 때가 많아서
// 패킷 정의를 구조체로 하고 1바이트 패킹을 하는 경우가 많다.
// 장점은 구현이 쉽다.
// 단점은 멤버 데이터 타입이 자유롭지 않다.
// 좀 더 자유롭게 사용하기 위해 시리얼라이즈 패킷 모듈을 만들거나 기존 라이브러리를 사용한다.
// 유명한 라이브러리로는 FlatBuffer가 있다.
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

// 세션에게 호스트인지 알려주기 위해서
struct PacketC2S_IamHost : PacketHeader
{
	std::byte isHost;
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
	// 둘다 레디면 턴 시작하기
	char startTurn;
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