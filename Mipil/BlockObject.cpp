#include "pch.h"
#include "BlockObject.h"

#include "BlockPOD.h"
#include "../Engine/StaticMeshComponent.h"

void BlockObject::Initialize()
{
	std::shared_ptr<StaticMeshComponent> meshComponent = CreateComponent<StaticMeshComponent>("meshComponent").lock();
	meshComponent->SetFilePath(m_filePath);
	SetRootComponent(meshComponent);

	// TODO : position 단위.. 맞춰야 함
	meshComponent->SetLocalPosition(m_blockPOD->position);
	meshComponent->SetLocalRotation(m_blockPOD->rotation);
	meshComponent->SetLocalScale(m_blockPOD->scale);

	GameObject::Initialize();
}

void BlockObject::Update(float deltaTime)
{
	GameObject::Update(deltaTime);
}
