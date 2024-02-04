#pragma once

constexpr float blockDefaultScale = 100;

// ������Ʈ Block�� ���� POD
struct Block
{
    std::string blockType;
    struct Position
    {
        float x;
        float y;
        float z;
    } position;
    struct Rotation
    {
        float x;
        float y;
        float z;
    } rotation;
    struct Scale
    {
        float x = blockDefaultScale;
        float y = blockDefaultScale;
        float z = blockDefaultScale;
    } scale;
    //m_LocalEulerAnglesHint.z
};

enum class eBlockType
{
	ONE,
    TWO,
    THREE,

    END
};

// �ۿ����� ���� �о����
// ��� ���ʷ����� ���۽�Ŵ
// ��� ������ �׸���Ŵ����� ������ �� �־�� �Ѵ�.
// �׷��Ƿ� ��� ���� ��ü�� ���忡 �����ϱ�
// 

// ������ �Ľ��ϴ� Ŭ����
// ��� ���ʷ����� �ϳ� �� �����
class YAMLParser
{
public:
	void LoadFile(std::string_view filePath);

private:
	std::string m_filePath;
    std::vector<Block*> m_blockTransforms[static_cast<int>(eBlockType::END)];
};

