#pragma once
#include "UIFunctor.h"

class UIClickFunctor :
	public UIFunctor
{
public:
	virtual ~UIClickFunctor() = default;
	virtual void Execute(const UIObject& onwer, float deltaTime) override;
};