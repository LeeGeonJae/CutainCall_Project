#pragma once
#include "pch.h"

#include "../Engine/EventListener.h"
#include "../Engine/FSMTransition.h"

class PlayerTransition
	: public FSMTransition
	, public EventListener
{
public:
	PlayerTransition();
	~PlayerTransition() override;

	std::string operator()() override abstract;
	void HandleEvent(Event* event) final;

	bool IsMoveState(Vector3 vec);

	bool m_bTurn = false;
	Vector3 m_action = {0.f, 0.f, 0.f};
	bool m_bGround = false;
	bool m_bOnCrash = false;
	bool m_bOnGetItem = false;

	std::string m_preState = "";
};


/// Idle
class IdleToTurnWait
	: public PlayerTransition
{
public:
	std::string operator()() final
	{
		if (m_bTurn && m_action == grid::STAY)
			return "State_TurnWait";

		return "";
	}
};

class IdleToTurnMove
	: public PlayerTransition
{
public:
	std::string operator()() final
	{
		if (m_bTurn && IsMoveState(m_action))
			return "State_TurnMove";

		return "";
	}
};

class IdleToTurnUp
	: public PlayerTransition
{
public:
	std::string operator()() final
	{
		if (m_bTurn && m_action == grid::Y_UP)
			return "State_TurnUp";

		return "";
	}
};


/// Floating
class FloatingToTurnMove
	: public PlayerTransition
{
public:
	std::string operator()() final
	{
		if (m_bTurn && IsMoveState(m_action))
			return "State_TurnMove";

		return "";
	}
};

class FloatingToTurnUp
	: public PlayerTransition
{
public:
	std::string operator()() final
	{
		if (m_bTurn && m_action == grid::Y_UP)
			return "State_TurnUp";

		return "";
	}
};

class FloatingToTurnWait
	: public PlayerTransition
{
public:
	std::string operator()() final
	{
		if (m_bTurn && m_action == grid::STAY && m_bGround)
			return "State_TurnWait";

		return "";
	}
};

class FloatingToTurnFloating
	: public PlayerTransition
{
public:
	std::string operator()() final
	{
		if (m_bTurn && m_action == grid::STAY && !m_bGround)
			return "State_TurnFloating";

		return "";
	}
};


/// TurnWait
class TurnWaitToIdle
	: public PlayerTransition
{
public:
	std::string operator()() final
	{
		if (!m_bTurn)
			return "State_Idle";

		return "";
	}
};


/// TurnMove
class TurnMoveToIdle
	: public PlayerTransition
{
public:
	std::string operator()() final
	{
		if (!m_bTurn)
			return "State_Idle";

		return "";
	}
};

class TurnMoveToCrash
	: public PlayerTransition
{
public:
	std::string operator()() final
	{
		if(m_bOnCrash)
		{
			m_bOnCrash = false;
			m_preState = "State_TurnMove";
			return "State_Crash";
		}

		return "";
	}
};

class TurnMoveToGetItem
	: public PlayerTransition
{
public:
	std::string operator()() final
	{
		if (m_bOnGetItem)
		{
			m_bOnGetItem = false;
			m_preState = "State_TurnMove";
			return "State_GetItem";
		}

		return "";
	}
};


/// TurnUp
class TurnUpToIdle
	: public PlayerTransition
{
public:
	std::string operator()() final
	{
		if (!m_bTurn && m_bGround)
			return "State_Idle";

		return "";
	}
};

class TurnUpToFloating
	: public PlayerTransition
{
public:
	std::string operator()() final
	{
		if (!m_bTurn && !m_bGround)
			return "State_Floating";

		return "";
	}
};

class TurnUpToCrash
	: public PlayerTransition
{
public:
	std::string operator()() final
	{
		if (m_bOnCrash)
		{
			m_bOnCrash = false;
			m_preState = "State_TurnUp";
			return "State_Crash";
		}

		return "";
	}
};

class TurnUpToGetItem
	: public PlayerTransition
{
public:
	std::string operator()() final
	{
		if (m_bOnGetItem)
		{
			m_bOnGetItem = false;
			m_preState = "State_TurnUp";
			return "State_GetItem";
		}

		return "";
	}
};


/// TurnFloating
class TurnFloatingToFloating
	: public PlayerTransition
{
public:
	std::string operator()() final
	{
		if (!m_bTurn)
			return "State_Floating";

		return "";
	}
};


/// Crash
class CrashToTurnMove
	: public PlayerTransition
{
public:
	std::string operator()() final
	{
		if (m_preState == "State_TurnMove")
		{
			m_preState = "";
			return "State_TurnMove";
		}

		return "";
	}
};

class CrashToTurnUp
	: public PlayerTransition
{
public:
	std::string operator()() final
	{
		if (m_preState == "State_TurnUp")
		{
			m_preState = "";
			return "State_TurnUp";
		}

		return "";
	}
};


/// GetItem
class GetItemToTurnMove
	: public PlayerTransition
{
public:
	std::string operator()() final
	{
		if (m_preState == "State_TurnMove")
		{
			m_preState = "";
			return "State_TurnMove";
		}

		return "";
	}
};

class GetItemToTurnUp
	: public PlayerTransition
{
public:
	std::string operator()() final
	{
		if (m_preState == "State_TurnUp")
		{
			m_preState = "";
			return "State_TurnUp";
		}

		return "";
	}
};