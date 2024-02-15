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
///	CreateMap�� ���� ó�� ���� �԰��� ���Ѵ�.
/// �׸��� �ʿ��� ������ ������Ʈ���� SetMapState�� ���� �� ������ ����.
/// �׸��� ���� �Ʒ���� �� �ϴ� �������� ��ġ�ȴ�.
/// DX ��ǥ�� �������� ���� row�� Z��, col�� X���̴�.
/// </summary>
class GridManager
{
public:
	GridManager() = default;
	~GridManager() = default;

	// ó�� ��, �׸��� ũ��, ���� ���� ��ġ ����
	void CreateMap(int row, int col, int height, float startXPos, float startYPos, float startZPos, float gridSize);

	// ������Ʈ�κ��� ��ġ �о �׸��� ��ħ Ȯ�� �� �ʿ� ����
	void SetMapState(std::shared_ptr<GameObject> object); 
	void SetMapState(std::vector<std::shared_ptr<GameObject>>& objects);

	// ���ڷ� �̵� Ÿ�� ���� ��, ���ɼ� Ȯ�� �� �̵�ó��. �׸��� �� ����
	void MoveOnGrid(TestPlayerObject* player, Vector3 vec);
	bool IsGround(Vector3 pos);

private:
	GridPos& ConvertToGridPos(Vector3 position) const;	// vector3 ��ǥ �׸��� ��ǥ�� ��ȯ
	Vector3& ConvertToDXPos(GridPos position) const;

	bool CheckDuplicated(GridPos& gridPos) const;
	eGridType MatchingGridType(eObjectType type) const;

	bool CheckMovePossibility(GridPos& gridPos, Vector3 vec) const;	// �̵� ���� ���ο� ���� ����

	int CalculateIdx(int x, int z) const { return z * m_numCol + x; }


	int m_numRow;
	int m_numCol;
	int m_numHeight;
	float m_startXPos;	// ���� ������ �ƴ� �������� CreateMap���� �����Ѵ�.
	float m_startYPos;
	float m_startZPos;
	float m_gridSize;

	std::vector<std::vector<eGridType>> m_gridMap;
};