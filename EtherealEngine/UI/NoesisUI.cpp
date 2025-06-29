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

	inline Noesis::Key ConvertVKToNoesisKey(unsigned char vk)
	{
		using namespace Noesis;

		switch (vk)
		{
		case VK_BACK: return Key_Back;
		case VK_TAB: return Key_Tab;
		case VK_RETURN: return Key_Return;
		case VK_LSHIFT: return Key_LeftShift;
		case VK_RSHIFT: return Key_RightShift;
		case VK_LCONTROL: return Key_LeftCtrl;
		case VK_RCONTROL: return Key_RightCtrl;
		case VK_LMENU: return Key_LeftAlt; // VK_LMENU is Left Alt, VK_RMENU is Right Alt
		case VK_RMENU: return Key_RightAlt;
		case VK_CAPITAL: return Key_CapsLock;
		case VK_ESCAPE: return Key_Escape;
		case VK_SPACE: return Key_Space;
		case VK_PRIOR: return Key_PageUp;
		case VK_NEXT: return Key_PageDown;
		case VK_END: return Key_End;
		case VK_HOME: return Key_Home;
		case VK_LEFT: return Key_Left;
		case VK_UP: return Key_Up;
		case VK_RIGHT: return Key_Right;
		case VK_DOWN: return Key_Down;
		case VK_INSERT: return Key_Insert;
		case VK_DELETE: return Key_Delete;
		case '0': return Key_D0;
		case '1': return Key_D1;
		case '2': return Key_D2;
		case '3': return Key_D3;
		case '4': return Key_D4;
		case '5': return Key_D5;
		case '6': return Key_D6;
		case '7': return Key_D7;
		case '8': return Key_D8;
		case '9': return Key_D9;
		case 'A': return Key_A;
		case 'B': return Key_B;
		case 'C': return Key_C;
		case 'D': return Key_D;
		case 'E': return Key_E;
		case 'F': return Key_F;
		case 'G': return Key_G;
		case 'H': return Key_H;
		case 'I': return Key_I;
		case 'J': return Key_J;
		case 'K': return Key_K;
		case 'L': return Key_L;
		case 'M': return Key_M;
		case 'N': return Key_N;
		case 'O': return Key_O;
		case 'P': return Key_P;
		case 'Q': return Key_Q;
		case 'R': return Key_R;
		case 'S': return Key_S;
		case 'T': return Key_T;
		case 'U': return Key_U;
		case 'V': return Key_V;
		case 'W': return Key_W;
		case 'X': return Key_X;
		case 'Y': return Key_Y;
		case 'Z': return Key_Z;
		case VK_F1: return Key_F1;
		case VK_F2: return Key_F2;
		case VK_F3: return Key_F3;
		case VK_F4: return Key_F4;
		case VK_F5: return Key_F5;
		case VK_F6: return Key_F6;
		case VK_F7: return Key_F7;
		case VK_F8: return Key_F8;
		case VK_F9: return Key_F9;
		case VK_F10: return Key_F10;
		case VK_F11: return Key_F11;
		case VK_F12: return Key_F12;
		default:
			return Key_None;
		}
	}

	void NoesisUI::HandleInput(Noesis::IView* view)
	{
		/****************** Mouse ******************/		
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

		/**************** Keyboard *****************/
		auto& keyboard = EEContext::Get().GetWindow().GetKeyboard();

		while (auto evt = keyboard.ReadEvent())
		{
			switch (evt->GetType())
			{
			case Keyboard::Event::Type::Press:
				view->KeyDown(ConvertVKToNoesisKey(evt->GetKeyCode()));
				break;

			case Keyboard::Event::Type::Release:
				view->KeyUp(ConvertVKToNoesisKey(evt->GetKeyCode()));
				break;

			case Keyboard::Event::Type::Char:
				view->Char(evt->GetChar());
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