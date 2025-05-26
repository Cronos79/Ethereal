#include "pch.h"
#include "ConfigManager.h"
#include <fstream>
#include "EELoggerMacros.h"

namespace EtherealEngine
{
	bool ConfigManager::Load(const std::string& path)
	{
		std::ifstream file(path);
		if (!file.is_open())
		{
			LOG_ERROR("Failed to open config: " + path);
			return false;
		}

		try
		{
			file >> data;
			LOG_INFO("Config loaded successfully: " + path);
		}
		catch (const std::exception& e)
		{
			LOG_ERROR("Failed to parse config JSON: " + std::string(e.what()));
			return false;
		}

		return true;
	}

	const nlohmann::json* ConfigManager::GetJsonByKey(const std::string& key) const
	{
		const nlohmann::json* current = &data;
		size_t pos = 0, end;

		while ((end = key.find('.', pos)) != std::string::npos)
		{
			std::string part = key.substr(pos, end - pos);
			if (!current->contains(part)) return nullptr;
			current = &(*current)[part];
			pos = end + 1;
		}

		std::string lastPart = key.substr(pos);
		if (!current->contains(lastPart)) return nullptr;
		return &(*current)[lastPart];
	}

	int ConfigManager::GetInt(const std::string& key, int defaultVal) const
	{
		const auto* val = GetJsonByKey(key);
		return (val && val->is_number_integer()) ? val->get<int>() : defaultVal;
	}

	float ConfigManager::GetFloat(const std::string& key, float defaultVal) const
	{
		const auto* val = GetJsonByKey(key);
		return (val && val->is_number()) ? val->get<float>() : defaultVal;
	}

	std::string ConfigManager::GetString(const std::string& key, const std::string& defaultVal) const
	{
		const auto* val = GetJsonByKey(key);
		return (val && val->is_string()) ? val->get<std::string>() : defaultVal;
	}

	bool ConfigManager::GetBool(const std::string& key, bool defaultVal) const
	{
		const auto* val = GetJsonByKey(key);
		return (val && val->is_boolean()) ? val->get<bool>() : defaultVal;
	}

	bool ConfigManager::Save(const std::string& path) const
	{
		std::ofstream file(path);
		if (!file.is_open())
		{
			LOG_ERROR("Failed to open config for saving: " + path);
			return false;
		}

		file << data.dump(4); // Pretty print with 4-space indent
		LOG_INFO("Config saved successfully: " + path);
		return true;
	}

	nlohmann::json* ConfigManager::GetJsonByKeyWritable(const std::string& key)
	{
		nlohmann::json* current = &data;
		size_t pos = 0, end;

		while ((end = key.find('.', pos)) != std::string::npos)
		{
			std::string part = key.substr(pos, end - pos);
			current = &(*current)[part];  // Creates object if not found
			pos = end + 1;
		}

		std::string lastPart = key.substr(pos);
		current = &(*current)[lastPart];
		return current;
	}

	void ConfigManager::SetInt(const std::string& key, int value)
	{
		*GetJsonByKeyWritable(key) = value;
	}

	void ConfigManager::SetFloat(const std::string& key, float value)
	{
		*GetJsonByKeyWritable(key) = value;
	}

	void ConfigManager::SetString(const std::string& key, const std::string& value)
	{
		*GetJsonByKeyWritable(key) = value;
	}

	void ConfigManager::SetBool(const std::string& key, bool value)
	{
		*GetJsonByKeyWritable(key) = value;
	}
} // namespace EtherealEngine