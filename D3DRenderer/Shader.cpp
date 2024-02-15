#include "pch.h"
#include "Shader.h"

#include "D3DRenderManager.h"

Shader::Shader()
	:m_ShaderType(eShaderType::END)
{
}

Shader::~Shader()
{
}

void Shader::Initalize(eShaderType type)
{
	m_ShaderType = type;

	CreateVS_IL();
	CreatePS();
}

void Shader::PipeLineSetting()
{
	D3DRenderManager::Instance->GetDevcon()->IASetInputLayout(m_pInputLayout.Get());
	D3DRenderManager::Instance->GetDevcon()->VSSetShader(m_pVertexShader.Get(), nullptr, 0);
	D3DRenderManager::Instance->GetDevcon()->PSSetShader(m_pPixelShader.Get(), nullptr, 0);
}

void Shader::CreateVS_IL()
{
	// 건재 : 스켈레탈 버텍스 쉐이더
	if (m_ShaderType == eShaderType::SKELETAL_SHADER)
	{
		D3D_SHADER_MACRO defines[] =
		{
			{"VERTEX_SKINNING",""}, // 매크로 이름과 값을 설정
			{nullptr, nullptr}    // 배열의 끝을 나타내기 위해 nullptr로 끝낸다.
		};

		D3D11_INPUT_ELEMENT_DESC layout[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "BLENDINDICES", 0, DXGI_FORMAT_R32G32B32A32_SINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "BLENDWEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		ID3D10Blob* vertexShaderBuffer = nullptr;	// 정점 셰이더 코드가 저장될 버퍼.
		HR_T(CompileShaderFromFile(L"../Shader/BasicVertexShader.hlsl", "main", "vs_5_0", &vertexShaderBuffer, defines));
		HR_T(D3DRenderManager::m_pDevice->CreateInputLayout(layout, ARRAYSIZE(layout),
			vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), m_pInputLayout.GetAddressOf()));

		HR_T(D3DRenderManager::m_pDevice->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(),
			vertexShaderBuffer->GetBufferSize(), NULL, m_pVertexShader.GetAddressOf()));
		SAFE_RELEASE(vertexShaderBuffer);
	}
	// 건재 : 스태틱 버텍스 쉐이더
	else if (m_ShaderType == eShaderType::STATIC_SHADER)
	{
		D3D11_INPUT_ELEMENT_DESC layout[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		ID3D10Blob* vertexShaderBuffer = nullptr;	// 정점 셰이더 코드가 저장될 버퍼.
		HR_T(CompileShaderFromFile(L"../Shader/BasicVertexShader.hlsl", "main", "vs_5_0", &vertexShaderBuffer, nullptr));
		HR_T(D3DRenderManager::m_pDevice->CreateInputLayout(layout, ARRAYSIZE(layout),
			vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), m_pInputLayout.GetAddressOf()));

		HR_T(D3DRenderManager::m_pDevice->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(),
			vertexShaderBuffer->GetBufferSize(), NULL, m_pVertexShader.GetAddressOf()));
		SAFE_RELEASE(vertexShaderBuffer);
	}
	// 건재 : 그림자 스켈레탈 버텍스 쉐이더
	else if (m_ShaderType == eShaderType::SKELETAL_SHADOW_SHADER)
	{
		D3D_SHADER_MACRO defines[] =
		{
			{"VERTEX_SKINNING",""}, // 매크로 이름과 값을 설정
			{nullptr, nullptr}    // 배열의 끝을 나타내기 위해 nullptr로 끝낸다.
		};

		D3D11_INPUT_ELEMENT_DESC layout[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "BLENDINDICES", 0, DXGI_FORMAT_R32G32B32A32_SINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "BLENDWEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		ID3D10Blob* vertexShaderBuffer = nullptr;	// 정점 셰이더 코드가 저장될 버퍼.
		HR_T(CompileShaderFromFile(L"../Shader/SkeletalMeshShadowShader.hlsl", "VS", "vs_5_0", &vertexShaderBuffer, defines));
		HR_T(D3DRenderManager::m_pDevice->CreateInputLayout(layout, ARRAYSIZE(layout),
			vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), m_pInputLayout.GetAddressOf()));

		HR_T(D3DRenderManager::m_pDevice->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(),
			vertexShaderBuffer->GetBufferSize(), NULL, m_pVertexShader.GetAddressOf()));
		SAFE_RELEASE(vertexShaderBuffer);
	}
	// 건재 : 그림자 스태틱 버텍스 쉐이더
	else if (m_ShaderType == eShaderType::STATIC_SHADOW_SHADER)
	{
		D3D11_INPUT_ELEMENT_DESC layout[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		ID3D10Blob* vertexShaderBuffer = nullptr;	// 정점 셰이더 코드가 저장될 버퍼.
		HR_T(CompileShaderFromFile(L"../Shader/StaticMeshShadowShader.hlsl", "VS", "vs_5_0", &vertexShaderBuffer, nullptr));
		HR_T(D3DRenderManager::m_pDevice->CreateInputLayout(layout, ARRAYSIZE(layout),
			vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), m_pInputLayout.GetAddressOf()));

		HR_T(D3DRenderManager::m_pDevice->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(),
			vertexShaderBuffer->GetBufferSize(), NULL, m_pVertexShader.GetAddressOf()));
		SAFE_RELEASE(vertexShaderBuffer);
	}
	// 건재 : 엔비론먼트 버텍스 쉐이더
	else if (m_ShaderType == eShaderType::ENVIRONMENT_SHADER)
	{
		D3D_SHADER_MACRO defines[] =
		{
			{"",""}, // 매크로 이름과 값을 설정
			{nullptr, nullptr}    // 배열의 끝을 나타내기 위해 nullptr로 끝낸다.
		};

		D3D11_INPUT_ELEMENT_DESC layout[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		ID3D10Blob* vertexShaderBuffer = nullptr;	// 정점 셰이더 코드가 저장될 버퍼.
		HR_T(CompileShaderFromFile(L"../Shader/VS_Environment.hlsl", "main", "vs_5_0", &vertexShaderBuffer, defines));
		HR_T(D3DRenderManager::m_pDevice->CreateInputLayout(layout, ARRAYSIZE(layout),
			vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), m_pInputLayout.GetAddressOf()));

		HR_T(D3DRenderManager::m_pDevice->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, m_pVertexShader.GetAddressOf()));
		SAFE_RELEASE(vertexShaderBuffer);
	}
	// 건재 : UI 버텍스 쉐이더
	else if (m_ShaderType == eShaderType::UI_SHADER)
	{
		D3D11_INPUT_ELEMENT_DESC layout[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};

		ID3D10Blob* vertexShaderBuffer = nullptr;	// 정점 셰이더 코드가 저장될 버퍼.
		HR_T(CompileShaderFromFile(L"../Shader/UIVertexShader.hlsl", "main", "vs_5_0", &vertexShaderBuffer, nullptr));
		HR_T(D3DRenderManager::m_pDevice->CreateInputLayout(layout, ARRAYSIZE(layout),
			vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), m_pInputLayout.GetAddressOf()));

		HR_T(D3DRenderManager::m_pDevice->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(),
			vertexShaderBuffer->GetBufferSize(), NULL, m_pVertexShader.GetAddressOf()));
		SAFE_RELEASE(vertexShaderBuffer);
	}
	else if (m_ShaderType == eShaderType::UI_MESH_SHADER)
	{
		D3D_SHADER_MACRO defines[] =
		{
			{"VERTEX_SKINNING",""}, // 매크로 이름과 값을 설정
			{nullptr, nullptr}    // 배열의 끝을 나타내기 위해 nullptr로 끝낸다.
		};

		D3D11_INPUT_ELEMENT_DESC layout[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "BLENDINDICES", 0, DXGI_FORMAT_R32G32B32A32_SINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "BLENDWEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		ID3D10Blob* vertexShaderBuffer = nullptr;	// 정점 셰이더 코드가 저장될 버퍼.
		HR_T(CompileShaderFromFile(L"../Shader/BasicVertexShader.hlsl", "main", "vs_5_0", &vertexShaderBuffer, defines));
		HR_T(D3DRenderManager::m_pDevice->CreateInputLayout(layout, ARRAYSIZE(layout),
			vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), m_pInputLayout.GetAddressOf()));

		HR_T(D3DRenderManager::m_pDevice->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(),
			vertexShaderBuffer->GetBufferSize(), NULL, m_pVertexShader.GetAddressOf()));
		SAFE_RELEASE(vertexShaderBuffer);
	}
}

