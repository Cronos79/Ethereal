#include "pch.h"
#include "Assets/AssetManager.h"
#include "Core/EELoggerMacros.h"
#include "Assets/ModelAsset.h"
#include <nlohmann/json.hpp>

#include <fstream>

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

} // namespace EtherealEngine