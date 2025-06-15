#include "Assets/GameConfig.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include "Core/Logger.h"

namespace Ethereal
{
	GameConfig::GameConfig()
		: m_Title(""), m_Width(0), m_Height(0), m_FullScreen(false)
	{

	}

	GameConfig::~GameConfig()
	{
	}

	bool GameConfig::Load(const std::string& path)
	{
		std::ifstream file(path);
		if (!file.is_open())
		{
			LOG_ERROR("Failed to open GameConfig file: {}", path);
			return false;
		}

		nlohmann::json json;
		try
		{
			file >> json;
		}
		catch (const std::exception& e)
		{
			LOG_ERROR("Failed to parse GameConfig JSON: {}", e.what());
			return false;
		}

		// Read values with defaults if missing
		m_Title = json.value("Title", "");
		m_Width = json.value("Width", 0);
		m_Height = json.value("Height", 0);
		m_FullScreen = json.value("FullScreen", false);

		LOG_INFO("Loaded GameConfig: Title='{}', Width={}, Height={}, FullScreen={}", m_Title, m_Width, m_Height, m_FullScreen);
		return true;
	}
}