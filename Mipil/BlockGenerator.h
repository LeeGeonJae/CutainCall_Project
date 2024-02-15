#pragma once

enum class eBlockType;
class World;
struct BlockPOD;

class BlockGenerator
{
public:
	BlockGenerator(std::shared_ptr<World> world, std::string_view filePath);
	~BlockGenerator();

	void Generate();

private:
	std::string MatchingFilePath(eBlockType blockType);

private:
	std::vector<std::shared_ptr<BlockPOD>> m_blockPODs;
	std::weak_ptr<World> m_ownerWorld;
};

