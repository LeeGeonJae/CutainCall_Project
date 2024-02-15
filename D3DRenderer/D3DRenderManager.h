#pragma once
#include "Graphics.h"

struct DebugPoint
{
	Vector3 m_Position;
	Color m_Color;
};

struct DebugLine
{
	Vector3 m_StartLine;
	Color m_StartColor;
	Vector3 m_EndLine;
	Color m_EndColor;
};

struct CB_BoolBuffer
{
	int UseNormalMap;
	int UseSpecularMap;
	int UseGamma;
	int UseDiffuseMap; //16

	int UseEmissiveMap;
	int UseOpacityMap;
	int UseMetalnessMap;
	int UseRoughnessMap; //16

	int UseCubeMap;
	float MetalnessValue;
	float RoughnessValue;
	int mapPad;
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

// 버텍스 셰이더에 옮겨주기 위해 사용하는, 휘발성 저장 공간.
// 메쉬가 사용하고 그 다음 메쉬가 사용하고 반복...하기때문에
// 휘발성 저장 공간이 하나만 있어도 됨.
//
// 여기에 저장되는 것은. 메쉬를 구성하는 버텍스들이 참조하는 모든 bone들의 offset * boneWorld 값들.
// 판단은 bone의 name이나 index로 같은지 판단해서 사용함.
struct CB_MatrixPalette
{
	Matrix Array[256];
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
class ParticleSystem;
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
	void AddParticle(std::shared_ptr<ParticleSystem> particle) { m_ParticleList.emplace_back(particle); }
	void AddDebugPoint(std::vector<DebugPoint>& point) { m_DebugPointList = point; }
	void AddDebugLine(std::vector<DebugLine>& line) { m_DebugLineList = line; }
	void AddUISkeletalMeshModel(std::shared_ptr<SkeletalMeshModel> skeletalMeshUIModel, int playerNumber) { m_UISkeletalMeshModels.emplace_back(skeletalMeshUIModel, playerNumber); }

	void SetEnvironmentModel(const std::shared_ptr<EnvironmentModel>& environmentModel);


public:
	//  Need  //
	bool Initialize(HWND hWnd);
	bool InitImGUI();
	bool InitD3D();
	void InitScene();

	//   Main Loop   //
	void Update(float deltaTime);		// 업데이트
	void ShadowRender();				// 그림자 렌더
	void Render();						// 모델링 렌더

	//  Utility  //
	void ApplyMaterial(Material* pMaterial);												// 머터리얼 GPU에 세팅하기
	void AddMeshInstance(std::shared_ptr<StaticMeshModel> pModel);							// 메시 인스턴스 세팅하기
	void AddMeshInstance(std::shared_ptr<SkeletalMeshModel> pModel);						// 메시 인스턴스 세팅하기
	void AddShadowMeshInstance(std::shared_ptr<StaticMeshModel> pModel);					// 그림자 메시 인스턴스 세팅하기
	void AddShadowMeshInstance(std::shared_ptr<SkeletalMeshModel> pModel);					// 그림자 메시 인스턴스 세팅하기
	void AddUIMeshInstance(std::shared_ptr<StaticMeshModel> pModel, int playerNumber);		// UI 메시 인스턴스 세팅하기
	void AddUIMeshInstance(std::shared_ptr<SkeletalMeshModel> pModel, int playerNumber);	// UI 메시 인스턴스 세팅하기
	void ConstantBuffUpdate();

public:
	void ClearWorldResource();	// 월드 이동 시 리소스 정리

private:
	void CreateConstantBuffer();
	void SortMeshInstance();

	// Environment //
	void SetEnvironment();

