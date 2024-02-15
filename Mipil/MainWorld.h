#pragma once

#include "../Engine/World.h"
#include "../Engine/EventListener.h"

/// <summary>
/// ���� ȭ���Ӵ�
/// </summary>

class MainWorld 
    : public World
    , public EventListener
{
public:
	void Initialize() override;
	void Update(float deltaTime) override;

public:
	void OnEnter() override;
	void OnExit() override;

public:
	void HandleEvent(Event* event) override;
};

