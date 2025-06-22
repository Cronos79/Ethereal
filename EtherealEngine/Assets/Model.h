#pragma once
#include "IAsset.h"
#include "Core/EtherealIncludes.h"
#include <vector>
#include "Assets/Mesh.h"
#include "Assets/Material.h"
#include "Renderer/DX11/ConstantBuffer.h"
#include "Renderer/DX11/ConstantBufferTypes.h"

using namespace DirectX;

namespace Ethereal
{
	class ETHEREAL_API Model : public IAsset
	{
	public:		
		Model();
		Model(std::shared_ptr<Material> material, std::shared_ptr<Mesh> mesh);
		virtual ~Model();

		bool Initialize();
		ID3D11InputLayout* GetInputLayout()
		{
			return m_InputLayout.Get();
		}
		ConstantBuffer<CB_VS_vertexshader>& GetConstantBuffer()
		{
			return m_ConstantBuffer;
		}

		void SetMaterial(std::shared_ptr<Material> material)
		{
			m_Material = std::move(material);
		}
		Material* GetMaterial() const
		{
			return m_Material.get();
		}
		void SetMesh(std::shared_ptr<Mesh> mesh)
		{
			m_Mesh = std::move(mesh);
		}
		Mesh* GetMesh() const
		{
			return m_Mesh.get();
		}

	private:
		std::shared_ptr<Material> m_Material;
		std::shared_ptr<Mesh> m_Mesh;
		Microsoft::WRL::ComPtr<ID3D11InputLayout> m_InputLayout;
		ConstantBuffer<CB_VS_vertexshader> m_ConstantBuffer;
	};
}