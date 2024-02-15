#include "pch.h"
#include "BlockGenerator.h"

#include "YAMLParser.h"
#include "BlockObject.h"
#include "BlockPOD.h"

#include "../Engine/World.h"

BlockGenerator::BlockGenerator(std::shared_ptr<World> world, std::string_view filePath)
{
	m_ownerWorld = world;

	std::shared_ptr<YAMLParser> parser = std::make_shared<YAMLParser>();
	parser->ReadBlockPOD(filePath, m_blockPODs);
}

BlockGenerator::~BlockGenerator()
{
}

void BlockGenerator::Generate()
{
	std::shared_ptr<World> world = m_ownerWorld.lock();
	for(const auto& pod : m_blockPODs)
	{
		std::shared_ptr<BlockObject> block = world->CreateGameObject<BlockObject>("block", eObjectType::LEVEL).lock();
		block->SetFilePath(MatchingFilePath(pod->blockType));
		block->SetBlockPOD(pod);
	}
}

std::string BlockGenerator::MatchingFilePath(eBlockType blockType)
{
	switch (blockType)
	{
	case eBlockType::ONE:
		return "../Resources/FBX/TOTEM_1.fbx";
	case eBlockType::TWO:
		return "../Resources/FBX/TOTEM_1.fbx";
	case eBlockType::THREE:
		return "../Resources/FBX/TOTEM_1.fbx";
	default:
		LOG_CONTENTS("정의되지 않은 블록 타입");
		assert(true);
		return "";
	}
}

