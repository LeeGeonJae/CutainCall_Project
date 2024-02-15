#pragma once

#include "Component.h"
#include "Transform.h"

/// <summary>
/// ��ġ������ ���� Component
/// �Ϲ������δ� �ν��Ͻ�ȭ���� �ʴ´�.
/// ��ġ������ �ʿ�� �ϴ� ������Ʈ���� �ش� ������Ʈ�� ��ӹ޾� ����Ѵ�.
/// </summary>
class SceneComponent
	: public Component
	, public Transform
{
public:
	SceneComponent() = default;
	~SceneComponent() override = default;

public:
	void SetLocalPosition(const Math::Vector3 pos);
	void SetLocalScale(const Math::Vector3 scale);
	void SetLocalRotation(const Math::Vector3 rot);
	void SetLocalTransform(Math::Matrix matrix);

	Math::Vector3& GetLocalPosition() { return m_localPosition; }
	Math::Vector3& GetLocalScale() { return m_localScale; }
	Math::Vector3& GetLocalRotation() { return m_localRotation; }
	Math::Matrix& GetWorldTransform() { return m_worldMatrix; }
	Math::Matrix& GetLocalTransform() { return m_localMatrix; }
	 Math::Vector3& GetForward() const;

public:
	void Initialize() override;
	void Update(float deltaTime) override;

private:
	Math::Vector3 m_localPosition = Math::Vector3(0.f, 0.f, 0.f);
	Math::Vector3 m_localScale = Math::Vector3(1.f, 1.f, 1.f);
	Math::Vector3 m_localRotation = Math::Vector3(0.f, 0.f, 0.f);

	bool m_bCalced = false;	// �� ������ ��Ʈ������ ������ �ʱ� ���� ����
};

