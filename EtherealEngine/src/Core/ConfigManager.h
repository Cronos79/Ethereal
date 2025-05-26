#pragma once
#include <string>
#include <unordered_map>
#include <nlohmann/json.hpp>


namespace EtherealEngine
{
	class ConfigManager
	{
	public:
		bool Load(const std::string& path);

		int GetInt(const std::string& key, int defaultVal = 0) const;
		float GetFloat(const std::string& key, float defaultVal = 0.0f) const;
		std::string GetString(const std::string& key, const std::string& defaultVal = "") const;
		bool GetBool(const std::string& key, bool defaultVal = false) const;

		// Clear the current configuration data
		void Clear()
		{
			data.clear();
		}

		bool Save(const std::string& path) const;

		void SetInt(const std::string& key, int value);
		void SetFloat(const std::string& key, float value);
		void SetString(const std::string& key, const std::string& value);
		void SetBool(const std::string& key, bool value);

	private:
		nlohmann::json* GetJsonByKeyWritable(const std::string& key);

	private:
		nlohmann::json data;

		const nlohmann::json* GetJsonByKey(const std::string& key) const;
	};
}