#pragma once
#include "../Engine/GameObject.h"

class SkeletalMeshComponent;

class SkeletalFbxObject
	: public GameObject
{
public:
	void Initialize() override;
	void Update(float deltaTime) override;

private:
	std::weak_ptr<SkeletalMeshComponent> m_meshComponent;
};

