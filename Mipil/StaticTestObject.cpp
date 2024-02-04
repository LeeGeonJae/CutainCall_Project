#include "pch.h"
#include "StaticTestObject.h"

#include "../Engine/StaticMeshComponent.h"

void StaticTestObject::Initialize()
{
	m_meshComponent = CreateComponent<StaticMeshComponent>("StaticMeshComponent");
	//m_meshComponent.lock()->SetFilePath("../Resources/FBX/zeldaPosed001.fbx");
	//m_meshComponent.lock()->SetFilePath("../Resources/FBX/cerberus2.fbx");
	m_meshComponent.lock()->SetFilePath("../Resources/FBX/sphere_alpha3.fbx");
	SetRootComponent(m_meshComponent.lock());

	GameObject::Initialize();
}

void StaticTestObject::Update(float deltaTime)
{
	GameObject::Update(deltaTime);
}
