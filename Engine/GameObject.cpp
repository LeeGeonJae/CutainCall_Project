#include "pch.h"
#include "GameObject.h"

#include "CommonApp.h"
#include "SkeletalMeshComponent.h"
#include "StaticMeshComponent.h"
#include "World.h"

GameObject::~GameObject()
{
	for(auto& component : m_ownComponents)
	{
		component.reset();
	}
	m_ownComponents.clear();
}

std::weak_ptr<Component> GameObject::GetComponent(std::string_view name) const
{
	return m_ownComponents[m_componentMap.find(std::string(name))->second];
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