#pragma once
#include "SkeletalMeshInstance.h"
#include "Bone.h"

class Material;
class ModelResource;
class SkeletonResource;

class SkeletalMeshModel
{
public:
	SkeletalMeshModel();
	~SkeletalMeshModel();


	Material* GetMaterial(UINT index);
	double GetAnimationDuration(int animationIdx);
	void SetWorldMatrix(Math::Matrix* worldMatrix) { m_worldMatrix = worldMatrix; }
	void SetAnimationTime(float* progressTime) { m_pAnimationProgressTime = progressTime; }

	bool ReadSceneResourceFromFBX(std::string filePath);
	bool AddSceneAnimationFromFBX(std::string filePath);
	void PlayAnimation(UINT index);
	void UpdateBoneNode(float deltaTime);
	void ChangeBoneAnimationReference(UINT index);

private:
	void SetSceneResource(std::shared_ptr<ModelResource> val);
	void InitBoneAnimationReference(UINT index);
	void CreateHierachy(SkeletonResource* skeleton);

	//void Update(float DeltaTime) override;

public:
	bool m_IsCulled = true;

	Bone m_RootNode;
	float* m_pAnimationProgressTime = nullptr;
	Math::Matrix* m_worldMatrix = nullptr;	// meshComponent�� matrix ����
	BoundingBox m_BoundingBox;

	std::shared_ptr<ModelResource>  m_ModelResource;
	std::vector<SkeletalMeshInstance> m_MeshInstances;
	//std::list<SkeletalMeshModel*>::iterator m_iterator;

	//std::string m_SceneFilePath; // BeginPlay���� �ε�
	//std::list<std::string> m_AnimationFilePath; // BeginPlay���� �ε�

};