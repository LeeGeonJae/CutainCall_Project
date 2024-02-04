#pragma once
#include "../Engine/FSMTransition.h"
#include "../Engine/EventListener.h"

class PlayerTransition
	: public FSMTransition
	, public EventListener
{
public:
	~PlayerTransition();

	std::string operator()() override abstract;
	void HandleEvent(Event* event) final;

protected:
	bool m_bMoving = true;	// �� ���� ���󰡷��� �ϴ� ������ ����
};

class IdleToRun
	: public PlayerTransition
{
public:
	std::string operator()() final
	{
		if (!m_bMoving)
			return "";
		else
			return "State_Run";
	}
};

class RunToIdle
	: public PlayerTransition
{
public:
	std::string operator()() final
	{
		if (m_bMoving)
			return "";
		else
			return "State_Idle";
	}
};