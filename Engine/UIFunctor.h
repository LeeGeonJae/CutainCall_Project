#pragma once

class UIObject;

class UIFunctor
{
public:
	virtual ~UIFunctor() = default;
	virtual void Execute(const UIObject& onwer, float deltaTime) abstract;
};

