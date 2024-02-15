#include "pch.h"
#include "SkeletalFbxObject.h"

#include "../Engine/SkeletalMeshComponent.h"

void SkeletalFbxObject::Initialize()
{
	m_meshComponent = CreateComponent<SkeletalMeshComponent>("SkeletalMeshComponent");
 	m_meshComponent.lock()->SetDefaultAnimation("../Resources/FBX/BEAR_ANI_BEAR_A.fbx");
	//m_meshComponent.lock()->AddAnimation("../Resources/FBX/bear_ head_.fbx");
 	//m_meshComponent.lock()->SetDefaultAnimation("../Resources/FBX/Zombie_Run.fbx");
	SetRootComponent(m_meshComponent.lock());

	GameObject::Initialize();
}

void SkeletalFbxObject::Update(float deltaTime)
{
	GameObject::Update(deltaTime);
}
