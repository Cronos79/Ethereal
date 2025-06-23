#pragma once
#include "EtherealIncludes.h"
#include <unordered_map>
#include <string>
#include <memory>
#include "Assets/IAsset.h"
#include "Assets/Shaders.h"

namespace Ethereal
{
	class ETHEREAL_API AssetManager
	{
	public:
		void Initialize();
		void Shutdown();

		bool LoadRegistry(const std::string& path);
		bool LoadGameConfig(const std::string& name);
		bool LoadShader(const std::string& name, ShaderType shaderType);
		bool LoadModel(const std::string& name);

		template<typename T>
		std::shared_ptr<T> Get(const std::string& name)
		{
			auto it = m_Assets.find(name);
			if (it != m_Assets.end())
				return std::dynamic_pointer_cast<T>(it->second);
			return nullptr;
		}

	private:
		std::unordered_map<std::string, std::shared_ptr<IAsset>> m_Assets;
		std::unordered_map<std::string, std::string> m_Registry; // name -> path
		bool m_Initialized = false;
	};
}