void Shader::CreatePS()
{
	// 건재 : 스태틱 & 스켈레탈 픽셀 쉐이더 (PBR)
	if (m_ShaderType == eShaderType::STATIC_SHADER  || m_ShaderType == eShaderType::SKELETAL_SHADER)
	{
		ComPtr<ID3D10Blob> pixelShaderBuffer = nullptr;	// 픽셀 셰이더 코드가 저장될 버퍼.
		HR_T(CompileShaderFromFile(L"../Shader/PBR_PixelShader.hlsl", "main", "ps_5_0", pixelShaderBuffer.GetAddressOf(), nullptr));
		HR_T(D3DRenderManager::m_pDevice->CreatePixelShader(
			pixelShaderBuffer->GetBufferPointer(),
			pixelShaderBuffer->GetBufferSize(), NULL, m_pPixelShader.GetAddressOf()));
	}
	// 건재 : 스태틱 그림자 쉐이더
	else if (m_ShaderType == eShaderType::STATIC_SHADOW_SHADER)
	{
		ComPtr<ID3D10Blob> buffer = nullptr;
		HR_T(CompileShaderFromFile(L"../Shader/StaticMeshShadowShader.hlsl", "PS", "ps_5_0", buffer.GetAddressOf(), nullptr));
		HR_T(D3DRenderManager::m_pDevice->CreatePixelShader(buffer->GetBufferPointer(), buffer->GetBufferSize(), NULL, m_pPixelShader.GetAddressOf()));
	}
	// 건재 : 스켈레탈 그림자 쉐이더
	else if (m_ShaderType == eShaderType::SKELETAL_SHADOW_SHADER)
	{
		ComPtr<ID3D10Blob> buffer = nullptr;
		HR_T(CompileShaderFromFile(L"../Shader/SkeletalMeshShadowShader.hlsl", "PS", "ps_5_0", buffer.GetAddressOf(), nullptr));
		HR_T(D3DRenderManager::m_pDevice->CreatePixelShader(buffer->GetBufferPointer(), buffer->GetBufferSize(), NULL, m_pPixelShader.GetAddressOf()));
	}
	// 건재 : 엔비론먼트 픽셀 쉐이더
	else if (m_ShaderType == eShaderType::ENVIRONMENT_SHADER )
	{
		ComPtr<ID3D10Blob> buffer = nullptr;
		HR_T(CompileShaderFromFile(L"../Shader/PS_Environment.hlsl", "main", "ps_5_0", buffer.GetAddressOf(), nullptr));
		HR_T(D3DRenderManager::m_pDevice->CreatePixelShader(buffer->GetBufferPointer(), buffer->GetBufferSize(), NULL, m_pPixelShader.GetAddressOf()));
	}
	// 건재 : UI 픽셀 쉐이더
	else if (m_ShaderType == eShaderType::UI_SHADER)
	{
		ComPtr<ID3D10Blob> buffer = nullptr;
		HR_T(CompileShaderFromFile(L"../Shader/UIPixelShader.hlsl", "main", "ps_5_0", buffer.GetAddressOf(), nullptr));
		HR_T(D3DRenderManager::m_pDevice->CreatePixelShader(buffer->GetBufferPointer(), buffer->GetBufferSize(), NULL, m_pPixelShader.GetAddressOf()));
	}
	// 건재 : UI 픽셀 쉐이더
	else if (m_ShaderType == eShaderType::UI_MESH_SHADER)
	{
		ComPtr<ID3D10Blob> buffer = nullptr;
		HR_T(CompileShaderFromFile(L"../Shader/UIMeshPixelShader.hlsl", "main", "ps_5_0", buffer.GetAddressOf(), nullptr));
		HR_T(D3DRenderManager::m_pDevice->CreatePixelShader(buffer->GetBufferPointer(), buffer->GetBufferSize(), NULL, m_pPixelShader.GetAddressOf()));
	}
}
