#include "pch.h"
#include "UIMainWorldButtonHover.h"
#include "UIObject.h"

void UIMainWorldButtonHover::ExecuteHovered(UIObject& onwer, float deltaTime)
{
	m_hovering = true;
	LOG_ENGINE("GameStart Hover In UI");
	onwer.GetUIInstance().lock()->LoadTextureResource(m_hoverTexturePath);
	onwer.SetScale({ 1.3f, 1.3f });
}

void UIMainWorldButtonHover::ExecuteHoverOut(UIObject& onwer, float deltaTime)
{
	if(m_hovering)
	{
		LOG_ENGINE("GameStart Hover Out UI");
		onwer.GetUIInstance().lock()->LoadTextureResource(onwer.GetTexturePath());
		onwer.SetScale({ 1.f, 1.f });
		m_hovering = false;
	}
}
