#include "pch.h"
#include "PixelShaderAsset.h"
#include <fstream>
#include "Core/EELoggerMacros.h"

namespace EtherealEngine
{
	bool PixelShaderAsset::LoadFromFile(const std::string& path)
	{
		std::ifstream file(path, std::ios::binary | std::ios::ate);
		if (!file.is_open())
		{
			LOG_ERROR("Failed to open pixel shader bytecode file: {}", path);
			return false;
		}
		std::streamsize size = file.tellg();
		file.seekg(0, std::ios::beg);
		m_Bytecode.resize(size);
		if (!file.read(reinterpret_cast<char*>(m_Bytecode.data()), size))
		{
			LOG_ERROR("Failed to read pixel shader bytecode file: {}", path);
			return false;
		}
		return true;
	}
}