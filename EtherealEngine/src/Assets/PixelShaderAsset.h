#pragma once
#include "Assets/IAsset.h"
#include <d3d12.h>
#include <vector>
#include <string>

namespace EtherealEngine
{
	class PixelShaderAsset : public IAsset {
	public:
		PixelShaderAsset() = default;
		virtual ~PixelShaderAsset() override = default;

		bool LoadFromFile(const std::string& path);

		const std::vector<uint8_t>& GetBytecode() const
		{
			return m_Bytecode;
		}
		D3D12_SHADER_BYTECODE GetShaderBytecode() const
		{
			D3D12_SHADER_BYTECODE bc = {};
			bc.pShaderBytecode = m_Bytecode.data();
			bc.BytecodeLength = m_Bytecode.size();
			return bc;
		}
	private:
		std::vector<uint8_t> m_Bytecode;
	};
}