	//?   Render    ///
	void ImguiRender();													// ImGui 렌더
	void RenderStaticMeshInstance(eShaderType shadertype);				// 불투명 스태틱 메시 렌더
	void RenderSkeletalMeshInstance(eShaderType shadertype);			// 불투명 스켈레탈 메시 렌더
	void RenderStaticMeshInstanceOpacity(eShaderType shadertype);		// 반투명 스태틱 메시 렌더
	void RenderSkeletalMeshInstanceOpacity(eShaderType shadertype);		// 반투명 스켈레탈 메시 렌더
	void RenderUIMeshInstacne();										// UI 3D 모델링 메시 렌더
	void RenderUIInstance();											// UI 인스턴스 렌더
	void RenderDebugDraw();												// 디버그 렌더
	void RenderEnvironment();											// 환경맵 렌더
	void RenderParticleEffect();										// 파티클 렌더

	//?   Util     ///
	void GetVideoMemoryInfo(std::string& out);							// 비디오 메모리 데이터 구하기
	void GetSystemMemoryInfo(std::string& out);							// 시스템 메모리 데이터 구하기
	void UpdateMapUse();

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
	static ComPtr<ID3D11Device> m_pDevice;					 // 디바이스

private:
	ComPtr<ID3D11DeviceContext> m_pDeviceContext;
	ComPtr<IDXGIFactory4> m_pDXGIFactory;		// DXGI팩토리
	ComPtr<IDXGIAdapter3> m_pDXGIAdapter;		// 비디오카드 정보에 접근 가능한 인터페이스
	UINT m_ClientWidth;
	UINT m_ClientHeight;
	HWND m_hWnd;

	//?   CB Data   ///
	ComPtr<ID3D11Buffer> m_pBoolBuffer = nullptr;		    // 상수 버퍼.
	ComPtr<ID3D11Buffer> m_pLightBuffer = nullptr;		    // 상수 버퍼.
	ComPtr<ID3D11Buffer> m_pMatPalette = nullptr;		    // 상수 버퍼.
	ComPtr<ID3D11Buffer> m_pIBL_Buffer = nullptr;			// 상수 버퍼.
	ComPtr<ID3D11Buffer> m_pTransformW_Buffer = nullptr;	// 상수 버퍼.
	ComPtr<ID3D11Buffer> m_pTransformVP_Buffer = nullptr;	// 상수 버퍼.
	ComPtr<ID3D11Buffer> m_pUIAnimation_Buffer = nullptr;	// 상수 버퍼.
	ComPtr<ID3D11Buffer> m_pFadeInOut_Buffer = nullptr;		// 상수 버퍼.
	ComPtr<ID3D11Buffer> m_pParticleBuffer = nullptr;		// 상수 버퍼.
	ComPtr<ID3D11Buffer> m_pParticleDataBuffer = nullptr;	// 상수 버퍼.

	CB_TransformW m_TransformW;
	CB_TransformVP m_TransformVP;
	CB_IBL m_IBL;
	CB_BoolBuffer m_bIsMapUse;

	BoundingFrustum m_FrustumCamera;
	BoundingFrustum m_FrustumShadow;
	///   RENDERER   ///
	std::unique_ptr<TextRenderer> m_textRenderer;

	///   CONTAINER   ///
	std::list<std::shared_ptr<StaticMeshModel>>	m_StaticMeshModels;								//  렌더링할 모델들의 포인터 저장해둔다.
	std::list<std::shared_ptr<StaticMeshModel>>	m_DebugStaticMeshModels;						//  디버그 렌더링할 모델들의 포인터 저장해둔다.
	std::list<std::shared_ptr<SkeletalMeshModel>> m_SkeletalMeshModels;							//  렌더링할 모델들의 포인터 저장해둔다.
	std::list<std::shared_ptr<SkeletalMeshModel>> m_DebugSkeletalMeshModels;					//  디버그 렌더링할 모델들의 포인터 저장해둔다.
	std::list<std::pair<std::shared_ptr<SkeletalMeshModel>, int>>	m_UISkeletalMeshModels;		//  렌더링할 UI모델들의 포인터 저장해둔다.

