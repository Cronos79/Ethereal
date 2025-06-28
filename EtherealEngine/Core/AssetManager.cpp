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
#include "Assets/LightObject.h"
#include "Assets/Texture.h"
#include "Assets/Scene.h"
#include "EEContext.h"
#include "UI/NoesisUI.h"


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
			//LOG_INFO("Registered asset: '{}' -> '{}'", key, value.get<std::string>());
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
		//LOG_INFO("Loaded shader '{}' of type '{}'", name, ShaderTypeToString(shaderType));
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
					mat.normalTexture = matJson.value("normalTexture", "");
					mat.specularTexture = matJson.value("specularTexture", "");
					mat.metallicTexture = matJson.value("metallicTexture", "");
					mat.roughnessTexture = matJson.value("roughnessTexture", "");
					mat.aoTexture = matJson.value("aoTexture", "");

					if (matJson.contains("inputLayout"))
						mat.inputLayout = matJson["inputLayout"];

					model->SetMaterialOverride(id, mat);
				}
			}

			if (!model->LoadFromFile(meshFullPath.string()))
			{
				LOG_ERROR("Failed to load model mesh: {}", meshFullPath.string());
				return false;
			}

			m_Assets[name] = model;
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

		gameObject->SetTypeName(type);
		gameObject->SetName(j.value("name", name));

		// Handle LightObject custom data
		if (type == "LightObject")
		{
			auto light = std::dynamic_pointer_cast<LightObject>(gameObject);
			if (light)
			{
				if (j.contains("lightDirection"))
				{
					auto& v = j["lightDirection"];
					light->SetLightDirection({ v[0], v[1], v[2] });
				}
				if (j.contains("lightColor"))
				{
					auto& c = j["lightColor"];
					light->SetLightColor({ c[0], c[1], c[2] });
				}
				if (j.contains("ambientStrength"))
				{
					light->SetAmbientStrength(j["ambientStrength"]);
				}
			}
		}

		// Optional model
		if (j.contains("model"))
		{
			std::string modelName = j["model"];
			if (LoadModel(modelName))
			{
				auto model = Get<Model>(modelName);
				gameObject->SetModel(model);
			}
			else
			{
				LOG_WARN("Model '{}' for GameObject '{}' could not be loaded", modelName, name);
			}
		}

		// Common transform
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
		//LOG_INFO("Loaded GameObject '{}'", name);
		return true;
	}

	bool AssetManager::LoadTexture(const std::string& name)
	{
		// Already loaded?
		if (m_Assets.find(name) != m_Assets.end())
			return true;

		// Look up path in registry
		auto it = m_Registry.find(name);
		if (it == m_Registry.end())
		{
			LOG_ERROR("Texture '{}' not found in registry.", name);
			return false;
		}

		// Construct full path
		std::filesystem::path fullPath = GetAssetsDirectory();
		fullPath /= it->second;

		// Load texture
		auto texture = std::make_shared<Texture>();
		if (!texture->LoadFromFile(fullPath.string()))
		{
			LOG_ERROR("Failed to load texture: {}", fullPath.string());
			return false;
		}

		m_Assets[name] = texture;
		LOG_INFO("Loaded texture '{}'", name);
		return true;
	}

	bool AssetManager::LoadScene(const std::string& name)
	{
		if (m_Assets.find(name) != m_Assets.end())
			return true;

		auto it = m_Registry.find(name);
		if (it == m_Registry.end())
		{
			LOG_ERROR("Scene '{}' not found in registry.", name);
			return false;
		}

		std::filesystem::path fullPath = GetAssetsDirectory() / it->second;
		std::ifstream file(fullPath);
		if (!file.is_open())
		{
			LOG_ERROR("Failed to open Scene file: {}", fullPath.string());
			return false;
		}

		nlohmann::json sceneJson;
		file >> sceneJson;

		auto scene = std::make_shared<Scene>();
		scene->SetName(sceneJson.value("name", name));
		scene->SetUIPath(sceneJson.value("UIPath", ""));		

		if (sceneJson.contains("gameObjects"))
		{
			for (const auto& objJson : sceneJson["gameObjects"])
			{
				std::string refName = objJson.value("ref", "");
				if (refName.empty())
				{
					LOG_WARN("Scene object missing 'ref' field.");
					continue;
				}

				// Load the referenced GameObject asset if not already
				if (!LoadGameObject(refName))
				{
					LOG_WARN("Referenced GameObject '{}' could not be loaded.", refName);
					continue;
				}

				auto original = Get<GameObject>(refName);
				if (!original)
				{
					LOG_WARN("Referenced GameObject '{}' not found after loading.", refName);
					continue;
				}

				auto clone = original->Clone();

				// Override transform if provided
				if (objJson.contains("transform"))
				{
					const auto& t = objJson["transform"];
					DirectX::XMFLOAT3 position = { 0, 0, 0 };
					DirectX::XMFLOAT3 rotation = { 0, 0, 0 };
					DirectX::XMFLOAT3 scale = { 1, 1, 1 };

					if (t.contains("position") && t["position"].is_array() && t["position"].size() == 3)
					{
						position = {
							static_cast<float>(t["position"].at(0)),
							static_cast<float>(t["position"].at(1)),
							static_cast<float>(t["position"].at(2))
						};
					}
					if (t.contains("rotation") && t["rotation"].is_array() && t["rotation"].size() == 3)
					{
						rotation = {
							static_cast<float>(t["rotation"].at(0)),
							static_cast<float>(t["rotation"].at(1)),
							static_cast<float>(t["rotation"].at(2))
						};
					}
					if (t.contains("scale") && t["scale"].is_array() && t["scale"].size() == 3)
					{
						scale = {
							static_cast<float>(t["scale"].at(0)),
							static_cast<float>(t["scale"].at(1)),
							static_cast<float>(t["scale"].at(2))
						};
					}
					clone->SetTransform(position, rotation, scale);
				}

				scene->AddGameObject(clone);
			}
		}
		scene->OnActivate();
		m_Assets[name] = scene;
		LOG_INFO("Loaded scene '{}'", name);
		return true;
	}

	void AssetManager::RegisterGameObjectFactory(GameObjectFactoryFunc factory)
	{
		m_GameObjectFactory = factory;
	}

}