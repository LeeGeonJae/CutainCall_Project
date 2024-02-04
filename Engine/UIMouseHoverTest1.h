#pragma once
#include "UIHoverFunctor.h"

class UIMouseHoverTest1 :
    public UIHoverFunctor
{
    virtual void Execute(const UIObject& onwer, float deltaTime) override;
};

