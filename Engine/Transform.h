#pragma once

#include <directxtk/SimpleMath.h>
namespace Math = DirectX::SimpleMath;

/// <summary>
/// �θ��� ��ġ�� �ݿ��� ��ġ ������Ʈ�� ���� ������Ʈ
///	�ͽ������� ��ӹ޾� ����Ѵ�
/// </summary>
class Transform
{
protected:
	Transform();
	~Transform() = default;

	const Math::Vector3 GetWorldPosition() const;

	void SetParent(Transform* pParent) { m_pParent = pParent; }

	virtual void Update(float deltaTime);


	Math::Matrix m_localMatrix;
	Math::Matrix m_worldMatrix;

	Transform* m_pParent = nullptr;
};

