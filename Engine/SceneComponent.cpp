#include "pch.h"
#include "SceneComponent.h"

void SceneComponent::SetLocalPosition(const Math::Vector3 pos)
{
	m_bCalced = false;
	m_localPosition = pos;
}

void SceneComponent::SetLocalScale(const Math::Vector3 scale)
{
	m_bCalced = false;
	m_localScale = scale;
}

void SceneComponent::SetLocalRotation(const Math::Vector3 rot)
{
	m_bCalced = false;
	m_localRotation = rot;
}

void SceneComponent::SetLocalTransform(Math::Matrix matrix)
{
	m_bCalced = false;
	Math::Quaternion q;
	matrix.Decompose(m_localScale, q, m_localPosition);
	m_localRotation = q.ToEuler();
}

void SceneComponent::Initialize()
{

}

void SceneComponent::Update(float deltaTime)
{
	if(!m_bCalced)
	{
		m_localMatrix = Math::Matrix::CreateScale(m_localScale) *
			Math::Matrix::CreateFromYawPitchRoll(XMConvertToRadians(m_localRotation.y), XMConvertToRadians(m_localRotation.x), XMConvertToRadians(m_localRotation.z)) *
			Math::Matrix::CreateTranslation(m_localPosition);
		m_bCalced = true;
	}

	Transform::Update(deltaTime);
}
