#pragma once
#include "../Engine/GameObject.h"

class StaticMeshComponent;

class StaticFbxObject
	: public GameObject
{
public:
	void Initialize() override;
	void Update(float deltaTime) override;

private:
	std::weak_ptr<StaticMeshComponent> m_meshComponent;
};

