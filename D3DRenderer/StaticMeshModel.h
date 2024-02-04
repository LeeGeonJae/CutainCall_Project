#pragma once

#include "StaticMeshInstance.h"

class Material;
class ModelResource;

class StaticMeshModel
{
public:
	StaticMeshModel();
	~StaticMeshModel();

	Material* GetMaterial(UINT index);
	void SetWorldMatrix(Math::Matrix* worldMatrix) { m_worldMatrix = worldMatrix; }

	bool ReadSceneResourceFromFBX(std::string filePath);

private:
	void SetSceneResource(std::shared_ptr<ModelResource> val);


public:
	bool m_IsCulled = true;

	std::string m_SceneFilePath; // BeginPlay���� �ε�
	std::shared_ptr<ModelResource>  m_ModelResource;
	std::vector<StaticMeshInstance> m_MeshInstances;

	Math::Matrix* m_worldMatrix = nullptr;	// meshComponent�� matrix ����
	BoundingBox m_BoundingBox;
};
