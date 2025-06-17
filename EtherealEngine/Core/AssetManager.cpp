#include "Core/AssetManager.h"
#include "Logger.h"
#include <memory>
#include <nlohmann/json.hpp>
#include <fstream>
#include "Core/EngineUtils.h"
#include "Assets/GameConfig.h"
#include "Assets/Shaders.h"

namespace Ethereal
{

	void AssetManager::Initialize()
	{
		m_Initialized = true;
	}

	void AssetManager::Shutdown()
	{
		m_Assets.clear();
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

	bool AssetManager::LoadGameConfig(const std::string& name)
	{
		auto it = m_Registry.find(name);
		if (it == m_Registry.end())
		{
			LOG_ERROR("GameConfig '{}' not found in registry.", name);
			return false;
		}

		std::filesystem::path fullPath = GetAssetsDirectory();
		fullPath /= it->second;

		std::ifstream file(fullPath);
		if (!file.is_open())
		{
			LOG_ERROR("Failed to open GameConfig file: {}", fullPath.string());
			return false;
		}

		auto gameConfig = std::make_shared<GameConfig>();
		gameConfig->Load(fullPath.string());

		m_Assets[name] = gameConfig;
		return true;
	}

	bool AssetManager::LoadShader(const std::string& name, ShaderType shaderType)
	{
		auto it = m_Registry.find(name);
		if (it == m_Registry.end())
		{
			LOG_ERROR("GameConfig '{}' not found in registry.", name);
			return false;
		}

		std::filesystem::path fullPath = GetAssetsDirectory();
		fullPath /= it->second;

		auto shader = std::make_shared<Shaders>();
		shader->LoadShader(fullPath.string(), shaderType);

		m_Assets[name] = shader;
		LOG_INFO("Loaded shader '{}' of type '{}'", name, ShaderTypeToString(shaderType));
		return true;
	}

}