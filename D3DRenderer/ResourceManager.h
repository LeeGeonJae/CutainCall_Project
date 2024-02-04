#pragma once

#include "ModelResource.h"
#include "Shader.h"

class ModelResource;
class MaterialTexture;
class Animation;
class Shader;
class Texture;

class ResourceManager
{
public:
	~ResourceManager() = default;


private:
	ResourceManager() = default;
	static ResourceManager* Instance;

public:
	static ResourceManager* GetInstance()
	{
		if (!Instance)
			Instance = new ResourceManager;

		return Instance;
	}
	std::shared_ptr<MaterialTexture> CreateMaterialTexture(std::wstring filePath);
	std::shared_ptr<ModelResource> CreateModelResource(std::string filePath, ModelType modelType);
	std::shared_ptr<Animation> CreateAnimationResource(std::string filePath);

	// ���� ���͸��� �������� ���� �ؽ�ó�� ����Ҽ� �����Ƿ� ���� ��Ͽ��� ���ϰ�� Ȯ���ؼ� ������
	std::shared_ptr<Texture> CreateTextureResource(std::wstring filePath);
	std::shared_ptr<Shader> CreateShaderResource(std::string key, eShaderType shaderType);

	void Finalize();


	std::unordered_map<std::wstring, std::weak_ptr<MaterialTexture>> m_MaterialTextureMap;	// Material
	std::unordered_map<std::string, std::weak_ptr<ModelResource>> m_StaticModelMap;			// StaticModel
	std::unordered_map<std::string, std::weak_ptr<ModelResource>> m_SkeletalModelMap;		// SkeletalModel
	std::unordered_map<std::string, std::weak_ptr<Animation>> m_AnimationMap;				// Animation
	std::unordered_map<std::wstring, std::shared_ptr<Texture>> m_TextureMap;				// UI Texture
	std::unordered_map<std::string, std::shared_ptr<Shader>> m_ShaderMap;					// Shader
};
