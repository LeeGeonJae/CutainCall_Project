#include "pch.h"
#include "YAMLParser.h"

#include "BlockPOD.h"

// yaml-cpp
#define YAML_CPP_STATIC_DEFINE
#include <yaml-cpp/yaml.h>

#ifdef _DEBUG
#pragma comment(lib, "yaml-cppd.lib")
#else
#pragma comment(lib, "yaml-cpp.lib")
#endif

void YAMLParser::ReadBlockPOD(std::string_view filePath, std::vector<std::shared_ptr<BlockPOD>>& blockPODs)
{
	try
	{
		std::vector<YAML::Node> blockNodes = YAML::LoadAllFromFile(std::string(filePath));
		

		for(const auto& node : blockNodes)
		{
			if (!node["PrefabInstance"])
				continue;

			std::shared_ptr<BlockPOD> block = std::make_shared<BlockPOD>();
			for(const auto& modification : node["PrefabInstance"]["m_Modification"]["m_Modifications"])
			{
				std::string property = modification["propertyPath"].as<std::string>();
				// Type
				if (property == "m_Name")
				{
					block->blockType = GetBlockType(modification["value"].as<std::string>());

					if (block->blockType == eBlockType::END)
                        continue;
				}

				// Position
				if (property == "m_LocalPosition.x")
				{
					block->position.x = modification["value"].as<float>() * 100;
				}
                if (property == "m_LocalPosition.y")
                {
                    block->position.y = modification["value"].as<float>() * 100;
                }
                if (property == "m_LocalPosition.z")
                {
                    block->position.z = modification["value"].as<float>() * 100;
                }

                // Rotation
                if (property == "m_LocalEulerAnglesHint.x")
                {
                    block->rotation.x = modification["value"].as<float>();
                }
                if (property == "m_LocalEulerAnglesHint.y")
                {
                    block->rotation.y = modification["value"].as<float>();
                }
                if (property == "m_LocalEulerAnglesHint.z")
                {
                    block->rotation.z = modification["value"].as<float>();
                }

                // Scale
                if (property == "m_LocalScale.x")
                {
                    block->scale.x = modification["value"].as<float>() / blockDefaultScale;
                }
                if (property == "m_LocalScale.y")
                {
                    block->scale.y = modification["value"].as<float>() / blockDefaultScale;
                }
                if (property == "m_LocalScale.z")
                {
                    block->scale.z = modification["value"].as<float>() / blockDefaultScale;
                }
			}

            blockPODs.emplace_back(block);
		}
	}
    catch (const YAML::Exception& e) {
        LOG_CONTENTS("YAML parsing error: " + std::string(e.what()));
        assert(true);
    }
}

eBlockType YAMLParser::GetBlockType(std::string_view blockName)
{
    std::string name = std::string(blockName);

    if(name == "totem")
    {
        return eBlockType::ONE;
    }
    if (name == "Block2")
    {
        return eBlockType::TWO;
    }
    if (name == "Block3")
    {
        return eBlockType::THREE;
    }

    LOG_CONTENTS("BlockName, Type ¸ÅÄª ºÒ°¡");
    return eBlockType::END;
}
