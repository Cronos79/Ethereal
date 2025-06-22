#include "Assets/Texture.h"
#include "Core/EEContext.h"
#include "Core/Logger.h"
#include <WICTextureLoader.h>

namespace Ethereal
{
	bool Texture::LoadFromFile(const std::string& path)
	{
		ID3D11Device* device = static_cast<ID3D11Device*>(EEContext::Get().GetDevice());
		m_SourcePath = path;
		Microsoft::WRL::ComPtr<ID3D11Resource> resource;
		std::wstring widePath(path.begin(), path.end());

		HRESULT hr = DirectX::CreateWICTextureFromFile(
			device,
			widePath.c_str(),
			&resource,
			&m_TextureView
		);

		return SUCCEEDED(hr);
	}
}