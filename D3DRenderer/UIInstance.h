#pragma once

class Texture;
class UIMesh;

struct CB_UIAnimationKeyframe
{
	Vector2 m_Offset = Vector2{ 0.f, 0.f };
	Vector2 m_Size = Vector2{ 0.f, 0.f };
	Vector2 m_TextureSize = Vector2{ 0.f, 0.f };
	float m_AnimationTime;
	int m_UseAnimation;
};

class UIInstance
{
public:
	UIInstance();
	~UIInstance();

public:
	// 건재 : 애니메이션 루프 Get & Set 추가 및 애니메이션 키 프레임 추가함수 추가
	bool GetIsLoop() { return m_bIsLoop; }
	void SetIsLoop(bool isloop) { m_bIsLoop = isloop; }
	void SetIsRender(bool isRender) { m_bIsRender = isRender; }
	Math::Matrix* GetWorldMatrix() { return m_pWorldTransform; }
	bool IsRender() { return m_bIsRender; }
	void NotLoopAnimationStart() { if (!m_bIsLoop) { m_nCurrentAnimationIndex = 0; m_DurationTime = 0; } }

	void AddKeyframe(CB_UIAnimationKeyframe keyframe) { m_KeyFrame.push_back(keyframe); }

public:
	void LoadTextureResource(std::wstring_view texturePath);
	void Create(Math::Matrix* pWorld, Math::Vector2 size);
	void Update(float deltaTime);
	void Render(ID3D11DeviceContext* deviceContext);

	void UIAnimationPlay(CB_UIAnimationKeyframe& animationKeyFrame);

private:
	bool m_bIsRender;

	Math::Matrix* m_pWorldTransform = nullptr;
	std::shared_ptr<UIMesh> m_UIMesh;
	std::shared_ptr<Texture> m_Texture;

	bool m_bIsLoop;
	float m_DurationTime;
	float m_AnimationTime;
	int m_nCurrentAnimationIndex;
	std::vector<CB_UIAnimationKeyframe> m_KeyFrame;
};