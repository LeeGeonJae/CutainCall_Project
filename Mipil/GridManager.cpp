#include "pch.h"
#include "GridManager.h"

#include <ios>

#include "TestPlayerObject.h"

#include "../Engine/EventManager.h"

void GridManager::CreateMap(int row, int col, int height, float startXPos, float startYPos, float startZPos, float gridSize)
{
	m_numRow = row;
	m_numCol = col;
	m_numHeight = height;
	m_startXPos = startXPos;
	m_startYPos = startYPos;
	m_startZPos = startZPos;
	m_gridSize = gridSize;

	m_gridMap.resize(m_numHeight, std::vector<eGridType>(m_numRow * m_numCol, eGridType::EMPTY));
}

void GridManager::SetMapState(std::shared_ptr<GameObject> object)
{
	GridPos gridPos = ConvertToGridPos(object->GetPosition());

	if(CheckDuplicated(gridPos))
	{
		LOG_CONTENTS("해당 그리드에 이미 데이터가 있으므로 생략");
	}
	else
	{
		m_gridMap[gridPos.y][CalculateIdx(gridPos.x, gridPos.z)] = MatchingGridType(object->GetObjectType());
	}
}

void GridManager::SetMapState(std::vector<std::shared_ptr<GameObject>>& objects)
{
	for(const auto& object : objects)
	{
		Vector3 pos = object->GetPosition();
		GridPos gridPos = ConvertToGridPos(object->GetPosition());

		if (CheckDuplicated(gridPos))
		{
			LOG_CONTENTS("해당 그리드에 이미 데이터가 있으므로 생략");
		}
		else
		{
			m_gridMap[gridPos.y][CalculateIdx(gridPos.x, gridPos.z)] = MatchingGridType(object->GetObjectType());
		}
	}
}

void GridManager::MoveOnGrid(TestPlayerObject* player, Vector3 vec)
{
	GridPos objPos = ConvertToGridPos(player->GetPosition());

	if(!CheckMovePossibility(objPos, vec))
	{
		EventManager::GetInstance()->SendEvent(eEventType::CRASH_ON_GRID, player, vec);
	}
	else
	{
		m_gridMap[objPos.y][CalculateIdx(objPos.x, objPos.z)] = eGridType::EMPTY;

		GridPos newPos = objPos + vec;
		m_gridMap[newPos.y][CalculateIdx(newPos.x, newPos.z)] = eGridType::PLAYER;

		EventManager::GetInstance()->SendEvent(eEventType::MOVE_ON_GRID, player, vec * m_gridSize);	// TEST : 즉시 이동 테스트 때문에 gridSize 곱해줌
	}
}

bool GridManager::IsGround(Vector3 pos)
{
	GridPos gridPos = ConvertToGridPos(pos);
	if (gridPos.y == 0)
		return true;

	if (m_gridMap[gridPos.y - 1][CalculateIdx(gridPos.x, gridPos.z)] != eGridType::EMPTY)
		return true;

	return false;
}

GridPos& GridManager::ConvertToGridPos(Vector3 position) const
{
	GridPos rt;
	rt.x = (position.x - m_startXPos) / m_gridSize;
	rt.y = (position.y - m_startYPos) / m_gridSize;
	rt.z = (position.z - m_startZPos) / m_gridSize;

	return rt;
}

Vector3& GridManager::ConvertToDXPos(GridPos position) const
{
	Vector3 rt;
	rt.x = position.x * m_gridSize + m_startXPos + m_gridSize / 2;
	rt.y = position.y * m_gridSize + m_startYPos + m_gridSize / 2;
	rt.z = position.z * m_gridSize + m_startZPos + m_gridSize / 2;

	return rt;
}

bool GridManager::CheckDuplicated(GridPos& gridPos) const
{
	if (m_gridMap[gridPos.y][CalculateIdx(gridPos.x, gridPos.z)] == eGridType::EMPTY)
		return false;
	return true;
}

eGridType GridManager::MatchingGridType(eObjectType type) const
{
	switch (type)
	{
	case eObjectType::LEVEL:
		return eGridType::BLOCK;
	case eObjectType::PLAYER:
		return eGridType::PLAYER;
	default:
		LOG_CONTENTS("정의되지 않은 오브젝트, 그리드 매칭");
		assert(true);
		return eGridType::EMPTY;
	}
}

bool GridManager::CheckMovePossibility(GridPos& gridPos, Vector3 vec) const
{
	bool rt = false;
	GridPos checkPos = gridPos + vec;

	if (checkPos.x < 0 || checkPos.x >= m_numCol
		|| checkPos.y < 0 || checkPos.y >= m_numHeight
		|| checkPos.z < 0 || checkPos.z >= m_numRow)	// z_up
		return rt;

	if (m_gridMap[checkPos.y][CalculateIdx(checkPos.x, checkPos.z)] == eGridType::EMPTY)
		rt = true;

	return rt;
}