#pragma once
#include "Graphics.h"

struct CB_BoolBuffer
{
	int UseNormalMap;
	int UseSpecularMap;
	int UseGamma;
	int UseDiffuseMap;

	int UseEmissiveMap;
	int UseOpacityMap;
	int UseMetalnessMap;
	int UseRoughnessMap;
};

struct CB_TransformW
{
	Matrix mWorld;
};
struct CB_TransformVP
{
	Matrix m_View;
	Matrix m_Projection;
	Matrix m_ShadowView;
	Matrix m_ShadowProjection;
};

struct CB_LightDirBuffer
{
	Vector4 vLightDir;
	Vector4 vLightColor;
	Vector4 mWorldCameraPosition;
	Vector4 pad[1];
};

struct CB_IBL
{
	int UseIBL = false;							// 4  16byte
	float AmbientOcclusion = 1.0f;				// 4
	Vector2 pad;								// 12	16
};

// ���ؽ� ���̴��� �Ű��ֱ� ���� ����ϴ�, �ֹ߼� ���� ����.
// �޽��� ����ϰ� �� ���� �޽��� ����ϰ� �ݺ�...�ϱ⶧����
// �ֹ߼� ���� ������ �ϳ��� �־ ��.
//
// ���⿡ ����Ǵ� ����. �޽��� �����ϴ� ���ؽ����� �����ϴ� ��� bone���� offset * boneWorld ����.
// �Ǵ��� bone�� name�̳� index�� ������ �Ǵ��ؼ� �����.
struct CB_MatrixPalette
{
	Matrix Array[128];
};


class EnvironmentModel;
class EnvironmentMeshComponent;
class SkeletalMeshInstance;
class SkeletalMeshModel;
class StaticMeshModel;
class StaticMeshInstance;
class Material;
class ResourceManager;
class Graphics;
class PipeLine;
class UIInstance;
class TextRenderer;
enum class eShaderType;

class D3DRenderManager
{
public:
	D3DRenderManager(UINT Width, UINT Height);
	~D3DRenderManager();

public:
	ComPtr<ID3D11Device> GetDevice() { return m_pDevice; }
	ComPtr<ID3D11DeviceContext> GetDevcon() { return m_pDeviceContext; }
	ComPtr<IDXGISwapChain> GetSwapChain() { return m_pGraphics->GetSwapChain(); }
	ComPtr<ID2D1RenderTarget> GetD2DRenderTarget() { return m_pGraphics->GetD2DRenderTarget(); }

	void SetViewMatrix(Math::Matrix viewMatrix);
	void SetCamMatrix(Math::Vector3 camMatrix);

	void AddStaticMeshModel(std::shared_ptr<StaticMeshModel> staticMeshModel) { m_StaticMeshModels.emplace_back(staticMeshModel); }
	void AddSkeletalMeshModel(std::shared_ptr<SkeletalMeshModel> skeletalMeshModel) { m_SkeletalMeshModels.emplace_back(skeletalMeshModel); }
	void AddUIInstance(std::shared_ptr<UIInstance> uiInstance) { m_UIInstanceList.emplace_back(uiInstance); }
	void SetEnvironmentModel(const std::shared_ptr<EnvironmentModel>& environmentModel);


public:
	//  Need  //
	bool Initialize(HWND hWnd);
	bool InitImGUI();
	bool InitD3D(); 
	void InitScene();

	//   Main Loop   //
	void Update(float deltaTime);
	void ShadowRender();
	void Render();

	//  Utility  //
	void ApplyMaterial(Material* pMaterial);
	void AddMeshInstance(std::shared_ptr<StaticMeshModel> pModel);
	void AddMeshInstance(std::shared_ptr<SkeletalMeshModel> pModel);
	void ConstantBuffUpdate();

public:
	void ClearWorldResource();	// ���� �̵� �� ���ҽ� ����

private:
	void CreateConstantBuffer();
	void SortMeshInstance();

	// Environment //
	//void CreateIBL();
	void SetEnvironment();

	//?   Render    ///
	void ImguiRender();
	void RenderStaticMeshInstance(eShaderType shadertype);			// ������ ����ƽ �޽� ����
	void RenderSkeletalMeshInstance(eShaderType shadertype);			// ������ ���̷�Ż �޽� ����
	void RenderStaticMeshInstanceOpacity(eShaderType shadertype);		// ������ ����ƽ �޽� ����
	void RenderSkeletalMeshInstanceOpacity(eShaderType shadertype);	// ������ ���̷�Ż �޽� ����
	void RenderUIInstance();
	void RenderDebugDraw();
	void RenderEnvironment();

