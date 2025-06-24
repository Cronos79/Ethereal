#include "Core/AssetManager.h"
#include "Logger.h"
#include <memory>
#include <nlohmann/json.hpp>
#include <fstream>
#include "Core/EngineUtils.h"
#include "Assets/GameConfig.h"
#include "Assets/Shaders.h"
#include "Assets/Model.h"
#include "Assets/GameObject.h"


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
		// Check if asset is already loaded
		if (m_Assets.find(name) != m_Assets.end())
		{
			return true;
		}

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

	bool AssetManager::LoadModel(const std::string& name)
	{
		// Already loaded?
		if (m_Assets.find(name) != m_Assets.end())
			return true;

		auto it = m_Registry.find(name);
		if (it == m_Registry.end())
		{
			LOG_ERROR("Model '{}' not found in registry.", name);
			return false;
		}

		std::filesystem::path fullPath = GetAssetsDirectory();
		fullPath /= it->second;

		// Check file extension
		if (fullPath.extension() == ".json")
		{
			std::ifstream file(fullPath);
			if (!file.is_open())
			{
				LOG_ERROR("Failed to open model JSON: {}", fullPath.string());
				return false;
			}

			nlohmann::json j;
			file >> j;

			std::string meshPath = j.value("mesh", "");
			if (meshPath.empty())
			{
				LOG_ERROR("No 'mesh' field found in model JSON '{}'.", fullPath.string());
				return false;
			}

			std::filesystem::path meshFullPath = GetAssetsDirectory() / meshPath;

			auto model = std::make_shared<Model>();
			
			if (j.contains("materials") && j["materials"].is_array())
			{
				for (const auto& matJson : j["materials"])
				{
					int32_t id = matJson.value("ID", -1);
					if (id < 0) continue;

					MaterialOverride mat;
					mat.vertexShader = matJson.value("vertexShader", "VertexShader");
					mat.pixelShader = matJson.value("pixelShader", "PixelShader");
					mat.diffuseTexture = matJson.value("diffuseTexture", "Textures/UV.png");

					model->SetMaterialOverride(id, mat);
				}
			}

			if (!model->LoadFromFile(meshFullPath.string()))
			{
				LOG_ERROR("Failed to load model mesh: {}", meshFullPath.string());
				return false;
			}

			m_Assets[name] = model;
			LOG_INFO("Loaded model '{}' from JSON", name);
			return true;
		}
		else
		{
			// Legacy FBX path
			auto model = std::make_shared<Model>();
			if (!model->LoadFromFile(fullPath.string()))
			{
				LOG_ERROR("Failed to load model: {}", fullPath.string());
				return false;
			}
			m_Assets[name] = model;
			LOG_INFO("Loaded model '{}'", name);
			return true;
		}
	}

	bool AssetManager::LoadGameObject(const std::string& name)
	{
		if (m_Assets.find(name) != m_Assets.end())
			return true;

		auto it = m_Registry.find(name);
		if (it == m_Registry.end())
		{
			LOG_ERROR("GameObject '{}' not found in registry.", name);
			return false;
		}

		std::filesystem::path fullPath = GetAssetsDirectory() / it->second;

		std::ifstream file(fullPath);
		if (!file.is_open())
		{
			LOG_ERROR("Failed to open GameObject file: {}", fullPath.string());
			return false;
		}

		nlohmann::json j;
		file >> j;

		std::string type = j.value("type", "GameObject");
		auto gameObject = GameObject::CreateByType(type);
		if (!gameObject)
		{
			LOG_WARN("GameObject type '{}' not found, using base GameObject.", type);
			gameObject = std::make_shared<GameObject>();
		}
		gameObject->SetTypeName(type);   //m_TypeName = type;
		gameObject->SetName(j.value("name", name));	
		std::string modelName = j.value("model", "");
		if (!LoadModel(modelName))
		{
			LOG_WARN("Model '{}' for GameObject '{}' could not be loaded", modelName, name);
			return false;
		}
		else
		{
			auto model = Get<Model>(modelName);
			gameObject->SetModel(model);
		}

		if (j.contains("transform"))
		{
			const auto& t = j["transform"];
			gameObject->SetTransform(
				{ t["position"][0], t["position"][1], t["position"][2] },
				{ t["rotation"][0], t["rotation"][1], t["rotation"][2] },
				{ t["scale"][0], t["scale"][1], t["scale"][2] }
			);
		}

		m_Assets[name] = gameObject;
		LOG_INFO("Loaded GameObject '{}'", name);
		return true;
	}

	void AssetManager::RegisterGameObjectFactory(GameObjectFactoryFunc factory)
	{
		m_GameObjectFactory = factory;
	}

}