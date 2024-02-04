#include "pch.h"
#include "Helper.h"
#include "StaticMeshModel.h"
#include "SkeletalMeshModel.h"
#include "D3DRenderManager.h"
#include "Material.h"
#include "ModelResource.h"
#include "EnvironmentModel.h"
#include "ResourceManager.h"
#include "Graphics.h"
#include "PipeLine.h"
#include "DebugDraw.h"
#include "UIInstance.h"
#include "TextRenderer.h"

#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>

#pragma comment (lib, "DXGI.lib")  // for  CreateDXGIFactory1 �Լ�
#pragma comment (lib, "d2d1.lib")
#pragma comment (lib, "dwrite.lib")

#define USE_FLIPMODE 1

D3DRenderManager* D3DRenderManager::Instance = nullptr;
ComPtr<ID3D11Device> D3DRenderManager::m_pDevice = nullptr;

// ModelResource -> ������ �������� ������ ���
// MeshInstance  -> ������ �ϱ� ���� �� �޽��� ������
// MeshComponent -> �����͸� ��� �ִ� ������

// ������
D3DRenderManager::D3DRenderManager(UINT Width, UINT Height)
{
	m_ClientHeight = {};
	m_ClientWidth = {};
	m_At = {};
	m_Eye = {};
	m_Up = {};
	m_hWnd = {};

	assert(Instance == nullptr);
	Instance = this;

	m_ClientWidth = Width;
	m_ClientHeight = Height;
}

