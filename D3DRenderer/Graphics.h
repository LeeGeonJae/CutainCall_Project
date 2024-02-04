#pragma once

// 그림자 맵 해상도
constexpr int SHADOWMAP_SIZE = 8192;

struct GraphicsInfo
{
	HWND m_hwnd;
	UINT m_ClientWidth = 800;
	UINT m_ClientHeight = 600;
	DirectX::SimpleMath::Vector4 m_ClearColor = DirectX::SimpleMath::Vector4(0.5f, 0.5f, 0.5f, 0.5f);
};

class Graphics
{
public:
	Graphics();
	~Graphics();

public:
	ComPtr<ID3D11Device> GetDevice() { return m_pDevice; }
	ComPtr<ID3D11DeviceContext> GetDeviceContext() { return m_pDeviceContext; }
	ComPtr<ID2D1RenderTarget> GetD2DRenderTarget() { return m_pd2dRenderTargetView; }
	ComPtr<IDXGISwapChain> GetSwapChain() { return m_pSwapChain; }
	ComPtr<IDWriteFactory> GetWriteFactory() { return m_pdWriteFactory; }
	D3D11_VIEWPORT GetViewport() { return m_Viewport; }
	ComPtr<ID3D11ShaderResourceView> GetShaderResourceView() { return m_pShadowResourceView; }

public:
	void Initalize(GraphicsInfo info);

	void ShaderRenderBegin();
	void ShaderRenderEnd();
	void RenderBegin();
	void RenderEnd();

private:
	void createSwapChain();
	void createDirect2D();
	void createRenderTargetView();
	void createStencilAndDepth();
	void setViewPort();

private:
	HWND m_hWnd;
	float m_ClientWidth;
	float m_ClientHeight;
	DirectX::SimpleMath::Vector4 m_ClearColor;

	ComPtr<ID3D11Device> m_pDevice;
	ComPtr<ID3D11DeviceContext> m_pDeviceContext = nullptr; // 즉시 디바이스 컨텍스트
	ComPtr<IDXGISwapChain> m_pSwapChain = nullptr;					// 스왑체인

	ComPtr<ID3D11RenderTargetView> m_pRenderTargetView = nullptr;	// 렌더링 타겟뷰
	ComPtr<ID3D11DepthStencilView> m_pDepthStencilView;				// 깊이 스텐실 뷰

	ComPtr<ID3D11DepthStencilView> m_pShadowDepthStencilView;		// 그림자 깊이 스텐실 뷰
	ComPtr<ID3D11ShaderResourceView> m_pShadowResourceView;			// 그림자 쉐이더 리소스 뷰

	ComPtr<ID2D1Factory> m_pd2dFactory;								//Direct2D
	ComPtr<ID2D1Device> m_pd2dDevice;
	ComPtr<ID2D1DeviceContext> m_pd2dDeviceContext;
	ComPtr<ID2D1RenderTarget> m_pd2dRenderTargetView;
	ComPtr<IDXGISurface> m_pdxgiBackbuffer;
	ComPtr<IDWriteFactory> m_pdWriteFactory;						//DirectWrite

	D3D11_VIEWPORT m_Viewport = {};
	D3D11_VIEWPORT m_ShadowViewport = {};							// 그림자 뷰포트
};

