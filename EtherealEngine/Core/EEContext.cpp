#include "Core/EEContext.h"
#include "Core/Logger.h"
#include "Core/EngineUtils.h"
#include "Assets/GameConfig.h"

namespace Ethereal
{

	EEContext::EEContext()
	{
	
	}

	EEContext::~EEContext()
	{
		m_Window.reset(); // Reset the unique pointer to release the window resources
		LOG_INFO("Ethereal Engine Context Destroyed");
	}

	void EEContext::Initialize()
	{
		// Initialize the Logger
		Logger::Init();
		
		m_AssetManager = std::make_unique<AssetManager>();
		m_AssetManager->Initialize();
		auto assetRegistryPath = GetAssetsDirectory() / "asset_registry.json";
		m_AssetManager->LoadRegistry(assetRegistryPath.string());
		m_AssetManager->LoadGameConfig("GameConfig");

		// Get GameConfig and set Settings
		auto cfg = m_AssetManager->Get<GameConfig>("GameConfig");

		m_GameSettings.WindowTitle = cfg->GetWindowsTitle();
		m_GameSettings.Width = cfg->GetWidth();
		m_GameSettings.Height = cfg->GetHeight();
		m_GameSettings.FullScreen = cfg->GetFullScreen();
		
		// Initialize the unique pointers
		m_Window = std::make_unique<EEWindows>();
		m_Window->Initialize();

		LOG_INFO("Ethereal Engine Context Initialized");
	}

} // namespace Ethereal