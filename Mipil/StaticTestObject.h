#pragma once
#include "../Engine/GameObject.h"

class StaticMeshComponent;

class StaticTestObject
	: public GameObject
{
public:
	void Initialize() override;
	void Update(float deltaTime) override;

private:
	std::weak_ptr<StaticMeshComponent> m_meshComponent;
};

