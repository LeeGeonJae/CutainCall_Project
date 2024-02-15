#include "pch.h"
#include "StaticFbxObject.h"

#include "../Engine/StaticMeshComponent.h"
#include "../Engine/SkeletalMeshComponent.h"

void StaticFbxObject::Initialize()
{
	m_meshComponent = CreateComponent<StaticMeshComponent>("StaticMeshComponent");
	m_meshComponent.lock()->SetFilePath("../Resources/FBX/BEAR_ANI_BEAR_O.fbx");
	//m_meshComponent.lock()->SetFilePath("../Resources/FBX/zeldaPosed001.fbx");
 	SetRootComponent(m_meshComponent.lock());

	GameObject::Initialize();
}

void StaticFbxObject::Update(float deltaTime)
{
	GameObject::Update(deltaTime);
}
