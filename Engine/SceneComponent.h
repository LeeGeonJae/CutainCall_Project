#pragma once

#include "Component.h"
#include "Transform.h"

/// <summary>
/// 위치정보를 갖는 Component
/// 일반적으로는 인스턴스화하지 않는다.
/// 위치정보를 필요로 하는 컴포넌트들은 해당 컴포넌트를 상속받아 사용한다.
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

	const Math::Vector3& GetLocalPosition() const { return m_localPosition; }
	const Math::Vector3& GetLocalScale() const { return m_localScale; }
	const Math::Vector3& GetLocalRotation() const { return m_localRotation; }
	const Math::Matrix& GetWorldTransform() { return m_worldMatrix; }
	const Math::Matrix& GetLocalTransform() { return m_localMatrix; }
	const Math::Vector3& GetForward() const;

public:
	void Initialize() override;
	void Update(float deltaTime) override;

private:
	Math::Vector3 m_localPosition = Math::Vector3(0.f, 0.f, 0.f);
	Math::Vector3 m_localScale = Math::Vector3(1.f, 1.f, 1.f);
	Math::Vector3 m_localRotation = Math::Vector3(0.f, 0.f, 0.f);

	bool m_bCalced = false;	// 매 프레임 매트릭스를 만들어내지 않기 위한 변수
};

