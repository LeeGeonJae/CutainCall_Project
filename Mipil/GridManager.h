#pragma once

class TestPlayerObject;
class GameObject;
enum class eObjectType;

enum class eGridType
{
	EMPTY,
	BLOCK,
	PLAYER,

	END
};

struct GridPos
{
	int x;
	int y;
	int z;

	GridPos& operator+(const GridPos& other) const
	{
		GridPos rt = { x + other.x, y + other.y, z + other.z };
		return rt;
	}

	GridPos& operator-(const GridPos& other) const
	{
		GridPos rt = { x - other.x, y - other.y, z - other.z };
		return rt;
	}

	GridPos& operator+(const Vector3& other) const
	{
		GridPos rt = { x + static_cast<int>(other.x), y + static_cast<int>(other.y), z + static_cast<int>(other.z) };
		return rt;
	}

	GridPos& operator-(const Vector3& other) const
	{
		GridPos rt = { x - static_cast<int>(other.x), y - static_cast<int>(other.y), z - static_cast<int>(other.z) };
		return rt;
	}
};

/// <summary>
///	CreateMap을 통해 처음 맵의 규격을 정한다.
/// 그리드 맵에서 동작할 오브젝트들은 SetMapState를 통해 맵 정보를 쓴다.
/// 그리드 맵은 아래평면 좌 하단 기준으로 배치된다.
/// DX 좌표계 기준으로 맵의 row는 Z축, col은 X축이다.
/// </summary>
class GridManager
{
public:
	GridManager() = default;
	~GridManager() = default;

	// 처음 맵, 그리드 크기, 시작 중점 위치 설정
	void CreateMap(int row, int col, int height, float startXPos, float startYPos, float startZPos, float gridSize);

	// 오브젝트로부터 위치 읽어서 그리드 겹침 확인 후 맵에 적용
	void SetMapState(std::shared_ptr<GameObject> object); 
	void SetMapState(std::vector<std::shared_ptr<GameObject>>& objects);

	// 인자로 이동 타입 받은 후, 가능성 확인 및 이동처리. 그리드 맵 수정
	void MoveOnGrid(TestPlayerObject* player, Vector3 vec);
	bool IsGround(Vector3 pos);

private:
	GridPos& ConvertToGridPos(Vector3 position) const;	// vector3 좌표 그리드 좌표로 변환
	Vector3& ConvertToDXPos(GridPos position) const;

	bool CheckDuplicated(GridPos& gridPos) const;
	eGridType MatchingGridType(eObjectType type) const;

	bool CheckMovePossibility(GridPos& gridPos, Vector3 vec) const;	// 이동 가능 여부와 방향 리턴

	int CalculateIdx(int x, int z) const { return z * m_numCol + x; }


	int m_numRow;
	int m_numCol;
	int m_numHeight;
	float m_startXPos;	// 블럭의 중점이 아닌 끝점으로 CreateMap에서 조정한다.
	float m_startYPos;
	float m_startZPos;
	float m_gridSize;

	std::vector<std::vector<eGridType>> m_gridMap;
};