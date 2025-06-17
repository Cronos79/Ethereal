#include "Assets/Material.h"
#include "Core/EEContext.h"
#include "Core/Logger.h"
#include "Renderer/DX11/RendererDX11.h"

namespace Ethereal
{
	bool Material::Initialize()
	{
		// Load the shaders into the asset manager
		EEContext::Get().GetAssetManager().LoadShader("VertexShader", ShaderType::VERTEX_SHADER);
		EEContext::Get().GetAssetManager().LoadShader("PixelShader", ShaderType::PIXEL_SHADER);

		// Get the Vertex shader from the asset manager
		m_VertexShaderAsset = EEContext::Get().GetAssetManager().Get<Shaders>("VertexShader");
		m_VertexBlob = m_VertexShaderAsset->GetVertexShaderBuffer();

		// Get the Pixel shader from the asset manager
		m_PixelShaderAsset = EEContext::Get().GetAssetManager().Get<Shaders>("PixelShader");
		m_PixelBlob = m_PixelShaderAsset->GetPixelShaderBuffer();

		return true;
	}
}