#pragma once
#include "RenderComponent.h"

class SkeletalMeshModel;
class Material;

/// <summary>
///	SkeletalMesh�� �������ϱ� ���� ������Ʈ
///	Renderer�� SkeletalMeshModel�� ����� �����Ѵ�.
///	�ִϸ��̼��� ���� �߰� ����, ó���� ����.
/// </summary>
class SkeletalMeshComponent
	: public RenderComponent
{
public:
	SkeletalMeshComponent() = default;
	~SkeletalMeshComponent() override;

public:
	std::shared_ptr<SkeletalMeshModel> GetSkeletalMeshModel() const { return m_skeletalMeshModel; }

	// CreateComponent �� �ʼ������� filePath ����
	void SetPlaySpeed(float speed) { m_animationPlaySpeed = speed; }
	Material* SetMaterial(Material* material, int instanceNumber);

	void SetDefaultAnimation(std::string_view filePath);
	void ChangeAnimation(std::string name);
	void AddAnimation(std::string filePath);

public:
	void Initialize() override;	// SkeletalMeshModel�� �������� ���� ����, �޾ƿ´�.
	void Update(float deltaTime) override;


private:
	std::string m_filePath;
	std::shared_ptr<SkeletalMeshModel> m_skeletalMeshModel;


	int  m_animationIndex = 0;
	float m_animationProgressTime = 0.0f;
	float m_animationPlaySpeed = 1.f;
	std::unordered_map<std::string, int> m_animationMap;

	// for anim change
	float m_durationTime = 0.0f;
	bool m_bAnimChanged = false;
};

