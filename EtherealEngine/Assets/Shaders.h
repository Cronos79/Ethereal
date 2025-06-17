#pragma once
#include "Core/EtherealIncludes.h"
#include "Core/IAsset.h"
#include <wrl/client.h>
#include <d3d11.h>


namespace Ethereal
{
	// Enum for shader types
	enum class ShaderType
	{
		VERTEX_SHADER,
		PIXEL_SHADER,
		GEOMETRY_SHADER,
		HULL_SHADER,
		DOMAIN_SHADER,
		COMPUTE_SHADER
	};

	class ETHEREAL_API Shaders : public IAsset
	{
	public:
		virtual bool Load(const std::string& path) override;
		bool LoadShader(const std::string& path, ShaderType type);

		ID3D11VertexShader* GetVertexShader() const;
		ID3D11PixelShader* GetPixelShader() const;
		ID3D10Blob* GetShaderBuffer() const;

	private:
		Microsoft::WRL::ComPtr<ID3D11VertexShader> m_VertexShader;
		Microsoft::WRL::ComPtr<ID3D11PixelShader> M_PixelShader;
		Microsoft::WRL::ComPtr<ID3D10Blob> m_ShaderBuffer;
	};
}