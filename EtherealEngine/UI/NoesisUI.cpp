#include "UI/NoesisUI.h"
#include "Core/Logger.h"
#include "Core/EEContext.h"

#include <fstream>
#include <nlohmann/json.hpp>

#include <NsGui/Uri.h>
#include <NoesisPCH.h> // make sure PCH is used if required by your Noesis setup

using namespace Noesis;
using namespace NoesisApp;

namespace NoesisApp
{
	Noesis::Ptr<Noesis::RenderDevice> CreateRenderDevice(ID3D11Device* device, ID3D11DeviceContext* context)
	{
		return Noesis::MakePtr<D3D11RenderDevice>(context, false);
	}
}

namespace Ethereal
{
	NoesisUI::NoesisUI()
	{
		Initialize();
	}

	NoesisUI::~NoesisUI()
	{
		m_View->GetRenderer()->Shutdown();
		m_View.Reset();
		m_RenderDevice.Reset();
		Noesis::GUI::Shutdown();
	}

	void NoesisUI::Initialize()
	{
		if (m_Initialized) return;

		Noesis::SetLogHandler([](const char*, uint32_t, uint32_t level, const char*, const char* msg)
			{
				switch (level)
				{
				case 0: case 1: case 2: LOG_INFO("[NOESIS] {}", msg); break;
				case 3: LOG_WARN("[NOESIS] {}", msg); break;
				case 4: LOG_ERROR("[NOESIS] {}", msg); break;
				default: LOG_INFO("[NOESIS] {}", msg); break;
				}
			});

		SetLicense();
		GUI::Init();

		// Set resource providers
		auto xamlPath = (GetAssetsDirectory() / "UI").string();
		auto fontPath = (GetAssetsDirectory() / "Fonts").string();
		auto texturePath = (GetAssetsDirectory() / "UI").string();

		GUI::SetXamlProvider(MakePtr<LocalXamlProvider>(xamlPath.c_str()));
		GUI::SetFontProvider(MakePtr<LocalFontProvider>(fontPath.c_str()));
		GUI::SetTextureProvider(MakePtr<LocalTextureProvider>(texturePath.c_str()));

		const char* fonts[] = { "Fonts/Roboto-Regular.ttf", "Arial", "Segoe UI Emoji" };
		GUI::SetFontFallbacks(fonts, 3);
		GUI::SetFontDefaultProperties(15.0f, FontWeight_Normal, FontStretch_Normal, FontStyle_Normal);

		InitializeRenderDevice();
		m_Initialized = true;
	}

	bool NoesisUI::SetLicense()
	{
		std::ifstream file("C:\\Projects\\noesis.json");
		if (!file.is_open())
		{
			LOG_ERROR("Failed to open Noesis license file");
			return false;
		}

		nlohmann::json json;
		try
		{
			file >> json;
		}
		catch (const std::exception& e)
		{
			LOG_ERROR("Failed to parse license JSON: {}", e.what());
			return false;
		}

		std::string user = json.value("User", "");
		std::string key = json.value("Key", "");
		if (user.empty() || key.empty())
		{
			LOG_ERROR("Missing User or Key in Noesis license");
			return false;
		}

		GUI::SetLicense(user.c_str(), key.c_str());
		LOG_INFO("Noesis license set successfully: {}", user);
		return true;
	}

	void NoesisUI::InitializeRenderDevice()
	{
		auto* device = static_cast<ID3D11Device*>(EEContext::Get().GetDevice());
		auto* context = static_cast<ID3D11DeviceContext*>(EEContext::Get().GetContext());

		m_RenderDevice = NoesisApp::CreateRenderDevice(device, context);
	}

	void NoesisUI::LoadXamlView(const std::string& name)
	{
		if (!m_Initialized)
		{
			Initialize();
		}
		if (!m_RenderDevice)
		{
			InitializeRenderDevice();
		}
		auto width = EEContext::Get().GetWidth();
		auto height = EEContext::Get().GetHeight();

		auto xaml = GUI::LoadXaml<FrameworkElement>(Uri(name.c_str()));
		m_View = GUI::CreateView(xaml);
		m_View->SetFlags(RenderFlags_PPAA | RenderFlags_LCD);
		m_View->SetSize(width, height);
		m_View->GetRenderer()->Init(m_RenderDevice);
	}

	void NoesisUI::UnloadXamlView()
	{
		if (m_View)
		{
			m_View->GetRenderer()->Shutdown();
			m_View.Reset();
		}
	}

	void NoesisUI::Render()
	{
		if (!m_Initialized || !m_View)
		{
			LOG_ERROR("NoesisUI not initialized or view not loaded");
			return;
		}
		auto deltaTime = EEContext::Get().GetDeltaTime();
		m_View->Update(deltaTime);

		// Update UI render tree and draw internal textures
		m_View->GetRenderer()->UpdateRenderTree();
		m_View->GetRenderer()->RenderOffscreen();

		m_View->GetRenderer()->Render();
	}

}