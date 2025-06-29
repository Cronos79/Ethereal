#pragma once
#include "Core/EtherealIncludes.h"

#include <string>
#include <d3d11.h>

// Noesis headers
#include <NsCore/Ptr.h>
#include <NsGui/IView.h>
#include <NsGui/IntegrationAPI.h>
#include <NsGui/FrameworkElement.h>

#include <NsApp/LocalXamlProvider.h>
#include <NsApp/LocalFontProvider.h>
#include <NsApp/LocalTextureProvider.h>

#include <NsRender/RenderDevice.h>
#include <NsRender/D3D11RenderDeviceApi.h>  // <-- Needed for D3D11CreateRenderTarget
#include "D3D11RenderDevice.h"
#include <vector>
#include <NsRender/RenderContext.h>
#include <NsApp/Display.h>

namespace Ethereal
{
	struct NoesisView
	{
		std::string Name;
		std::string Path;
		bool IsVisable = false;
		Noesis::Ptr<Noesis::IView> View;
	};

	class RendInst
	{
	public:
		void Resize();

	private:

	};

	class ETHEREAL_API NoesisUI
	{
	public:
		NoesisUI();
		virtual ~NoesisUI();

		void Initialize();
		bool SetLicense();
		void InitializeRenderDevice();
		void LoadXamlView(const std::string& path, const std::string& name = "DefaultView", bool isVisible = false);
		void UnloadXamlViews();
		void Render();
		void HandleInput(Noesis::IView* view);

		Noesis::Ptr<Noesis::RenderDevice> GetRenderDevice() const
		{
			return m_RenderDevice;
		}

	private:
		void SetupDisplay(Noesis::IView* view);

	private:	
		std::vector<NoesisView> m_Views;
		Noesis::Ptr<Noesis::RenderDevice> m_RenderDevice;
		bool m_Initialized = false;
		Noesis::Ptr<NoesisApp::Display> m_Display;
	};
}