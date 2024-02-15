#include "pch.h"
#include "CubeMapObject.h"

#include "../Engine/EnvironmentComponent.h"

void CubeMapObject::Initialize()
{
	m_environmentComponent = CreateComponent<EnvironmentComponent>("environment").lock();
	m_environmentComponent->SetSceneFilePath("../Resources/FBX/EnvironmentCube.fbx");
	m_environmentComponent->SetEnvironmentTexturePath(L"../Resources/Textures/SpaceSkyBoxEnvHDR.dds");
	m_environmentComponent->SetIBLDiffuseTexturePath(L"../Resources/Textures/SpaceSkyBoxDiffuseHDR.dds");
	m_environmentComponent->SetIBLSpecularTexturePath(L"../Resources/Textures/SpaceSkyBoxSpecularHDR.dds");
	m_environmentComponent->SetIBLBRDFTexturePath(L"../Resources/Textures/SpaceSkyBoxBrdf.dds");
	m_environmentComponent->SetLocalScale(Vector3(100.f, 100.f, 100.f));

	GameObject::Initialize();
}

void CubeMapObject::Update(float deltaTime)
{
	GameObject::Update(deltaTime);
}
