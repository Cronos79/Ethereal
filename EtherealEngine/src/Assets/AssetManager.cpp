#include "pch.h"
#include "Assets/AssetManager.h"
#include "Core/EELoggerMacros.h"
#include "Assets/ModelAsset.h"
#include "Assets/VertexShaderAsset.h"

#include <nlohmann/json.hpp>
#include <fstream>
#include "PixelShaderAsset.h"

namespace EtherealEngine
{

	AssetManager::AssetManager()
	{

	}

	AssetManager::~AssetManager()
	{

	}

	void AssetManager::Initialize()
	{
		// Load the asset registry JSON file
		std::string registryPath = EEContext::Get().GetAssetPath("asset_registry.json");
		if (!LoadRegistry(registryPath))
		{
			LOG_WARN("Asset registry could not be loaded from '{}'", registryPath);
		}
		
		LOG_INFO("AssetManager initialized.");
	}

	void AssetManager::Shutdown()
	{
		LOG_INFO("AssetManager shutdown.");
		m_Assets.clear();
		m_Registry.clear();
	}

	bool AssetManager::LoadRegistry(const std::string& path)
	{
		std::ifstream file(path);
		if (!file.is_open())
		{
			LOG_ERROR("Failed to open asset registry: {}", path);
			return false;
		}

		nlohmann::json json;
		file >> json;

		for (auto& [key, value] : json.items())
		{
			m_Registry[key] = value;
			LOG_INFO("Registered asset: '{}' -> '{}'", key, value.get<std::string>());
		}
		return true;
	}

	bool AssetManager::LoadModel(const std::string& name)
	{
		auto it = m_Registry.find(name);
		if (it == m_Registry.end())
		{
			LOG_WARN("Model '{}' not found in registry.", name);
			return false;
		}

		std::string path = EEContext::Get().GetAssetPath(it->second);
		auto model = std::make_shared<ModelAsset>();
		if (!model->LoadFromFile(path))
		{
			LOG_ERROR("Failed to load model: {}", path);
			return false;
		}
		m_Assets[name] = model;
		LOG_INFO("Model '{}' loaded from '{}'", name, path);
		return true;
	}

	bool AssetManager::LoadVertexShader(const std::string& name)
	{
		auto it = m_Registry.find(name);
		if (it == m_Registry.end())
		{
			LOG_WARN("Vertex shader '{}' not found in registry.", name);
			return false;
		}

		std::string path = EEContext::Get().GetAssetPath(it->second);
		auto shader = std::make_shared<VertexShaderAsset>();
		if (!shader->LoadFromFile(path))
		{
			LOG_ERROR("Failed to load vertex shader: {}", path);
			return false;
		}
		m_Assets[name] = shader;
		LOG_INFO("Vertex shader '{}' loaded from '{}'", name, path);
		return true;
	}

	bool AssetManager::LoadPixelShader(const std::string& name)
	{
		auto it = m_Registry.find(name);
		if (it == m_Registry.end())
		{
			LOG_WARN("Pixel shader '{}' not found in registry.", name);
			return false;
		}
		std::string path = EEContext::Get().GetAssetPath(it->second);
		auto shader = std::make_shared<PixelShaderAsset>();
		if (!shader->LoadFromFile(path))
		{
			LOG_ERROR("Failed to load pixel shader: {}", path);
			return false;
		}
		m_Assets[name] = shader;
		LOG_INFO("Pixel shader '{}' loaded from '{}'", name, path);
		return true;
	}

} // namespace EtherealEngine