#include "Renderer/DX11/RendererDX11.h"
#include "Core/Logger.h"
#include "Core/EngineUtils.h"
#include "Renderer/DX11/AdapterReader.h"
#include "Core/EEContext.h"

namespace Ethereal
{

	RendererDX11::RendererDX11()
	{

	}

	RendererDX11::~RendererDX11()
	{

	}

	void RendererDX11::Initialize()
	{	
		std::vector<AdapterData> adapters = AdapterReader::GetAdapters();

		// Get adapter with most dedicated memory
		AdapterData bestAdapter = adapters[0];
		for (const auto& adapter : adapters)
		{
			if (adapter.desc.DedicatedVideoMemory > bestAdapter.desc.DedicatedVideoMemory)
			{
				bestAdapter = adapter;
			}
		}

		LOG_INFO("Using adapter: {}", WCharToString(bestAdapter.desc.Description));

		HWND hwnd = EEContext::Get().GetWindowHandle(); // Get the main window handle
		if (!hwnd)
		{
			LOG_ERROR("Failed to get window handle for DirectX 11 initialization");
			return;
		}
		if (bestAdapter.pAdapter.Get() == nullptr)
		{
			LOG_ERROR("No suitable DirectX 11 adapter found");
			return;
		}
		int width = EEContext::Get().GetWidth(); // Get the default width from context
		int height = EEContext::Get().GetHeight(); // Get the default height from context

		// Swap chain description
		DXGI_SWAP_CHAIN_DESC scd;
		ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));
		scd.BufferDesc.Width = width; // Use default width
		scd.BufferDesc.Height = height; // Use default height
		scd.BufferDesc.RefreshRate.Numerator = 60; // 60Hz refresh rate
		scd.BufferDesc.RefreshRate.Denominator = 1; // 60Hz refresh rate
		scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // Common format	
		scd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED; // Default scanline ordering
		scd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED; // Default scaling
		scd.SampleDesc.Count = 1; // No multisampling
		scd.SampleDesc.Quality = 0; // No multisampling quality
		scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // Use as render target
		scd.BufferCount = 1; // Single back buffer			
		scd.OutputWindow = hwnd; // Use the main window
		scd.Windowed = TRUE; // Windowed mode
		scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD; // Discard old buffers		
		scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH; // Allow mode switching

		HRESULT hr = D3D11CreateDeviceAndSwapChain(
			bestAdapter.pAdapter.Get(),
			D3D_DRIVER_TYPE_UNKNOWN, // Use the adapter directly, not a software driver
			NULL, // No software rasterizer
			NULL,//D3D11_CREATE_DEVICE_BGRA_SUPPORT | D3D11_CREATE_DEVICE_SINGLETHREADED,
			NULL, // No feature levels specified, use the default
			0, // No feature levels specified
			D3D11_SDK_VERSION, // Use the DirectX 11 SDK version
			&scd, // Swap chain description
			&m_SwapChain, // Swap chain output
			&m_Device, // Device output
			nullptr, // No feature level output
			&m_Context // Device context output
		);
		if (FAILED(hr))
		{
			LOG_ERROR("Failed to create DirectX 11 device and swap chain: {}", hr);
			return;
		}

		// Create the back buffer
		
		Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
		hr = m_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backBuffer.GetAddressOf()));
		if (FAILED(hr))
		{
			LOG_ERROR("Failed to get back buffer from swap chain: {}", hr);
			return;
		}
		// Create a render target view
		hr = m_Device->CreateRenderTargetView(backBuffer.Get(), NULL, &m_RenderTargetView);
		if (FAILED(hr))
		{
			LOG_ERROR("Failed to create render target view: {}", hr);
			return;
		}

		// Create depth stencil
		D3D11_TEXTURE2D_DESC depthStencilDesc;
		ZeroMemory(&depthStencilDesc, sizeof(D3D11_TEXTURE2D_DESC));
		depthStencilDesc.Width = width; // Use default width
		depthStencilDesc.Height = height; // Use default height
		depthStencilDesc.MipLevels = 1; // Single mip level
		depthStencilDesc.ArraySize = 1; // Single array size
		depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; // 24-bit depth, 8-bit stencil
		depthStencilDesc.SampleDesc.Count = 1; // No multisampling
		depthStencilDesc.SampleDesc.Quality = 0; // No multisampling quality
		depthStencilDesc.Usage = D3D11_USAGE_DEFAULT; // Default usage
		depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL; // Bind as depth stencil
		depthStencilDesc.CPUAccessFlags = 0; // No CPU access
		depthStencilDesc.MiscFlags = 0; // No special options

		hr = m_Device->CreateTexture2D(&depthStencilDesc, NULL, &m_DepthStencilBuffer);
		if (FAILED(hr))
		{
			LOG_ERROR("Failed to create depth stencil buffer: {}", hr);
			return;
		}

		// Create depth stencil view
		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
		ZeroMemory(&depthStencilViewDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
		depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; // Match the depth stencil format
		depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D; // 2D texture
		depthStencilViewDesc.Texture2D.MipSlice = 0; // Use the first mip level
		depthStencilViewDesc.Flags = 0; // No special flags

		m_Device->CreateDepthStencilView(m_DepthStencilBuffer.Get(), &depthStencilViewDesc, &m_DepthStencilView);

		m_Context->OMSetRenderTargets(1, m_RenderTargetView.GetAddressOf(), m_DepthStencilView.Get()); // Set the render target view	

		// Create Depth Stencil State
		D3D11_DEPTH_STENCIL_DESC depthstencildesc;
		ZeroMemory(&depthstencildesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
		depthstencildesc.DepthEnable = TRUE; // Enable depth testing
		depthstencildesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL; // Write to depth buffer	
		depthstencildesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL; // Use less comparison for depth

		hr = m_Device->CreateDepthStencilState(&depthstencildesc, &m_DepthStencilState);
		if (FAILED(hr))
		{
			LOG_ERROR("Failed to create depth stencil state: {}", hr);
			return;
		}

		D3D11_VIEWPORT viewport;
		ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));
		viewport.TopLeftX = 0.0f; // Start at the top-left corner
		viewport.TopLeftY = 0.0f; // Start at the top-left corner
		viewport.Width = static_cast<float>(width); // Use default width
		viewport.Height = static_cast<float>(height); // Use default height
		viewport.MinDepth = 0.0f; // Minimum depth
		viewport.MaxDepth = 1.0f; // Maximum depth

		m_Context->RSSetViewports(1, &viewport); // Set the viewport

		// Create Rasterizer State
		D3D11_RASTERIZER_DESC rasterizerDesc;
		ZeroMemory(&rasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));
		rasterizerDesc.FillMode = D3D11_FILL_SOLID; // Solid fill mode
		rasterizerDesc.CullMode = D3D11_CULL_BACK; // Back-face culling
		rasterizerDesc.FrontCounterClockwise = FALSE; // Clockwise front face
		hr = m_Device->CreateRasterizerState(&rasterizerDesc, &m_RasterizerState);
		if (FAILED(hr))
		{
			LOG_ERROR("Failed to create rasterizer state: {}", hr);
			return;
		}

		InitImGui(hwnd); // Initialize ImGui with the main window handle
		// Finished initializing DirectX 11
	}
	void RendererDX11::InitImGui(HWND hwnd)
	{
		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
		//io.ConfigViewportsNoAutoMerge = true;
		//io.ConfigViewportsNoTaskBarIcon = true;
		//io.ConfigViewportsNoDefaultParent = true;
		//io.ConfigDockingAlwaysTabBar = true;
		//io.ConfigDockingTransparentPayload = true;
		//io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleFonts;     // FIXME-DPI: Experimental. THIS CURRENTLY DOESN'T WORK AS EXPECTED. DON'T USE IN USER APP!
		//io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleViewports; // FIXME-DPI: Experimental.

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();
		//ImGui::StyleColorsLight();

		// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

		// Setup Platform/Renderer backends
		ImGui_ImplWin32_Init(hwnd);
		ImGui_ImplDX11_Init(m_Device.Get(), m_Context.Get());
	}


	void RendererDX11::BeginFrame()
	{
		float bgcolor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		m_Context->ClearRenderTargetView(m_RenderTargetView.Get(), bgcolor); // Clear the render target view
		m_Context->ClearDepthStencilView(m_DepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0); // Clear the depth stencil view

		// Start the Dear ImGui frame
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
	}

	void RendererDX11::Draw(GameObject obj)
	{
		m_Context->IASetInputLayout(obj.GetMesh()->GetInputLayout());
		m_Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		m_Context->RSSetState(m_RasterizerState.Get()); // Set the rasterizer state

		m_Context->VSSetShader(obj.GetMaterial()->GetVertexShader()->GetVertexShader(), NULL, 0);
		m_Context->PSSetShader(obj.GetMaterial()->GetPixelShader()->GetPixelShader(), NULL, 0);
		m_Context->OMSetDepthStencilState(m_DepthStencilState.Get(), 1); // Set the depth stencil state

		UINT stride = sizeof(Vertex);
		UINT offset = 0;
		ID3D11Buffer* vb = obj.GetMesh()->GetVertexBuffer();
		m_Context->IASetVertexBuffers(0, 1, &vb, &stride, &offset);

		m_Context->Draw(3, 0);

		bool show_demo_window = true; // Show ImGui demo window for debugging
		if (show_demo_window)
			ImGui::ShowDemoWindow(&show_demo_window);
	}

	void RendererDX11::EndFrame()
	{
		ImGui::Render();
		
		//g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, nullptr);
		//g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha);
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		// Update and Render additional Platform Windows
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}

		m_SwapChain->Present(1, 0); // Present the swap chain
	}

	void RendererDX11::Shutdown()
	{
	}

}