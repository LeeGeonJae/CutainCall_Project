#include "pch.h"
#include "UIPopUp.h"

UIPopUp::UIPopUp(const std::wstring& name)
	: UIObject(name)
	, m_triggerPop(false)
{
}

UIPopUp::~UIPopUp()
{
}

void UIPopUp::Initialize()
{
	__super::Initialize();
	m_bVisible = false;
	m_triggerPop = false;

	for (auto& child : m_children)
	{
		child->SetVisible(false);
	}
}

void UIPopUp::Update(float deltaTime)
{
	__super::Update(deltaTime);

	if (m_triggerPop == false)
	{
		m_bVisible = false;

		for (auto& child : m_children)
		{
			child->SetVisible(false);
		}
	}

	if (m_triggerPop == true)
	{
		m_bVisible = true;

		for (auto& child : m_children)
		{
			child->SetVisible(true);
		}
	}
}

void UIPopUp::TriggerPop()
{
	m_triggerPop = !m_triggerPop;
}