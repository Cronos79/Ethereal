#pragma once
#include "Core/EtherealIncludes.h"
#include "Assets/IAsset.h"
#include "Renderer/DX11/VertexBuffer.h"
#include "Renderer/DX11/IndexBuffer.h"
#include "Renderer/DX11/VertexBufferLayout.h"
#include <wrl/client.h>
#include <d3d11.h>
#include <memory>
#include <vector>
#include "Renderer/DX11/ConstantBufferTypes.h"
#include "Renderer/DX11/ConstantBuffer.h"

namespace Ethereal
{
	class ETHEREAL_API Mesh : public IAsset
	{
	public:
		Mesh() = default;
		~Mesh() = default;

		bool Initialize(const std::vector<uint8_t>& vertexData, const VertexBufferLayout& layout, const std::vector<uint32_t>& indices);
		bool Initialize(std::vector<uint8_t>&& vertexData, VertexBufferLayout&& layout, std::vector<uint32_t>&& indices);
		void UploadToGPU();
		void UpdateBuffer(const DirectX::XMMATRIX& worldMatrix, const DirectX::XMMATRIX& viewMatrix, const DirectX::XMMATRIX& projMatrix);

		ID3D11Buffer* GetVertexBuffer() const
		{
			return m_VertexBuffer.Get();
		}
		const UINT* GetStridePtr() const
		{
			return m_VertexBuffer.StridePtr();
		}
		ID3D11Buffer* GetIndexBuffer() const
		{
			return m_IndexBuffer.Get();
		}
		UINT GetIndexCount() const
		{
			return m_IndexBuffer.IndexCount();
		}

		void SetName(const std::string& name)
		{
			m_Name = name;
		}
		const std::string& GetName() const
		{
			return m_Name;
		}

		void SetMaterialIndex(uint32_t index)
		{
			m_MaterialIndex = index;
		}
		uint32_t GetMaterialIndex() const
		{
			return m_MaterialIndex;
		}

		std::vector<uint32_t>& GetIndices()
		{
			return m_Indices;
		}
		const VertexBufferLayout& GetLayout() const
		{
			return m_VertexLayout;
		}
		const std::vector<uint8_t>& GetVertexData() const
		{
			return m_VertexData;
		}

		ConstantBuffer<CB_VS_PerObject>& GetPerObjectCB()
		{
			return m_PerObjectCB;
		}

	private:
		bool CreateConstantBuffer();

	private:
		std::string m_Name;
		uint32_t m_MaterialIndex = 0;

		std::vector<uint8_t> m_VertexData;
		std::vector<uint32_t> m_Indices;
		VertexBufferLayout m_VertexLayout;

		VertexBuffer m_VertexBuffer;
		IndexBuffer m_IndexBuffer;
		ConstantBuffer<CB_VS_PerObject> m_PerObjectCB;
	};
}