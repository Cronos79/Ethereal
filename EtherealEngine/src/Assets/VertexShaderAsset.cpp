#include "pch.h"
#include "VertexShaderAsset.h"
#include <d3dcompiler.h>
#include <fstream>
#include <vector>
#include <wrl/client.h>
#include "Core/EELoggerMacros.h"

using Microsoft::WRL::ComPtr;

namespace EtherealEngine
{
	bool VertexShaderAsset::LoadFromFile(const std::string& path)
	{
		std::ifstream file(path, std::ios::binary | std::ios::ate);
		if (!file.is_open())
		{
			LOG_ERROR("Failed to open shader bytecode file: {}", path);
			return false;
		}
		std::streamsize size = file.tellg();
		file.seekg(0, std::ios::beg);
		m_Bytecode.resize(size);
		if (!file.read(reinterpret_cast<char*>(m_Bytecode.data()), size))
		{
			LOG_ERROR("Failed to read shader bytecode file: {}", path);
			return false;
		}
		return true;
	}
}