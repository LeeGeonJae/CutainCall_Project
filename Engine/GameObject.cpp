#include "pch.h"
#include "GameObject.h"

#include "SceneComponent.h"

GameObject::~GameObject()
{
	for(auto& component : m_ownComponents)
	{
		component.reset();
	}
	m_ownComponents.clear();
}

void GameObject::SetPosition(Vector3 position)
{
	m_pRootComponent->SetLocalPosition(position);
}

void GameObject::SetRotation(Vector3 rotation)
{
	m_pRootComponent->SetLocalRotation(rotation);
}

void GameObject::SetScale(Vector3 scale)
{
	m_pRootComponent->SetLocalScale(scale);
}

std::weak_ptr<Component> GameObject::GetComponent(std::string_view name) const
{
	return m_ownComponents[m_componentMap.find(std::string(name))->second];
}

const Vector3& GameObject::GetPosition() const
{
	return m_pRootComponent->GetLocalPosition();
}

const Vector3& GameObject::GetRotation() const
{
	return m_pRootComponent->GetLocalRotation();
}

const Vector3& GameObject::GetScale() const
{
	return m_pRootComponent->GetLocalScale();
}

const Matrix& GameObject::GetTransform() const
{
	return m_pRootComponent->GetWorldTransform();
}

void GameObject::Initialize()
{
	for (const auto& component : m_ownComponents)
	{
		component->Initialize();
	}
}

void GameObject::Update(float deltaTime)
{
	for (const auto& component : m_ownComponents)
	{
		component->Update(deltaTime);
	}
}