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

namespace Ethereal
{
	class ETHEREAL_API NoesisUI
	{
	public:
		NoesisUI();
		virtual ~NoesisUI();

		void Initialize();
		bool SetLicense();
		void LoadXamlView(const std::string& name);
		void UnloadXamlView();
		void Render();

		Noesis::Ptr<Noesis::IView> GetView() const
		{
			return m_View;
		}
		Noesis::Ptr<Noesis::RenderDevice> GetRenderDevice() const
		{
			return m_RenderDevice;
		}

	private:
		void InitializeRenderDevice();

	private:
		Noesis::Ptr<Noesis::IView> m_View;
		Noesis::Ptr<Noesis::RenderDevice> m_RenderDevice;

		bool m_Initialized = false;
	};
}