#pragma once

struct BlockPOD;
enum class eBlockType;

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
	YAMLParser() = default;
	~YAMLParser() = default;

	void ReadBlockPOD(std::string_view filePath, std::vector<std::shared_ptr<BlockPOD>>& blockPODs);

private:
	eBlockType GetBlockType(std::string_view blockName);
};

