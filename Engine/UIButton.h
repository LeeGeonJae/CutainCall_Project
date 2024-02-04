#pragma once
#include "UIObject.h"

class UIButton :
    public UIObject
{
public:
    UIButton(const std::wstring& name);
    virtual ~UIButton();

    //virtual void Update(float deltaTime) override;

    void SetLabel(const std::string& label);
    void OnClick();

private:
    std::string m_label;
};