// �Ҹ���
D3DRenderManager::~D3DRenderManager()
{
	for (auto componentPtr : m_SkeletalMeshModels)
	{
		componentPtr.reset();
	}
	for (auto componentPtr : m_StaticMeshModels)
	{
		componentPtr.reset();
	}

	m_SkeletalMeshModels.clear();
	m_StaticMeshModels.clear();

	ResourceManager::GetInstance()->Finalize();

	DebugDraw::Uninitialize();

	// Cleanup Imgui
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

// ������Ʈ
void D3DRenderManager::Update(float deltaTime)
{
	float fov = m_Fov / 180.0f * 3.14f;
	float aspectRatio = m_ClientWidth / (FLOAT)m_ClientHeight;
	m_Projection = XMMatrixPerspectiveFovLH(fov, aspectRatio, m_Near, m_Far);

	ConstantBuffUpdate();

	m_nDrawRenderCount = 0;
	for (auto& StaticMeshModel : m_StaticMeshModels)	// deleteGameObject �Ŀ� �� ������ ����
	{
		if (m_FrustumCamera.Intersects(StaticMeshModel->m_BoundingBox))
		{
			// �ϳ��� �޽� ������Ʈ�� �������� �޽� Instance �� ����������.
			AddMeshInstance(StaticMeshModel);
			StaticMeshModel->m_IsCulled = true;
			//StaticMeshComponent->Update(deltaTime);
			m_nDrawRenderCount++;
		}
		else
		{
			StaticMeshModel->m_IsCulled = false;
		}
	}

	for (auto& SkeletalMeshModel : m_SkeletalMeshModels)
	{
		if (m_FrustumCamera.Intersects(SkeletalMeshModel->m_BoundingBox))
		{
			// �ϳ��� �޽� ������Ʈ�� �������� �޽� Instance �� ����������.
			AddMeshInstance(SkeletalMeshModel);
			SkeletalMeshModel->m_IsCulled = true;
			//SkeletalMeshComponent->Update(deltaTime);
			m_nDrawRenderCount++;
		}
		else
		{
			SkeletalMeshModel->m_IsCulled = false;
		}
	}

	SortMeshInstance();

	// UIInstance Animation ����� ���� ������Ʈ
	for (auto& ui : m_UIInstanceList)
	{
		ui->Update(deltaTime);
	}

	//if (m_pEnvironmentModel != nullptr)
	//{
	//	m_pEnvironmentModel->Update(deltaTime);
	//}


	m_StaticMeshModels.clear();
	m_SkeletalMeshModels.clear();
}

// ��� ���� ������Ʈ ( ��, ī�޶� )
void D3DRenderManager::ConstantBuffUpdate()
{
	///  ConstantBuffer Binding  ///
	CB_BoolBuffer CB_Bool;
	CB_Bool.UseGamma = m_isGamma;

	CB_LightDirBuffer CB_Light;
	CB_Light.vLightColor.x = m_vLightColor[0];
	CB_Light.vLightColor.y = m_vLightColor[1];
	CB_Light.vLightColor.z = m_vLightColor[2];
	CB_Light.vLightColor.w = 1.0f;

	CB_Light.vLightDir.x = m_vLightDir[0];
	CB_Light.vLightDir.y = m_vLightDir[1];
	CB_Light.vLightDir.z = m_vLightDir[2];
	CB_Light.vLightDir.w = 1.0f;

	CB_Light.vLightDir.Normalize();

//  // ToDo �ӽ÷� ī�޶� ��ġ ��������. ������ �ð� ������ ����.
	CB_Light.mWorldCameraPosition = XMVectorSet(m_CamPosition.x, m_CamPosition.y, m_CamPosition.z, 0.0f);
	//	CB_Light.mWorldCameraPosition = XMVectorSet(-m_View._41, -m_View._42, -m_View._43, 0.0f);

		// ī�޶� �ø� �������� ī�޶� �����
	if (!m_IsFreezeCulling)
	{
		BoundingFrustum::CreateFromMatrix(m_FrustumCamera, m_Projection);
		m_FrustumCamera.Transform(m_FrustumCamera, m_View.Invert());
	}
	// ī�޶�
	{
		m_TransformVP.m_View = XMMatrixTranspose(m_View);
		m_TransformVP.m_Projection = XMMatrixTranspose(m_Projection);
	}
	// �׸���
	{
		Vector3 lightDir;
		memcpy(&lightDir, m_vLightDir, sizeof(Vector3));

		m_ShadowProjection = XMMatrixPerspectiveFovLH(XM_PIDIV4, SHADOWMAP_SIZE / SHADOWMAP_SIZE, 2000.f, 400000.f);
		m_ShadowLookAt = { 0.f, 0.f, 0.f };
		m_ShadowPos = m_ShadowLookAt + (-lightDir * m_ShadowUpDistFromLookAt);
		m_ShadowView = XMMatrixLookAtLH(m_ShadowPos, m_ShadowLookAt, Vector3(0.f, 1.f, 0.f));

		m_TransformVP.m_ShadowView = m_ShadowView.Transpose();
		m_TransformVP.m_ShadowProjection = m_ShadowProjection.Transpose();
	}

	m_pGraphics->GetDeviceContext()->UpdateSubresource(m_pTransformVP_Buffer.Get(), 0, nullptr, &m_TransformVP, 0, 0);
	m_pGraphics->GetDeviceContext()->UpdateSubresource(m_pIBL_Buffer.Get(), 0, nullptr, &m_IBL, 0, 0);
	m_pGraphics->GetDeviceContext()->UpdateSubresource(m_pLightBuffer.Get(), 0, nullptr, &CB_Light, 0, 0);
}

void D3DRenderManager::AddMeshInstance(std::shared_ptr<StaticMeshModel> pModel)
{
	// ����ƽ �޽� �߿��� ���Ľ�Ƽ(����)�� �ִ� �޽ô� StaticMeshInstanceOpacity ����Ʈ�� ����
	// �ƴϸ� �������� ����ƽ �޽ôϱ� StaticMeshInstance ����Ʈ�� ����
	for (size_t i = 0; i < pModel->m_MeshInstances.size(); i++)
	{
		auto* pMeshInstance = &pModel->m_MeshInstances[i];

		if (pMeshInstance->m_pMaterial->m_MaterialMapFlags & MaterialMapFlags::OPACITY)
		{
			m_StaticMeshInstanceOpacity.push_back(&pModel->m_MeshInstances[i]);
		}
		else
		{
			m_StaticMeshInstance.push_back(&pModel->m_MeshInstances[i]);
		}
	}
}

void D3DRenderManager::AddMeshInstance(std::shared_ptr<SkeletalMeshModel> pModel)
{
	// ����ƽ �޽� �߿��� ���Ľ�Ƽ(����)�� �ִ� �޽ô� m_SkeletalMeshInstanceOpacity ����Ʈ�� ����
	// �ƴϸ� �������� ����ƽ �޽ôϱ� m_SkeletalMeshInstance ����Ʈ�� ����
	for (size_t i = 0; i < pModel->m_MeshInstances.size(); i++)
	{
		auto* pMeshInstance = &pModel->m_MeshInstances[i];

		if (pMeshInstance->m_pMaterial->m_MaterialMapFlags & MaterialMapFlags::OPACITY)
		{
			m_SkeletalMeshInstanceOpacity.push_back(&pModel->m_MeshInstances[i]);
		}
		else
		{
			m_SkeletalMeshInstance.push_back(&pModel->m_MeshInstances[i]);
		}
	}
}

void D3DRenderManager::ShadowRender()
{
	m_pGraphics->ShaderRenderBegin();
	m_pGraphics->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_pGraphics->GetDeviceContext()->VSSetConstantBuffers(3, 1, m_pTransformVP_Buffer.GetAddressOf());

	// ���������ο� ����ƽ �޽� ���̴��� InputLayout, VS_Shader, PS_Shader ����
	RenderStaticMeshInstance(eShaderType::STATIC_SHADOW_SHADER);				// ������ ����ƽ �޽�
	RenderSkeletalMeshInstance(eShaderType::SKELETAL_SHADOW_SHADER);			// ������ ���̷�Ż �޽�
	m_pDeviceContext->OMSetDepthStencilState(m_pPipeLine->GetDisableDSS().Get(), 1);
	RenderStaticMeshInstanceOpacity(eShaderType::STATIC_SHADOW_SHADER);		// ������ ����ƽ �޽�
	RenderSkeletalMeshInstanceOpacity(eShaderType::SKELETAL_SHADOW_SHADER);	// ������ ����ƽ �޽�
	m_pDeviceContext->OMSetDepthStencilState(m_pPipeLine->GetLessEqualDSS().Get(), 0);

	m_pGraphics->ShaderRenderEnd();
}

// ����
void D3DRenderManager::Render()
{
	// �׸��� �� ����
	ShadowRender();

	// RenderTargetView�ϰ� SwapChain�� �ִ� Graphics���� RenderBegin�� RenderEnd�Լ� ȣ��
	m_pGraphics->RenderBegin();
	m_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// vertex shader
	m_pDeviceContext->VSSetConstantBuffers(0, 1, m_pIBL_Buffer.GetAddressOf());
	m_pDeviceContext->VSSetConstantBuffers(1, 1, m_pBoolBuffer.GetAddressOf());
	m_pDeviceContext->VSSetConstantBuffers(2, 1, m_pTransformW_Buffer.GetAddressOf());
	m_pDeviceContext->VSSetConstantBuffers(3, 1, m_pTransformVP_Buffer.GetAddressOf());
	m_pDeviceContext->VSSetConstantBuffers(4, 1, m_pLightBuffer.GetAddressOf());
	m_pDeviceContext->VSSetConstantBuffers(5, 1, m_pMatPalette.GetAddressOf());
	m_pDeviceContext->VSSetConstantBuffers(6, 1, m_pUIAnimation_Buffer.GetAddressOf());

	// pixel shader
	m_pDeviceContext->PSSetConstantBuffers(0, 1, m_pIBL_Buffer.GetAddressOf());
	m_pDeviceContext->PSSetConstantBuffers(1, 1, m_pBoolBuffer.GetAddressOf());
	m_pDeviceContext->PSSetConstantBuffers(2, 1, m_pTransformW_Buffer.GetAddressOf());
	m_pDeviceContext->PSSetConstantBuffers(3, 1, m_pTransformVP_Buffer.GetAddressOf());
	m_pDeviceContext->PSSetConstantBuffers(4, 1, m_pLightBuffer.GetAddressOf());


	/// ȯ��� ���� ///
	RenderEnvironment();

	//----------------//


	/// 3D ���� ///
	// ����ť ����
	// ���������ο� ����ƽ �޽� ���̴��� InputLayout, VS_Shader, PS_Shader ����
	RenderStaticMeshInstance(eShaderType::STATIC_SHADER);				// ������ ����ƽ �޽�
	RenderSkeletalMeshInstance(eShaderType::SKELETAL_SHADER);			// ������ ���̷�Ż �޽�
	m_pDeviceContext->OMSetDepthStencilState(m_pPipeLine->GetDisableDSS().Get(), 1);
	RenderStaticMeshInstanceOpacity(eShaderType::STATIC_SHADER);		// ������ ����ƽ �޽�
	RenderSkeletalMeshInstanceOpacity(eShaderType::SKELETAL_SHADER);	// ������ ���̷�Ż �޽�
	m_pDeviceContext->OMSetDepthStencilState(m_pPipeLine->GetLessEqualDSS().Get(), 0);

	//----------------//
	/// 2D ����  ///
	// 3D�� �ƴ� 2D�� �������ϱ� ���� ���� -> Orthographic
	m_TransformVP.m_Projection = m_Orthographic;
	RenderUIInstance();
	m_textRenderer->DrawTextInQueue();
	RenderDebugDraw();
	ImguiRender();

	//----------------//

	m_StaticMeshInstance.clear();
	m_SkeletalMeshInstance.clear();
	m_StaticMeshInstanceOpacity.clear();
	m_SkeletalMeshInstanceOpacity.clear();
	m_UIInstanceList.clear();

	// ���� : ���� ����
	m_pGraphics->RenderEnd();
}

// ������ ����ƽ �޽� �ν��Ͻ� ����
void D3DRenderManager::RenderStaticMeshInstance(eShaderType shadertype)
{
	shared_ptr<Shader> staticshader;

	if (shadertype == eShaderType::STATIC_SHADER)
	{
		staticshader = ResourceManager::GetInstance()->CreateShaderResource("StaticMeshShader", shadertype);
		staticshader->PipeLineSetting();

		m_pDeviceContext->PSSetShaderResources(11, 1, m_pGraphics->GetShaderResourceView().GetAddressOf());
	}
	else if (shadertype == eShaderType::STATIC_SHADOW_SHADER)
	{
		staticshader = ResourceManager::GetInstance()->CreateShaderResource("ShadowStaticMeshShader", shadertype);
		staticshader->PipeLineSetting();
	}

	// ���� : ���� ���͸���� ���� ���͸����� ���� ���͸����� �ٲ� ��쿡 ���͸��� ����
	Material* pPrevMaterial = nullptr;
	for (const auto& meshInstance : m_StaticMeshInstance)
	{
		if (pPrevMaterial != meshInstance->m_pMaterial)
		{
			ApplyMaterial(meshInstance->m_pMaterial);	// ���͸��� ����
			pPrevMaterial = meshInstance->m_pMaterial;
		}

		//? Static Mesh
		m_TransformW.mWorld = meshInstance->m_pNodeWorldTransform->Transpose();
		m_pGraphics->GetDeviceContext()->UpdateSubresource(m_pTransformW_Buffer.Get(), 0, nullptr, &m_TransformW, 0, 0);

		// Draw
		meshInstance->Render(m_pGraphics->GetDeviceContext().Get());
	}
}

// ������ ���̷�Ż �޽� �ν��Ͻ� ����
void D3DRenderManager::RenderSkeletalMeshInstance(eShaderType shadertype)
{
	shared_ptr<Shader> skeletalshader;

	if (shadertype == eShaderType::SKELETAL_SHADER)
	{
		skeletalshader = ResourceManager::GetInstance()->CreateShaderResource("SkeletalMeshShader", shadertype);
		skeletalshader->PipeLineSetting();

		m_pDeviceContext->PSSetShaderResources(11, 1, m_pGraphics->GetShaderResourceView().GetAddressOf());
	}
	else if (shadertype == eShaderType::SKELETAL_SHADOW_SHADER)
	{
		skeletalshader = ResourceManager::GetInstance()->CreateShaderResource("ShadowSkeletalMeshShader", shadertype);
		skeletalshader->PipeLineSetting();
	}

	// ���� ���͸���� ���� ���͸����� ���� ���͸����� �ٲ� ��쿡 ���͸��� ����
	Material* pPrevMaterial = nullptr;
	for (const auto& meshInstance : m_SkeletalMeshInstance)
	{
		if (pPrevMaterial != meshInstance->m_pMaterial)
		{
			ApplyMaterial(meshInstance->m_pMaterial);	// ���͸��� ����
			pPrevMaterial = meshInstance->m_pMaterial;
		}

		//? Skeletal Mesh
		CB_MatrixPalette CB_MatPalatte;
		meshInstance->UpdateMatrixPallete(CB_MatPalatte.Array);
		m_pGraphics->GetDeviceContext()->UpdateSubresource(m_pMatPalette.Get(), 0, nullptr, &CB_MatPalatte, 0, 0);

		// Draw
		meshInstance->Render(m_pGraphics->GetDeviceContext().Get());
	}
}

// ������ ����ƽ �޽� �ν��Ͻ� ����
void D3DRenderManager::RenderStaticMeshInstanceOpacity(eShaderType shadertype)
{
	shared_ptr<Shader> staticshader;

	if (shadertype == eShaderType::STATIC_SHADER)
	{
		staticshader = ResourceManager::GetInstance()->CreateShaderResource("StaticMeshShader", shadertype);
		staticshader->PipeLineSetting();

		m_pDeviceContext->PSSetShaderResources(11, 1, m_pGraphics->GetShaderResourceView().GetAddressOf());
	}
	else if (shadertype == eShaderType::STATIC_SHADOW_SHADER)
	{
		staticshader = ResourceManager::GetInstance()->CreateShaderResource("ShadowStaticMeshShader", shadertype);
		staticshader->PipeLineSetting();
	}

	// ���� ���͸���� ���� ���͸����� ���� ���͸����� �ٲ� ��쿡 ���͸��� ����
	Material* pPrevMaterial = nullptr;
	for (const auto& meshInstance : m_StaticMeshInstanceOpacity)
	{
		if (pPrevMaterial != meshInstance->m_pMaterial)
		{
			ApplyMaterial(meshInstance->m_pMaterial);	// ���͸��� ����
			pPrevMaterial = meshInstance->m_pMaterial;
		}

		//? Static Mesh
		m_TransformW.mWorld = meshInstance->m_pNodeWorldTransform->Transpose();
		m_pGraphics->GetDeviceContext()->UpdateSubresource(m_pTransformW_Buffer.Get(), 0, nullptr, &m_TransformW, 0, 0);

		// Draw
		meshInstance->Render(m_pGraphics->GetDeviceContext().Get());
	}
}

// ������ ���̷�Ż �޽� �ν��Ͻ� ����
void D3DRenderManager::RenderSkeletalMeshInstanceOpacity(eShaderType shadertype)
{
	shared_ptr<Shader> skeletalshader;

	if (shadertype == eShaderType::SKELETAL_SHADER)
	{
		skeletalshader = ResourceManager::GetInstance()->CreateShaderResource("SkeletalMeshShader", shadertype);
		skeletalshader->PipeLineSetting();

		m_pDeviceContext->PSSetShaderResources(11, 1, m_pGraphics->GetShaderResourceView().GetAddressOf());
	}
	else if (shadertype == eShaderType::SKELETAL_SHADOW_SHADER)
	{
		skeletalshader = ResourceManager::GetInstance()->CreateShaderResource("ShadowSkeletalMeshShader", shadertype);
		skeletalshader->PipeLineSetting();
	}

	// ���� ���͸���� ���� ���͸����� ���� ���͸����� �ٲ� ��쿡 ���͸��� ����
	Material* pPrevMaterial = nullptr;
	for (const auto& meshInstance : m_SkeletalMeshInstanceOpacity)
	{
		if (pPrevMaterial != meshInstance->m_pMaterial)
		{
			ApplyMaterial(meshInstance->m_pMaterial);	// ���͸��� ����
			pPrevMaterial = meshInstance->m_pMaterial;
		}

		//? Skeletal Mesh
		CB_MatrixPalette CB_MatPalatte;
		meshInstance->UpdateMatrixPallete(CB_MatPalatte.Array);
		m_pGraphics->GetDeviceContext()->UpdateSubresource(m_pMatPalette.Get(), 0, nullptr, &CB_MatPalatte, 0, 0);

		// Draw
		meshInstance->Render(m_pGraphics->GetDeviceContext().Get());
	}
}

void D3DRenderManager::RenderUIInstance()
{
	// ���������ο� ���̷�Ż �޽� ���̴��� InputLayout, VS_Shader, PS_Shader ����
	shared_ptr<Shader> shader = ResourceManager::GetInstance()->CreateShaderResource("UIShader", eShaderType::UI_SHADER);
	shader->PipeLineSetting();

	m_pGraphics->GetDeviceContext()->UpdateSubresource(m_pTransformVP_Buffer.Get(), 0, nullptr, &m_TransformVP, 0, 0);
	for (const auto& uiInstance : m_UIInstanceList)
	{
		m_TransformW.mWorld = uiInstance->GetWorldMatrix()->Transpose();
		m_pDeviceContext->UpdateSubresource(m_pTransformW_Buffer.Get(), 0, nullptr, &m_TransformW, 0, 0);

		// UI �ִϸ��̼� ��� ���� ������Ʈ
		CB_UIAnimationKeyframe CB_animationKeyFrame;
		uiInstance->UIAnimationPlay(CB_animationKeyFrame);
		m_pDeviceContext->UpdateSubresource(m_pUIAnimation_Buffer.Get(), 0, nullptr, &CB_animationKeyFrame, 0, 0);

		// UI �ν��Ͻ� ����
		uiInstance->Render(m_pDeviceContext.Get());
	}
}

// �޽� �ν��Ͻ� ���͸��󺰷� ����
void D3DRenderManager::SortMeshInstance()
{
	//���������ο� �����ϴ� ���͸����� �ؽ��� ������ �ּ�ȭ �ϱ����� ���͸��� ���� �����Ѵ�.
	m_SkeletalMeshInstance.sort([](const SkeletalMeshInstance* lhs, const SkeletalMeshInstance* rhs)
		{
			return lhs->m_pMaterial < rhs->m_pMaterial;
		});
	//���������ο� �����ϴ� ���͸����� �ؽ��� ������ �ּ�ȭ �ϱ����� ���͸��� ���� �����Ѵ�.
	m_StaticMeshInstance.sort([](const StaticMeshInstance* lhs, const StaticMeshInstance* rhs)
		{
			return lhs->m_pMaterial < rhs->m_pMaterial;
		});

	//���������ο� �����ϴ� ���͸����� �ؽ��� ������ �ּ�ȭ �ϱ����� ���͸��� ���� �����Ѵ�.
	m_SkeletalMeshInstanceOpacity.sort([](const SkeletalMeshInstance* lhs, const SkeletalMeshInstance* rhs)
		{
			return lhs->m_pMaterial < rhs->m_pMaterial;
		});

	//���������ο� �����ϴ� ���͸����� �ؽ��� ������ �ּ�ȭ �ϱ����� ���͸��� ���� �����Ѵ�.
	m_StaticMeshInstanceOpacity.sort([](const StaticMeshInstance* lhs, const StaticMeshInstance* rhs)
		{
			return lhs->m_pMaterial < rhs->m_pMaterial;
		});
}

//void D3DRenderManager::CreateIBL()
//{
//	m_pEnvironmentModel = make_shared<EnvironmentModel>();
//	auto pComponent = m_pEnvironmentModel;
//
//	pComponent->ReadEnvironmentMeshFromFBX("../Resources/FBX/EnvironmentCube.fbx");
//	pComponent->ReadEnvironmentTextureFromDDS(L"../Resources/Textures/BakerSampleEnvHDR.dds");
//	pComponent->ReadIBLDiffuseTextureFromDDS(L"../Resources/Textures/BakerSampleDiffuseHDR.dds");
//	pComponent->ReadIBLSpecularTextureFromDDS(L"../Resources/Textures/BakerSampleSpecularHDR.dds");
//	pComponent->ReadIBLBRDFTextureFromDDS(L"../Resources/Textures/BakerSampleBRDF.dds");
//	pComponent->SetLocalScale(Vector3(100, 100, 100));
//	SetEnvironment();
//
//	m_IBL.UseIBL = true;
//}

void D3DRenderManager::SetEnvironment()
{
	//m_pDeviceContext->PSSetShaderResources(7, 1, m_pEnvironmentModel->m_EnvironmentTextureResource->m_pTextureSRV.GetAddressOf());
	//m_pDeviceContext->PSSetShaderResources(8, 1, m_pEnvironmentModel->m_IBLDiffuseTextureResource->m_pTextureSRV.GetAddressOf());
	//m_pDeviceContext->PSSetShaderResources(9, 1, m_pEnvironmentModel->m_IBLSpecularTextureResource->m_pTextureSRV.GetAddressOf());
	//m_pDeviceContext->PSSetShaderResources(10, 1, m_pEnvironmentModel->m_IBLBRDFTextureResource->m_pTextureSRV.GetAddressOf());
}

// ����� ��ο�
void D3DRenderManager::RenderDebugDraw()
{
	DebugDraw::g_BatchEffect->Apply(m_pGraphics->GetDeviceContext().Get());
	DebugDraw::g_BatchEffect->SetView(m_View);
	DebugDraw::g_BatchEffect->SetProjection(m_Projection);

	m_pGraphics->GetDeviceContext()->IASetInputLayout(DebugDraw::g_pBatchInputLayout.Get());

	DebugDraw::g_Batch->Begin();

	// Culling �ڽ��� �������� �ڽ���
	if (m_IsDrawDebugCulling)
	{
		DebugDraw::Draw(DebugDraw::g_Batch.get(), m_FrustumCamera, Colors::Yellow); // BoundingBox

		for (auto& model : m_SkeletalMeshModels)
		{
			DebugDraw::Draw(DebugDraw::g_Batch.get(), model->m_BoundingBox,
				model->m_IsCulled ? Colors::Red : Colors::Blue); // BoundingBox
		}
		for (auto& model : m_StaticMeshModels)
		{
			DebugDraw::Draw(DebugDraw::g_Batch.get(), model->m_BoundingBox,
				model->m_IsCulled ? Colors::Red : Colors::Blue); // BoundingBox
		}
	}

	DebugDraw::g_Batch->End();
}

void D3DRenderManager::RenderEnvironment()
{
	//if (m_pEnvironmentModel == nullptr)	return;
	//m_pGraphics->GetDeviceContext()->RSSetState(m_pPipeLine->GetRasterizerStateCCW().Get()); // ������ ������ ���� CCW

	//// ���������ο� ���̷�Ż �޽� ���̴��� InputLayout, VS_Shader, PS_Shader ����
	//shared_ptr<Shader> shader = ResourceManager::GetInstance()->CreateShaderResource("EnvironmentShader", eShaderType::ENVIRONMENT_SHADER);
	//shader->PipeLineSetting();

	//// ���� ���Ľ� ���´� �̰͸� ��������.
	//m_pGraphics->GetDeviceContext()->VSSetConstantBuffers(0, 1, m_pTransformW_Buffer.GetAddressOf()); //debugdraw���� ������Ѽ� �����Ѵ�.

	//m_TransformW.mWorld = m_pEnvironmentModel->m_worldMatrix->Transpose();
	//m_pGraphics->GetDeviceContext()->UpdateSubresource(m_pTransformW_Buffer.Get(), 0, nullptr, &m_TransformW, 0, 0);
	//m_pEnvironmentModel->m_MeshInstance.Render(m_pGraphics->GetDeviceContext().Get());

	//m_pGraphics->GetDeviceContext()->RSSetState(m_pPipeLine->GetRasterizerStateCW().Get()); // ������ ������ ���� CW
}

// ���� �޸� �б�
void D3DRenderManager::GetVideoMemoryInfo(std::string& out)
{
	DXGI_QUERY_VIDEO_MEMORY_INFO videoMemoryInfo;
	m_pDXGIAdapter->QueryVideoMemoryInfo(0, DXGI_MEMORY_SEGMENT_GROUP_LOCAL, &videoMemoryInfo);

	out = std::to_string(videoMemoryInfo.CurrentUsage / 1024 / 1024) + " MB" + " / " + std::to_string(videoMemoryInfo.Budget / 1024 / 1024) + " MB";
}

// �ý��� �޸� �б�
void D3DRenderManager::GetSystemMemoryInfo(std::string& out)
{
	HANDLE hProcess = GetCurrentProcess();
	PROCESS_MEMORY_COUNTERS_EX pmc;
	pmc.cb = sizeof(PROCESS_MEMORY_COUNTERS_EX);
	GetProcessMemoryInfo(hProcess, (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));
	out = std::to_string((pmc.PagefileUsage) / 1024 / 1024) + " MB";
}

void D3DRenderManager::DrawTextRequest(std::wstring_view text, FLOAT x, FLOAT y, D2D1::ColorF color, std::wstring_view fontFamily, FLOAT fontSize, DWRITE_FONT_WEIGHT fontWeight, DWRITE_FONT_STYLE fontStyle, DWRITE_FONT_STRETCH fontStretch)
{
	m_textRenderer->DrawTextRequest(text, x, y, color, fontFamily, fontSize, m_ClientWidth, m_ClientHeight, fontWeight, fontStyle, fontStretch);
}

void D3DRenderManager::ClearWorldResource()
{
	for (auto model : m_SkeletalMeshModels)
	{
		model.reset();
	}
	for (auto model : m_StaticMeshModels)
	{
		model.reset();
	}
	m_SkeletalMeshModels.clear();
	m_StaticMeshModels.clear();

	m_StaticMeshInstance.clear();
	m_SkeletalMeshInstance.clear();
	m_StaticMeshInstanceOpacity.clear();
	m_SkeletalMeshInstanceOpacity.clear();
	m_UIInstanceList.clear();
}

// ���͸��� ����
void D3DRenderManager::ApplyMaterial(Material* pMaterial)
{
	ID3D11ShaderResourceView* pNullSRV[7] = {
		pMaterial->m_pDiffuseRV != nullptr ? pMaterial->m_pDiffuseRV->m_pTextureSRV.Get() : nullptr,
		pMaterial->m_pNormalRV != nullptr ? pMaterial->m_pNormalRV->m_pTextureSRV.Get() : nullptr,
		pMaterial->m_pSpecularRV != nullptr ? pMaterial->m_pSpecularRV->m_pTextureSRV.Get() : nullptr,
		pMaterial->m_pEmissiveRV != nullptr ? pMaterial->m_pEmissiveRV->m_pTextureSRV.Get() : nullptr,
		pMaterial->m_pOpacityRV != nullptr ? pMaterial->m_pOpacityRV->m_pTextureSRV.Get() : nullptr,
		pMaterial->m_pMetalnessRV != nullptr ? pMaterial->m_pMetalnessRV->m_pTextureSRV.Get() : nullptr,
		pMaterial->m_pRoughnessRV != nullptr ? pMaterial->m_pRoughnessRV->m_pTextureSRV.Get() : nullptr ,
	};

	m_pGraphics->GetDeviceContext()->PSSetShaderResources(0, 7, pNullSRV); // �ѹ��� 7���� �ؽ�ó�� �����Ѵ�.

	CB_BoolBuffer CB_Bool;
	CB_Bool.UseGamma = m_isGamma;
	CB_Bool.UseDiffuseMap = pMaterial->m_pDiffuseRV != nullptr ? true : false;
	CB_Bool.UseNormalMap = pMaterial->m_pNormalRV != nullptr ? true : false;
	CB_Bool.UseSpecularMap = pMaterial->m_pSpecularRV != nullptr ? true : false;
	CB_Bool.UseEmissiveMap = pMaterial->m_pEmissiveRV != nullptr ? true : false;
	CB_Bool.UseOpacityMap = pMaterial->m_pOpacityRV != nullptr ? true : false;
	CB_Bool.UseMetalnessMap = pMaterial->m_pMetalnessRV != nullptr ? true : false;
	CB_Bool.UseRoughnessMap = pMaterial->m_pRoughnessRV != nullptr ? true : false;

	if (CB_Bool.UseOpacityMap && m_pPipeLine->GetAlphaBlendState() != nullptr)
		m_pGraphics->GetDeviceContext()->OMSetBlendState(m_pPipeLine->GetAlphaBlendState().Get(), nullptr, 0xffffffff); // ���ĺ��� ���¼��� , �ٸ��ɼ��� �⺻��
	else
		m_pGraphics->GetDeviceContext()->OMSetBlendState(nullptr, nullptr, 0xffffffff);	// �������� , �ٸ��ɼ��� �⺻��

	m_pGraphics->GetDeviceContext()->UpdateSubresource(m_pBoolBuffer.Get(), 0, nullptr, &CB_Bool, 0, 0);
}

void D3DRenderManager::SetEnvironmentModel(const std::shared_ptr<EnvironmentModel>& environmentModel)
{
	//m_pEnvironmentModel = environmentModel;
	m_IBL.UseIBL = true;

	SetEnvironment();
}

bool D3DRenderManager::Initialize(HWND hWnd)
{
	//m_pResourceManager = std::make_shared<ResourceManager>();
	m_hWnd = hWnd;

	if (!InitD3D())		return false;
	if (!InitImGUI())	return false;

	m_textRenderer = std::make_unique<TextRenderer>();
	m_textRenderer->Initailize(m_pGraphics->GetD2DRenderTarget().Get(), m_pGraphics->GetWriteFactory().Get());
	// ���� : UI ������ ���� �� ���� �Ŵ��� ���� ����
	/*
	std::shared_ptr<UIInstance> ui = std::make_shared<UIInstance>();
	Texture* texture = ResourceManager::GetInstance()->CreateTextureResource(L"../Resources/Textures/hair_diff.png").get();
	ui->Create(0.3, 0, 0.6, 0.6, texture);
	AddUIInstance(ui);
	std::shared_ptr<UIInstance> ui2 = std::make_shared<UIInstance>();
	ui2->Create(0.5, 0.5, 1, 1, texture);
	AddUIInstance(ui2);
	*/

	return true;
}

bool D3DRenderManager::InitD3D()
{
	// �����.
	HRESULT hr = 0;

	// Create DXGI factory
	HR_T(CreateDXGIFactory1(__uuidof(IDXGIFactory4), (void**)m_pDXGIFactory.GetAddressOf()));
	HR_T(m_pDXGIFactory->EnumAdapters(0, reinterpret_cast<IDXGIAdapter**>(m_pDXGIAdapter.GetAddressOf())));

	// Graphics ����
	{
		m_pGraphics = make_shared<Graphics>();
		GraphicsInfo info;
		info.m_ClearColor = Vector4(0.0f, 0.5f, 0.5f, 1.0f);
		info.m_ClientHeight = m_ClientHeight;
		info.m_ClientWidth = m_ClientWidth;
		info.m_hwnd = m_hWnd;

		m_pGraphics->Initalize(info);
		m_pDevice = m_pGraphics->GetDevice();
		m_pDeviceContext = m_pGraphics->GetDeviceContext();
	}
	// PipeLine ����
	{
		m_pPipeLine = make_shared<PipeLine>();
		PipeLineInfo info;
		info.m_pDevice = m_pDevice;
		info.m_pDeviceContext = m_pGraphics->GetDeviceContext();
		m_pPipeLine->Initalize(info);
	}
	// Debug ����
	{
		DebugDraw::Initialize(m_pDevice, m_pGraphics->GetDeviceContext());
	}

	// Render() ���� ���������ο� ���ε��� ��� ���� ����  //  Create the constant buffer
	CreateConstantBuffer();

	// ȯ��� ����.
	//CreateIBL();
	//	CreateLightIBL();

		// ������ �ʱ�ȭ
	InitScene();

	return true;
}

void D3DRenderManager::InitScene()
{
	m_Projection = XMMatrixPerspectiveFovLH(XM_PIDIV2, m_ClientWidth / (FLOAT)m_ClientHeight, 0.1f, 1000.0f);
	m_Orthographic = XMMatrixOrthographicLH((float)m_ClientWidth, (float)m_ClientHeight, m_Near, m_Far);
}

void D3DRenderManager::SetViewMatrix(Math::Matrix viewMatrix)
{
	m_View = viewMatrix;
}

void D3DRenderManager::SetCamMatrix(Math::Vector3 camMatrix)
{
	m_CamPosition = camMatrix;
}

bool D3DRenderManager::InitImGUI()
{
	// ImGui �ʱ�ȭ.
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init(m_hWnd);
	ImGui_ImplDX11_Init(m_pDevice.Get(), m_pGraphics->GetDeviceContext().Get());

	return true;
}

void D3DRenderManager::CreateConstantBuffer()
{
	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;

	bd.ByteWidth = sizeof(CB_IBL);
	HR_T(m_pDevice->CreateBuffer(&bd, nullptr, m_pIBL_Buffer.GetAddressOf()));

	bd.ByteWidth = sizeof(CB_BoolBuffer);
	HR_T(m_pDevice->CreateBuffer(&bd, nullptr, m_pBoolBuffer.GetAddressOf()));

	bd.ByteWidth = sizeof(CB_TransformW);
	HR_T(m_pDevice->CreateBuffer(&bd, nullptr, m_pTransformW_Buffer.GetAddressOf()));

	bd.ByteWidth = sizeof(CB_TransformVP);
	HR_T(m_pDevice->CreateBuffer(&bd, nullptr, m_pTransformVP_Buffer.GetAddressOf()));

	bd.ByteWidth = sizeof(CB_LightDirBuffer);
	HR_T(m_pDevice->CreateBuffer(&bd, nullptr, m_pLightBuffer.GetAddressOf()));

	bd.ByteWidth = sizeof(CB_MatrixPalette);
	HR_T(m_pDevice->CreateBuffer(&bd, nullptr, m_pMatPalette.GetAddressOf()));

	bd.ByteWidth = sizeof(CB_UIAnimationKeyframe);
	HR_T(m_pDevice->CreateBuffer(&bd, nullptr, m_pUIAnimation_Buffer.GetAddressOf()));
}

void D3DRenderManager::ImguiRender()
{
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Start the Dear ImGui frame
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	{
		ImGui::Begin("Editor");

		std::string str;
		GetVideoMemoryInfo(str);
		ImGui::Text("VideoMemory: %s", str.c_str());
		GetSystemMemoryInfo(str);
		ImGui::Text("SystemMemory: %s", str.c_str());

		ImGui::Dummy(ImVec2(0.0f, 10.0f));
		//float camPos[3] = { m_View._41, m_View._42, m_View._43 };
		//ImGui::SliderFloat3("Cam_Pos", &camPos, -1000.0f, 1000.0f);

		ImGui::Dummy(ImVec2(0.0f, 10.0f));
		ImGui::Checkbox("NormalMap", &m_isNormalMap);
		ImGui::Checkbox("SpecularMap", &m_isSpecularMap);
		ImGui::Checkbox("Gamma_Correction", &m_isGamma);
		ImGui::Checkbox("DiffuseMap", &m_isDiffuse);
		ImGui::Checkbox("EmissiveMap", &m_isEmissive);
		ImGui::Checkbox("OpacityMap", &m_isOpacity);
		ImGui::Checkbox("MetalnessMap", &m_isMetalness);
		ImGui::Checkbox("RoughnessMap", &m_isRoughness);

		ImGui::Dummy(ImVec2(0.0f, 10.0f));
		ImGui::SliderFloat("Far", &m_Far, 1.0f, 100000.0f);
		ImGui::SliderFloat("Near", &m_Near, 0.01f, 2.0f);
		ImGui::SliderFloat("Fov", &m_Fov, -20.0f, 180.0f);

		ImGui::Dummy(ImVec2(0.0f, 10.0f));
		ImGui::SliderFloat3("Light_RGB", m_vLightColor, 0.0f, 1.0f);
		ImGui::SliderFloat3("Light_XYZ", m_vLightDir, -1.0f, 1.0f);

		ImGui::End();
	}

	{
		ImGui::Begin("Debug");

		ImGui::Checkbox("FreezeCulling", &m_IsFreezeCulling);
		ImGui::Checkbox("DrawDebugCulling", &m_IsDrawDebugCulling);
		ImGui::Checkbox("DrawShadowFrustum", &m_IsDrawShadowFrustum);
		ImGui::Checkbox("DrawDebugCollision", &m_IsDrawDebugCollision);

		ImGui::Dummy(ImVec2(0.0f, 10.0f));
		std::string str;
		str = to_string(m_nDrawRenderCount);
		ImGui::Text("RenderDrawCount : %s", str.c_str());

		ImGui::End();
	}

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

