#include "Core/EEContext.h"
#include "Core/Logger.h"
#include "Core/EngineUtils.h"
#include "Assets/GameConfig.h"
#include "Renderer/DX11/RendererDX11.h"

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

		try
		{
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

			m_Renderer = std::make_unique<RendererDX11>();
			m_Renderer->Initialize();

			m_CameraManager = std::make_unique<CameraManager>();

			if (!m_SceneManager)
			{
				m_SceneManager = std::make_unique<SceneManager>();
			}
			m_NoesisUI = std::make_unique<NoesisUI>();
		}
		catch (Ethereal::COMException* e)
		{
			// MessageBox to display the error message
			MessageBoxW(NULL, e->what(), L"Ethereal Engine Error", MB_OK | MB_ICONERROR);
		}
	}

	SceneManager& EEContext::GetSceneManager()
	{
		if (!m_SceneManager)
		{
			m_SceneManager = std::make_unique<SceneManager>();
		}
		return *m_SceneManager;
	}

} // namespace Ethereal