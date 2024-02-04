#pragma once
#include "UIClickFunctor.h"
class UIMouseClickTest :
    public UIClickFunctor
{
    virtual void Execute(const UIObject& onwer, float deltaTime) override;
};

