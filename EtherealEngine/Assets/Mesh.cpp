#include "Assets/Mesh.h"
#include "Core/Logger.h"
#include "Core/EEContext.h"

namespace Ethereal
{
	bool Mesh::Initialize(const std::vector<uint8_t>& vertexData, const VertexBufferLayout& layout, const std::vector<uint32_t>& indices)
	{
		m_VertexData = vertexData;
		m_VertexLayout = layout;
		m_Indices = indices;
		CreateConstantBuffer();
		UploadToGPU();
		return true;
	}

	bool Mesh::Initialize(std::vector<uint8_t>&& vertexData, VertexBufferLayout&& layout, std::vector<uint32_t>&& indices)
	{
		m_VertexData = std::move(vertexData);
		m_VertexLayout = std::move(layout);
		m_Indices = std::move(indices);
		if (!CreateConstantBuffer())
		{
			LOG_ERROR("Failed to create constant buffer for mesh");
			return false;
		}
		UploadToGPU();
		return true;
	}

	void Mesh::UploadToGPU()
	{
		ID3D11Device* device = static_cast<ID3D11Device*>(EEContext::Get().GetDevice());

		if (!m_VertexData.empty() && !m_VertexBuffer.Get())
		{
			m_VertexBuffer.Initialize(device, m_VertexData.data(), static_cast<UINT>(m_VertexData.size()), m_VertexLayout.GetStride());
		}

		if (!m_Indices.empty() && !m_IndexBuffer.Get())
		{
			m_IndexBuffer.Initialize(device, reinterpret_cast<DWORD*>(m_Indices.data()), (UINT)m_Indices.size());
		}
	}

	void Mesh::UpdateBuffer(const DirectX::XMMATRIX& worldMatrix, const DirectX::XMMATRIX& viewMatrix, const DirectX::XMMATRIX& projMatrix)
	{
		DirectX::XMMATRIX wvp = XMMatrixTranspose(worldMatrix * viewMatrix * projMatrix);
		m_PerObjectCB.data.worldViewProj = wvp;
		m_PerObjectCB.ApplyChanges();
	}

	bool Mesh::CreateConstantBuffer()
	{
		ID3D11Device* device = static_cast<ID3D11Device*>(EEContext::Get().GetDevice());
		ID3D11DeviceContext* context = static_cast<ID3D11DeviceContext*>(EEContext::Get().GetContext());
		HRESULT hr = m_PerObjectCB.Initialize(device, context);
		if (FAILED(hr))
		{
			LOG_ERROR("Failed to initialize model constant buffer: {}", hr);
			return false;
		}
		return true;
	}

}