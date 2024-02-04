#pragma once

constexpr float blockDefaultScale = 100;

// 오브젝트 Block을 위한 POD
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

// 밖에서는 파일 읽어오고
// 블록 제너레이터 동작시킴
// 블록 정보는 그리드매니저도 접근할 수 있어야 한다.
// 그러므로 블록 정보 자체는 월드에 저장하기
// 

// 데이터 파싱하는 클래스
// 블록 제너레이터 하나 더 만들기
class YAMLParser
{
public:
	void LoadFile(std::string_view filePath);

private:
	std::string m_filePath;
    std::vector<Block*> m_blockTransforms[static_cast<int>(eBlockType::END)];
};

