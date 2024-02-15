#pragma once
#include "UIClickFunctor.h"
#include "EventListener.h"

class UIGameStartClick 
    :public UIClickFunctor
    , public EventListener
{
public:
    virtual void Execute(UIObject& onwer, float deltaTime) override;

public:
    void HandleEvent(Event* event) override;
};

