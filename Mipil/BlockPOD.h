#pragma once

constexpr float blockDefaultScale = 100;

enum class eBlockType
{
	ONE,
	TWO,
	THREE,

	END
};

// ������Ʈ Block�� ���� POD
struct BlockPOD
{
    eBlockType blockType;
    Vector3 position;
    Vector3 rotation;
    Vector3 scale = { 1, 1, 1 };
};