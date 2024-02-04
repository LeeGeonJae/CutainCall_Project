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
	bool m_bMoving = true;	// 이 값을 따라가려고 하는 경향이 있음
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