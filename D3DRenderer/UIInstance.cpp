#include "pch.h"
#include "UIInstance.h"

#include "ResourceManager.h"
#include "Texture.h"
#include "UIMesh.h"

UIInstance::UIInstance()
	: m_bIsRender(true)
	, m_nCurrentAnimationIndex(0)
	, m_DurationTime(0)
	, m_AnimationTime(0.2)
{
}

UIInstance::~UIInstance()
{
	m_KeyFrame.clear();
}

void UIInstance::LoadTextureResource(std::wstring_view texturePath)
{
	m_Texture = ResourceManager::GetInstance()->CreateTextureResource(texturePath.data());
}

void UIInstance::Create(Math::Matrix* pWorld, Math::Vector2 size)
{
	m_pWorldTransform = pWorld;
	m_UIMesh = make_shared<UIMesh>();
	m_UIMesh->Create(size);
}

// UIInstance �ִϸ��̼� ����� ���� �����ð� �ޱ�
void UIInstance::Update(float deltaTime)
{
	m_DurationTime += deltaTime;
}

void UIInstance::Render(ID3D11DeviceContext* deviceContext)
{
	if (IsRender())
	{
		deviceContext->IASetVertexBuffers(0, 1, m_UIMesh->GetVertexBuffer().GetAddressOf(), m_UIMesh->GetVertexBufferStride(), m_UIMesh->GetVertexBufferOffset());
		deviceContext->IASetIndexBuffer(m_UIMesh->GetIndexBuffer().Get(), DXGI_FORMAT_R32_UINT, 0);
		deviceContext->PSSetShaderResources(0, 1, m_Texture->GetTexture().GetAddressOf());

		deviceContext->DrawIndexed(m_UIMesh->GetIndexCount(), 0, 0);
	}
}

// UIInstance �ִϸ��̼� ��� ���� ���� �Լ�
void UIInstance::UIAnimationPlay(CB_UIAnimationKeyframe& animationKeyFrame)
{
	// UI �ִϸ��̼� Ű �������� �� �ִٸ� �ִϸ��̼� ���
	// �ƴ϶�� ����
	if (!m_KeyFrame.empty())
	{
		animationKeyFrame.m_UseAnimation = true;
	}
	else
	{
		animationKeyFrame.m_UseAnimation = false;
		return;
	}

	// UI �ִϸ��̼��� ������ �ƴ϶�� �ִϸ��̼� �ε����� Ű ������ ������ϰ� �Ȱ����� �� ����
	if (!m_bIsLoop && m_KeyFrame.size() == m_nCurrentAnimationIndex + 1)
	{
		m_DurationTime = 0;
	}

	float animationTime = m_KeyFrame[m_nCurrentAnimationIndex].m_AnimationTime;
	if (m_DurationTime >= animationTime)
	{
		m_DurationTime -= animationTime;
		m_nCurrentAnimationIndex++;

		if (m_nCurrentAnimationIndex >= m_KeyFrame.size())
			m_nCurrentAnimationIndex = 0;
	}

	animationKeyFrame.m_Offset = m_KeyFrame[m_nCurrentAnimationIndex].m_Offset;
	animationKeyFrame.m_Size = m_KeyFrame[m_nCurrentAnimationIndex].m_Size;
	animationKeyFrame.m_TextureSize = m_KeyFrame[m_nCurrentAnimationIndex].m_TextureSize;


}
