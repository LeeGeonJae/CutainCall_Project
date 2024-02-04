#pragma once
#include "UIHoverFunctor.h"
class UIMouseHoverTest :
    public UIHoverFunctor
{
    virtual void Execute(const UIObject& onwer, float deltaTime) override;
};

