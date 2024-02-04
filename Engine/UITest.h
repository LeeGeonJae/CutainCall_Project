#pragma once
#include "UIObject.h"
class UITest :
	public UIObject
{
public:
	UITest(const std::wstring& name)
		: UIObject(name)
	{
	};
};

