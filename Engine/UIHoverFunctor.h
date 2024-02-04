#pragma once
#include "UIFunctor.h"

class UIHoverFunctor :
	public UIFunctor
{
public:
	virtual ~UIHoverFunctor() = default;
	virtual void Execute(const UIObject& onwer, float deltaTime) override;
};