	//?   Util     ///
	void GetVideoMemoryInfo(std::string& out);
	void GetSystemMemoryInfo(std::string& out);

public:
	void DrawTextRequest(std::wstring_view text,
		FLOAT x,
		FLOAT y,
		D2D1::ColorF color,
		std::wstring_view fontFamily,
		FLOAT fontSize,
		DWRITE_FONT_WEIGHT fontWeight = DWRITE_FONT_WEIGHT_NORMAL,
		DWRITE_FONT_STYLE fontStyle = DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH fontStretch = DWRITE_FONT_STRETCH_NORMAL);

public:
	///   D3D   ///
	static D3DRenderManager* Instance;
	static ComPtr<ID3D11Device> m_pDevice;					 // ����̽�

private:
	ComPtr<ID3D11DeviceContext> m_pDeviceContext;
	ComPtr<IDXGIFactory4> m_pDXGIFactory;		// DXGI���丮
	ComPtr<IDXGIAdapter3> m_pDXGIAdapter;		// ����ī�� ������ ���� ������ �������̽�
	UINT m_ClientWidth;
	UINT m_ClientHeight;
	HWND m_hWnd;

	//?   CB Data   ///
	ComPtr<ID3D11Buffer> m_pBoolBuffer = nullptr;		    // ��� ����.
	ComPtr<ID3D11Buffer> m_pLightBuffer = nullptr;		    // ��� ����.
	ComPtr<ID3D11Buffer> m_pMatPalette = nullptr;		    // ��� ����.
	ComPtr<ID3D11Buffer> m_pIBL_Buffer = nullptr;			// ��� ����.
	ComPtr<ID3D11Buffer> m_pTransformW_Buffer = nullptr;		// ��� ����.
	ComPtr<ID3D11Buffer> m_pTransformVP_Buffer = nullptr;		// ��� ����.
	ComPtr<ID3D11Buffer> m_pUIAnimation_Buffer = nullptr;		// ��� ����.

	CB_TransformW m_TransformW;
	CB_TransformVP m_TransformVP;
	CB_IBL m_IBL;

	BoundingFrustum m_FrustumCamera;
	///   RENDERER   ///
	std::unique_ptr<TextRenderer> m_textRenderer;

	///   CONTAINER   ///
	std::list<std::shared_ptr<StaticMeshModel>> m_StaticMeshModels;			//  �������� �𵨵��� ������ �����صд�.
	std::list<std::shared_ptr<SkeletalMeshModel>> m_SkeletalMeshModels;		//  �������� �𵨵��� ������ �����صд�.

	std::list<StaticMeshInstance*> m_StaticMeshInstance;				//  �������� �𵨵��� ������ �����صд�.
	std::list<SkeletalMeshInstance*> m_SkeletalMeshInstance;			//  �������� �𵨵��� ������ �����صд�.
	std::list<StaticMeshInstance*> m_StaticMeshInstanceOpacity;			//  �������� ������ �𵨵��� ������ �����صд�.
	std::list<SkeletalMeshInstance*> m_SkeletalMeshInstanceOpacity;		//  �������� ������ �𵨵��� ������ �����صд�.
	std::list<std::shared_ptr<UIInstance>> m_UIInstanceList;		//  �������� ������ �𵨵��� ������ �����صд�.

	//std::shared_ptr<EnvironmentModel> m_pEnvironmentModel;
	//std::shared_ptr<EnvironmentModel> m_pLightEnvironment;

private:
	///  FOR SHADER  ///
	XMVECTOR m_Eye;
	XMVECTOR m_At;
	XMVECTOR m_Up;
	Matrix   m_View;		// ī�޶���ǥ�� �������� ��ȯ�� ���� ���.
	Matrix   m_Projection;	// ������ġ��ǥ��( Normalized Device Coordinate) �������� ��ȯ�� ���� ���.
	Matrix   m_Orthographic;// 2D �������� ���� ���翵 ���

	Math::Vector3   m_CamPosition;		// ī�޶� ��ġ

	//float m_Cam[3] = { 0.0f, 0.0f, -500.0f };
	float m_Fov = 45.0f;
	float m_Near = 0.01f;
	float m_Far = 30000.0f;

	float m_SpecularPower = 4.0f;
	float m_Ambient = 0.369f;

	float m_vLightColor[3] = { 1, 1, 1 };
	float m_vLightDir[3] = { 0, 0, 1 };

	bool m_isNormalMap = true;
	bool m_isSpecularMap = true;
	bool m_isGamma = true;
	bool m_isDiffuse = true;
	bool m_isEmissive = true;
	bool m_isOpacity = true;
	bool m_isMetalness = true;
	bool m_isRoughness = true;

	int m_nDrawRenderCount;

	bool m_IsFreezeCulling = false;
	bool m_IsDrawDebugCulling = true;
	bool m_IsDrawShadowFrustum = true;
	bool m_IsDrawDebugCollision = true;

	// ���� : Shadow
	Matrix m_ShadowView;
	Matrix m_ShadowProjection;
	Vector3 m_ShadowLookAt;
	Vector3 m_ShadowPos;
	float m_ShadowForwardDistFromCamera = 2000.f;
	float m_ShadowUpDistFromLookAt = 5000.f;

private:
	// ���� : �׷��Ƚ�, ����������
	std::shared_ptr<Graphics> m_pGraphics;
	std::shared_ptr<PipeLine> m_pPipeLine;
};
