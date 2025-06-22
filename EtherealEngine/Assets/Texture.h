#pragma once
#include <d3d11.h>
#include <wrl/client.h>
#include <string>
#include "Core/EtherealIncludes.h"
#include "Assets/IAsset.h"

namespace Ethereal
{
	class ETHEREAL_API Texture : public IAsset
	{
	public:
		Texture() = default;
		~Texture() = default;

		bool LoadFromFile(const std::string& path);

		ID3D11ShaderResourceView* GetTextureView() const
		{
			return m_TextureView.Get();
		}

	private:
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_TextureView;
		std::string m_SourcePath;
	};
}