#pragma once

#include <string>
#include <DirectXMath.h>

namespace EtherealEngine
{
	class Material
	{
	public:
		Material() = default;
		~Material() = default;

		// Colors
		void SetDiffuseColor(const DirectX::XMFLOAT3& color)
		{
			m_DiffuseColor = color;
		}
		void SetSpecularColor(const DirectX::XMFLOAT3& color)
		{
			m_SpecularColor = color;
		}
		void SetEmissiveColor(const DirectX::XMFLOAT3& color)
		{
			m_EmissiveColor = color;
		}
		void SetShininess(float shininess)
		{
			m_Shininess = shininess;
		}

		// Texture paths
		void SetDiffuseTexture(const std::string& path)
		{
			m_DiffuseTexture = path;
		}
		void SetSpecularTexture(const std::string& path)
		{
			m_SpecularTexture = path;
		}
		void SetNormalTexture(const std::string& path)
		{
			m_NormalTexture = path;
		}

		const DirectX::XMFLOAT3& GetDiffuseColor() const
		{
			return m_DiffuseColor;
		}
		const DirectX::XMFLOAT3& GetSpecularColor() const
		{
			return m_SpecularColor;
		}
		const DirectX::XMFLOAT3& GetEmissiveColor() const
		{
			return m_EmissiveColor;
		}
		float GetShininess() const
		{
			return m_Shininess;
		}

		const std::string& GetDiffuseTexture() const
		{
			return m_DiffuseTexture;
		}
		const std::string& GetSpecularTexture() const
		{
			return m_SpecularTexture;
		}
		const std::string& GetNormalTexture() const
		{
			return m_NormalTexture;
		}

		void SetVertexShader(const std::string& name)
		{
			m_VertexShaderName = name;
		}
		void SetPixelShader(const std::string& name)
		{
			m_PixelShaderName = name;
		}
		const std::string& GetVertexShader() const
		{
			return m_VertexShaderName;
		}
		const std::string& GetPixelShader() const
		{
			return m_PixelShaderName;
		}

	private:
		// Base colors
		DirectX::XMFLOAT3 m_DiffuseColor{ 1.0f, 1.0f, 1.0f };
		DirectX::XMFLOAT3 m_SpecularColor{ 1.0f, 1.0f, 1.0f };
		DirectX::XMFLOAT3 m_EmissiveColor{ 0.0f, 0.0f, 0.0f };
		float m_Shininess = 32.0f;

		// Texture file paths
		std::string m_DiffuseTexture;
		std::string m_SpecularTexture;
		std::string m_NormalTexture;

		std::string m_VertexShaderName;
		std::string m_PixelShaderName;
	};
}