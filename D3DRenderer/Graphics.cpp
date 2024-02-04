#include "pch.h"
#include "Graphics.h"

Graphics::Graphics()
{
}

Graphics::~Graphics()
{
}

void Graphics::Initalize(GraphicsInfo info)
{
	m_hWnd = info.m_hwnd;
	m_ClientWidth = info.m_ClientWidth;
	m_ClientHeight = info.m_ClientHeight;
	m_ClearColor = info.m_ClearColor;

	setViewPort();
	createDirect2D();
	createSwapChain();
	createRenderTargetView();
	createStencilAndDepth();
}

void Graphics::ShaderRenderBegin()
{
	// 현재 쉐이더 리소스 세팅을 언바인딩
	ID3D11ShaderResourceView* pSRV = nullptr;
	m_pDeviceContext->PSSetShaderResources(11, 1, &pSRV);
	SAFE_RELEASE(pSRV);

	m_pDeviceContext->RSSetViewports(1, &m_ShadowViewport);
	m_pDeviceContext->OMSetRenderTargets(0, NULL, m_pShadowDepthStencilView.Get());
	m_pDeviceContext->ClearDepthStencilView(m_pShadowDepthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.f, 0);
	m_pDeviceContext->PSSetShader(NULL, NULL, 0);
}

void Graphics::ShaderRenderEnd()
{
}

void Graphics::RenderBegin()
{
	// Draw계열 함수를 호출하기전에 렌더링 파이프라인에 필수 스테이지 설정을 해야한다
	float color[4] = { m_ClearColor.x, m_ClearColor.y, m_ClearColor.z, m_ClearColor.w };
	m_pDeviceContext->RSSetViewports(1, &m_Viewport);
	m_pDeviceContext->OMSetRenderTargets(1, m_pRenderTargetView.GetAddressOf(), m_pDepthStencilView.Get());
	m_pDeviceContext->ClearRenderTargetView(m_pRenderTargetView.Get(), color);
	m_pDeviceContext->ClearDepthStencilView(m_pDepthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);

	//d2d
	GetD2DRenderTarget()->BeginDraw();
}

void Graphics::RenderEnd()
{
	//d2d
	GetD2DRenderTarget()->EndDraw();

	HRESULT hr = m_pSwapChain->Present(0, 0);
	assert(SUCCEEDED(hr));
}

void Graphics::createSwapChain()
{
	DXGI_SWAP_CHAIN_DESC swapDesc = {};

	swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapDesc.OutputWindow = m_hWnd;	// 스왑체인 출력할 창 핸들 값.
	swapDesc.Windowed = true;		// 창 모드 여부 설정.
	swapDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	// 백버퍼(텍스처)의 가로/세로 크기 설정.
	swapDesc.BufferDesc.Width = m_ClientWidth;
	swapDesc.BufferDesc.Height = m_ClientHeight;
	// 화면 주사율 설정.

	swapDesc.BufferDesc.RefreshRate.Numerator = 0;
	swapDesc.BufferDesc.RefreshRate.Denominator = 0;
	// 샘플링 관련 설정.
	swapDesc.SampleDesc.Count = 1;
	swapDesc.SampleDesc.Quality = 0;

	swapDesc.BufferCount = 2;
	swapDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;	// Present()호출 후 디스플레이 화면에서 픽셀을 처리하는 옵션
	swapDesc.Flags |= DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	UINT creationFlags = D3D10_CREATE_DEVICE_BGRA_SUPPORT; //D2D 때문에 BGRA 서포트 해줘야함
#ifdef _DEBUG
	creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	// 1. 장치 생성.   2.스왑체인 생성. 3.장치 컨텍스트 생성.
	HR_T(D3D11CreateDeviceAndSwapChain
	(
		  NULL
		, D3D_DRIVER_TYPE_HARDWARE
		, NULL
		, creationFlags
		, NULL
		, NULL
		, D3D11_SDK_VERSION
		, &swapDesc
		, &m_pSwapChain
		, &m_pDevice
		, NULL
		, &m_pDeviceContext)
	);
}

