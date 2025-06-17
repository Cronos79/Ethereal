#include "Assets/Shaders.h"
#include "Core/Logger.h"
#include <d3dcompiler.h>
#include "Core/EngineUtils.h"
#include "Core/EEContext.h"
#include <d3d11.h>

namespace Ethereal
{
	bool Shaders::Load(const std::string& path)
	{
		return true;
	}
	bool Shaders::LoadShader(const std::string& path, ShaderType type)
	{
		if (path.empty())
		{
			return false;
		}

		HRESULT hr = D3DReadFileToBlob(StringToWChar(path).c_str(), &m_ShaderBuffer);
		if (FAILED(hr))
		{
			LOG_ERROR("Failed to read shader file: {}", path);
			return false;
		}

		auto device = static_cast<ID3D11Device*>(EEContext::Get().GetDevice());

		if (type == ShaderType::VERTEX_SHADER)
		{
			hr = device->CreateVertexShader(m_ShaderBuffer->GetBufferPointer(), m_ShaderBuffer->GetBufferSize(), nullptr, &m_VertexShader);
			if (FAILED(hr))
			{
				LOG_ERROR("Failed to create vertex shader from file: {}", path);
				return false;
			}
		}
		else if (type == ShaderType::PIXEL_SHADER)
		{
			hr = device->CreatePixelShader(m_ShaderBuffer->GetBufferPointer(), m_ShaderBuffer->GetBufferSize(), nullptr, &M_PixelShader);
			if (FAILED(hr))
			{
				LOG_ERROR("Failed to create pixel shader from file: {}", path);
				return false;
			}
		}
		else
		{
			LOG_ERROR("Unsupported shader type for file: {}", path);
			return false;
		}
		return true;
	}
	ID3D11VertexShader* Shaders::GetVertexShader() const
	{
		return m_VertexShader.Get();
	}
	ID3D11PixelShader* Shaders::GetPixelShader() const
	{
		return M_PixelShader.Get();
	}
	ID3D10Blob* Shaders::GetShaderBuffer() const
	{
		return m_ShaderBuffer.Get();
	}
} // namespace Ethereal



