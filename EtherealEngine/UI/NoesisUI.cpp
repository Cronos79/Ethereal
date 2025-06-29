#include "UI/NoesisUI.h"
#include "Core/Logger.h"
#include "Core/EEContext.h"

#include <fstream>
#include <nlohmann/json.hpp>

#include <NsGui/Uri.h>
#include <NoesisPCH.h> // make sure PCH is used if required by your Noesis setup
#include <NsApp/Launcher.h>
#include "Platform/EEWindows.h"
#include "Platform/Mouse.h"

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
		UnloadXamlViews();
		 if (m_RenderDevice)
		{
			m_RenderDevice.Reset();
			m_RenderDevice = nullptr;
		} 	
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

		// Register app components. We need a few in this example, like Display and RenderContext
		NoesisApp::Launcher::RegisterAppComponents();

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

	void NoesisUI::SetupDisplay(Noesis::IView* view)
	{
		
	}

	void NoesisUI::LoadXamlView(const std::string& path, const std::string& name, bool isVisible)
	{
		if (!m_Initialized)
		{
			Initialize();
		}
		if (m_RenderDevice == nullptr)
		{
			InitializeRenderDevice();
		}
		auto width = EEContext::Get().GetWidth();
		auto height = EEContext::Get().GetHeight();

		NoesisView nView = {};
		auto xaml = GUI::LoadXaml<FrameworkElement>(Uri(path.c_str()));
		nView.View = GUI::CreateView(xaml);
		nView.View->SetFlags(RenderFlags_PPAA | RenderFlags_LCD);
		nView.View->SetSize(width, height);
		nView.View->GetRenderer()->Init(m_RenderDevice);

		nView.Name = name;
		nView.IsVisable = isVisible;
		SetupDisplay(nView.View);
		m_Views.push_back(nView);
	}

	void NoesisUI::UnloadXamlViews()
	{
		// loop through all views and unload them
		for (auto& view : m_Views)
		{
			if (view.View)
			{
				view.View->GetRenderer()->Shutdown();
				view.View.Reset();
			}
		}
	}

	void NoesisUI::Render()
	{
		if (!m_Initialized)
		{
			LOG_ERROR("NoesisUI not initialized or view not loaded");
			return;
		}
		auto deltaTime = EEContext::Get().GetDeltaTime();
		for (auto& view : m_Views)
		{	
			if (view.View && view.IsVisable)
			{
				view.View->Update(deltaTime);
				HandleInput(view.View);
				// Update UI render tree and draw internal textures
				view.View->GetRenderer()->UpdateRenderTree();
				view.View->GetRenderer()->RenderOffscreen();

				view.View->GetRenderer()->Render();
			}
		}	
	}

	void NoesisUI::HandleInput(Noesis::IView* view)
	{
		auto& mouse = EEContext::Get().GetWindow().GetMouse();
		int x1 = mouse.GetPosX();
		int y1 = mouse.GetPosY();
		view->MouseMove(x1, y1);
		while (auto e = mouse.Read())
		{
			auto evt = *e;
			int x = evt.GetPosX();
			int y = evt.GetPosY();

			switch (evt.GetType())
			{
			case Mouse::Event::Type::LPress:
				view->MouseButtonDown(x, y, Noesis::MouseButton_Left);
				break;

			case Mouse::Event::Type::LRelease:
				view->MouseButtonUp(x, y, Noesis::MouseButton_Left);
				break;

			case Mouse::Event::Type::RPress:
				view->MouseButtonDown(x, y, Noesis::MouseButton_Right);
				break;

			case Mouse::Event::Type::RRelease:
				view->MouseButtonUp(x, y, Noesis::MouseButton_Right);
				break;

			case Mouse::Event::Type::WheelUp:
				view->MouseWheel(x, y, +1.0f);
				break;

			case Mouse::Event::Type::WheelDown:
				view->MouseWheel(x, y, -1.0f);
				break;

				// Optional:
			case Mouse::Event::Type::Enter:
			case Mouse::Event::Type::Leave:
				// These might map to custom behaviors in Noesis, or be ignored
				break;
			}
		}

	}

	void RendInst::Resize()
	{
		auto renderer = EEContext::Get().GetRenderer();
		if (renderer)
		{
			renderer->Resize();
		}
	}

}