	std::list<StaticMeshInstance*> m_StaticMeshInstance;										//  렌더링할 메시들의 포인터 저장해둔다.
	std::list<StaticMeshInstance*> m_ShadowStaticMeshInstance;									//  렌더링할 그림자 메시들의 포인터 저장해둔다.
	std::list<StaticMeshInstance*> m_StaticMeshInstanceOpacity;									//  렌더링할 반투명 메시들의 포인터 저장해둔다.
	std::list<SkeletalMeshInstance*> m_SkeletalMeshInstance;									//  렌더링할 메시들의 포인터 저장해둔다.
	std::list<SkeletalMeshInstance*> m_ShadowSkeletalMeshInstance;								//  렌더링할 그림자 메시들의 포인터 저장해둔다.
	std::list<SkeletalMeshInstance*> m_SkeletalMeshInstanceOpacity;								//  렌더링할 반투명 메시들의 포인터 저장해둔다.
	std::list<std::shared_ptr<UIInstance>> m_UIInstanceList;									//  렌더링할 UI들의 포인터 저장해둔다.
	std::list<std::shared_ptr<ParticleSystem>> m_ParticleList;									//  렌더링할 파티클들의 포인터 저장해둔다.
	std::vector<DebugPoint> m_DebugPointList;													//  렌더링할 파직스 디버그 포인트들의 포인터들을 저장해둔다.
	std::vector<DebugLine> m_DebugLineList;														//  렌더링할 파직스 디버그 라인들의 포인터들을 저장해둔다.
	std::list<std::pair<SkeletalMeshInstance*, int>> m_UISkeletalMeshInstance;					//  UI창에 렌더링할 3D 모델링 포인터들을 저장해둔다.
	std::list<std::pair<SkeletalMeshInstance*, int>> m_UISkeletalMeshInstanceOpacity;			//  UI창에 렌더링할 3D 모델링 포인터들을 저장해둔다.

	std::shared_ptr<EnvironmentModel> m_pEnvironmentModel;
	std::shared_ptr<EnvironmentModel> m_pLightEnvironment;

private:
	///  FOR SHADER  ///
	Matrix   m_View;					// 카메라좌표계 공간으로 변환을 위한 행렬.
	Matrix   m_Projection;				// 단위장치좌표계( Normalized Device Coordinate) 공간으로 변환을 위한 행렬.
	Matrix   m_Orthographic;			// 2D 렌더링을 위한 정사영 행렬

	Matrix	m_PlayerOneUITransform;		// Player UI Mesh 트랜스폼
	Matrix	m_PlayerTwoUITransform;		// Player UI Mesh 트랜스폼

	Math::Vector3   m_CamPosition;		// 카메라 위치

	//float m_Cam[3] = { 0.0f, 0.0f, -500.0f };
	float m_Fov = 45.0f;
	float m_Near = 0.1f;
	float m_Far = 1000000.0f;

	float m_SpecularPower = 4.0f;
	float m_Ambient = 0.369f;

	float m_vLightColor[3] = { 1, 1, 1 };
	float m_vLightDir[3] = { 0, 0, 1 };

	bool m_isUseEdit = false;
	bool m_isUseIBL = true;
	bool m_isCubeMap = true;
	bool m_isNormalMap = true;
	bool m_isSpecularMap = true;
	bool m_isGamma = true;
	bool m_isDiffuse = true;
	bool m_isEmissive = true;
	bool m_isOpacity = true;
	bool m_isMetalness = true;
	bool m_isRoughness = true;

	float m_MetalnessValue = 1.0f;
	float m_RoughnessValue = 1.0f;

	int m_nDrawRenderCount;

	bool m_IsFreezeCulling = false;
	bool m_IsDrawDebugCulling = true;
	bool m_IsDrawShadowFrustum = true;
	bool m_IsDrawDebugCollision = true;

	// 건재 : Shadow
	Matrix m_ShadowView;
	Matrix m_ShadowProjection;
	Vector3 m_ShadowLookAt;
	Vector3 m_ShadowPos;
	float m_ShadowForwardDistFromCamera = 2000.f;
	float m_ShadowUpDistFromLookAt = 5000.f;

	float m_GameTime;

private:
	// 건재 : 그래픽스, 파이프라인
	std::shared_ptr<Graphics> m_pGraphics;
	std::shared_ptr<PipeLine> m_pPipeLine;
};
