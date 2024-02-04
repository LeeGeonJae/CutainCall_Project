#pragma once
#include "UIClickFunctor.h"
class UIMouseClickTest1 :
    public UIClickFunctor
{
    virtual void Execute(const UIObject& onwer, float deltaTime) override;
};