void Graphics::createDirect2D()
{
	HR_T(DWriteCreateFactory(
		DWRITE_FACTORY_TYPE_SHARED,
		__uuidof(IDWriteFactory),
		reinterpret_cast<IUnknown**>(m_pdWriteFactory.GetAddressOf())
	));

	HR_T(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, m_pd2dFactory.GetAddressOf()));
}

void Graphics::createRenderTargetView()
{
	ComPtr<ID3D11Texture2D> pBackBufferTexture = nullptr;
	HR_T(m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBackBufferTexture));
	HR_T(m_pDevice->CreateRenderTargetView(pBackBufferTexture.Get(), NULL, m_pRenderTargetView.GetAddressOf()));  // 텍스처는 내부 참조 증가

	HR_T(m_pSwapChain->GetBuffer(0, IID_PPV_ARGS(m_pdxgiBackbuffer.GetAddressOf())));

	D2D1_RENDER_TARGET_PROPERTIES props =
		D2D1::RenderTargetProperties(
			D2D1_RENDER_TARGET_TYPE_DEFAULT,
			D2D1::PixelFormat(DXGI_FORMAT_R8G8B8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED),
			0,
			0);

	HR_T(m_pd2dFactory->CreateDxgiSurfaceRenderTarget(
		m_pdxgiBackbuffer.Get(),
		props,
		m_pd2dRenderTargetView.GetAddressOf()));
}

void Graphics::createStencilAndDepth()
{
	D3D11_TEXTURE2D_DESC descDepth = {};
	descDepth.Width = m_ClientWidth;
	descDepth.Height = m_ClientHeight;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;

	ID3D11Texture2D* textureDepthStencil = nullptr;
	HR_T(m_pDevice->CreateTexture2D(&descDepth, nullptr, &textureDepthStencil));

	// Create the depth stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
	descDSV.Format = descDepth.Format;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;
	HR_T(m_pDevice->CreateDepthStencilView(textureDepthStencil, &descDSV, &m_pDepthStencilView));
	SAFE_RELEASE(textureDepthStencil);

	// 그림자용 ShadowMap Texture와 SRV, DSV 생성
	{
		// Create Shadow map texture desc
		D3D11_TEXTURE2D_DESC texDesc = {};
		texDesc.Width = static_cast<float>(SHADOWMAP_SIZE);
		texDesc.Height = static_cast<float>(SHADOWMAP_SIZE);
		texDesc.MipLevels = 1;
		texDesc.ArraySize = 1;
		texDesc.Usage = D3D11_USAGE_DEFAULT;
		texDesc.Format = DXGI_FORMAT_R32_TYPELESS;
		texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
		texDesc.SampleDesc.Count = 1;
		texDesc.SampleDesc.Quality = 0;

		ID3D11Texture2D* pShadowMap = nullptr;
		HR_T(m_pDevice->CreateTexture2D(&texDesc, NULL, &pShadowMap));

		// Create the depth stencil view desc
		D3D11_DEPTH_STENCIL_VIEW_DESC ShadowdescDSV = {};
		ShadowdescDSV.Format = DXGI_FORMAT_D32_FLOAT;
		ShadowdescDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		HR_T(m_pDevice->CreateDepthStencilView(pShadowMap, &ShadowdescDSV, m_pShadowDepthStencilView.GetAddressOf()));

		// Create the depth stencil view desc
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = 1;
		HR_T(m_pDevice->CreateShaderResourceView(pShadowMap, &srvDesc, m_pShadowResourceView.GetAddressOf()));
		SAFE_RELEASE(pShadowMap);
	}
}

void Graphics::setViewPort()
{
	m_Viewport.TopLeftX = 0.f;
	m_Viewport.TopLeftY = 0.f;
	m_Viewport.Width = static_cast<float>(m_ClientWidth);
	m_Viewport.Height = static_cast<float>(m_ClientHeight);
	m_Viewport.MinDepth = 0.f;
	m_Viewport.MaxDepth = 1.f;

	m_ShadowViewport.TopLeftX = 0.f;
	m_ShadowViewport.TopLeftY = 0.f;
	m_ShadowViewport.Width = static_cast<float>(SHADOWMAP_SIZE);
	m_ShadowViewport.Height = static_cast<float>(SHADOWMAP_SIZE);
	m_ShadowViewport.MinDepth = 0.f;
	m_ShadowViewport.MaxDepth = 1.f;
}
