#include "Renderer/DX11/RendererDX11.h"
#include "Core/Logger.h"
#include "Core/EngineUtils.h"
#include "Renderer/DX11/AdapterReader.h"
#include "Core/EEContext.h"
#include <d3d11_1.h> // For CD3D11_* helpers

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
		try
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
			COM_ERROR_IF_FAILED(hr, "Failed to create DirectX 11 device and swap chain");

			// Create the back buffer

			Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
			hr = m_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backBuffer.GetAddressOf()));
			COM_ERROR_IF_FAILED(hr, "Failed to get back buffer from swap chain");

			// Create a render target view
			hr = m_Device->CreateRenderTargetView(backBuffer.Get(), NULL, &m_RenderTargetView);
			COM_ERROR_IF_FAILED(hr, "Failed to create render target view");

			// Create depth stencil
			CD3D11_TEXTURE2D_DESC depthStencilDesc(
				DXGI_FORMAT_D24_UNORM_S8_UINT, // Format
				width,                         // Width
				height,                        // Height
				1,                             // ArraySize
				1,                             // MipLevels
				D3D11_BIND_DEPTH_STENCIL       // BindFlags
			);

			hr = m_Device->CreateTexture2D(&depthStencilDesc, NULL, &m_DepthStencilBuffer);
			COM_ERROR_IF_FAILED(hr, "Failed to create depth stencil buffer");

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
			COM_ERROR_IF_FAILED(hr, "Failed to create depth stencil state");

			D3D11_VIEWPORT viewport;
			ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));
			viewport.TopLeftX = 0.0f; // Start at the top-left corner
			viewport.TopLeftY = 0.0f; // Start at the top-left corner
			viewport.Width = static_cast<float>(width); // Use default width
			viewport.Height = static_cast<float>(height); // Use default height
			viewport.MinDepth = 0.0f; // Minimum depth
			viewport.MaxDepth = 1.0f; // Maximum depth

			m_Context->RSSetViewports(1, &viewport); // Set the viewport

			//Create Rasterizer State
			CD3D11_RASTERIZER_DESC rasterizerDesc(D3D11_DEFAULT);
			hr = m_Device->CreateRasterizerState(&rasterizerDesc, m_RasterizerState.GetAddressOf());
			COM_ERROR_IF_FAILED(hr, "Failed to create rasterizer state");

			//Create Rasterizer State for culling front
			//CD3D11_RASTERIZER_DESC rasterizerDesc_CullFront(D3D11_DEFAULT);
			//rasterizerDesc_CullFront.CullMode = D3D11_CULL_MODE::D3D11_CULL_FRONT;
			//hr = m_Device->CreateRasterizerState(&rasterizerDesc_CullFront, m_RasterizerState_CullFront.GetAddressOf());
			//if (FAILED(hr))
			//{
			//	LOG_ERROR("Failed to create rasterizer state: {}", hr);
			//	return;
			//}

			// Create the blend state
			D3D11_BLEND_DESC blendDesc;
			ZeroMemory(&blendDesc, sizeof(D3D11_BLEND_DESC));
			//blendDesc.AlphaToCoverageEnable = FALSE; // Disable alpha to coverage
			//blendDesc.IndependentBlendEnable = FALSE; // Disable independent blending	

			D3D11_RENDER_TARGET_BLEND_DESC rtbs;
			ZeroMemory(&rtbs, sizeof(D3D11_RENDER_TARGET_BLEND_DESC));
			rtbs.BlendEnable = true; // Enable blending
			rtbs.SrcBlend = D3D11_BLEND_SRC_ALPHA; // Source blend factor
			rtbs.DestBlend = D3D11_BLEND_INV_SRC_ALPHA; // Destination blend factor
			rtbs.BlendOp = D3D11_BLEND_OP_ADD; // Blend operation
			rtbs.SrcBlendAlpha = D3D11_BLEND_ONE; // Source alpha blend factor
			rtbs.DestBlendAlpha = D3D11_BLEND_ZERO; // Destination alpha blend factor
			rtbs.BlendOpAlpha = D3D11_BLEND_OP_ADD; // Alpha blend operation
			rtbs.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL; // Write to all color channels


			blendDesc.RenderTarget[0] = rtbs; // Use the previously defined render target blend state
			hr = m_Device->CreateBlendState(&blendDesc, &m_BlendState);
			COM_ERROR_IF_FAILED(hr, "Failed to create blend state");

			// Create sampler state
			D3D11_SAMPLER_DESC samplerDesc;
			ZeroMemory(&samplerDesc, sizeof(D3D11_SAMPLER_DESC));
			samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR; // Linear filtering
			samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP; // Wrap texture coordinates in U direction
			samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP; // Wrap texture coordinates in V direction
			samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP; // Wrap texture coordinates in W direction
			samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER; // Always pass comparison
			samplerDesc.MinLOD = 0.0f; // Minimum LOD level
			samplerDesc.MaxLOD = D3D11_FLOAT32_MAX; // Maximum LOD level

			hr = m_Device->CreateSamplerState(&samplerDesc, &m_SamplerState);
			COM_ERROR_IF_FAILED(hr, "Failed to create sampler state");

			InitImGui(hwnd); // Initialize ImGui with the main window handle
			// Finished initializing DirectX 11
		}
		catch (const COMException& e)
		{
			MessageBoxW(nullptr, e.what(), L"COM Exception", MB_OK | MB_ICONERROR); // Show a message box with the COM exception message
		}
		catch (const std::exception& e)
		{
			MessageBoxA(nullptr, e.what(), "Exception", MB_OK | MB_ICONERROR); // Show a message box with the exception message			
		}
		catch (...)
		{
			MessageBoxW(nullptr, L"An unknown error occurred during DirectX 11 initialization.", L"Unknown Exception", MB_OK | MB_ICONERROR); // Show a message box for unknown exceptions
		}
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

		int32_t windowWidth = EEContext::Get().GetWidth(); // Get the current window width
		int32_t windowHeight = EEContext::Get().GetHeight(); // Get the current window height)
		// Start the Dear ImGui frame
		ImGuiIO& io = ImGui::GetIO();
		io.DisplaySize = ImVec2((float)windowWidth, (float)windowHeight); // Your actual window size in pixels
		io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f); // For most cases, unless you have DPI scaling
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
	}		
	
	void RendererDX11::Draw(const std::shared_ptr<GameObject>& obj)
	{
		auto& model = obj->GetModel();

		// --- Set once per model ---
		m_Context->IASetInputLayout(model.GetInputLayout());
		m_Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		m_Context->RSSetState(m_RasterizerState.Get());
		m_Context->OMSetDepthStencilState(m_DepthStencilState.Get(), 1);
		m_Context->OMSetBlendState(m_BlendState.Get(), nullptr, 0xFFFFFFFF);
		m_Context->PSSetSamplers(0, 1, m_SamplerState.GetAddressOf());

		// Camera and world matrix setup (set once per model)
		
		DirectX::XMMATRIX world = obj->GetTransform().GetMatrix();
		if (!model.GetConstantBuffer().IsBufferInitialized())
		{
			model.GetConstantBuffer().Initialize(m_Device.Get(), m_Context.Get());
		}
		auto& camera = EEContext::Get().GetCameraManager().GetCurrentCamera();
		auto& constantBuffer = model.GetConstantBuffer();
		constantBuffer.data.mat = DirectX::XMMatrixTranspose(world * camera.GetViewMatrix() * camera.GetProjectionMatrix());
		constantBuffer.ApplyChanges();
		m_Context->VSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());

		const auto& meshes = model.GetMeshes();
		const auto& materials = model.GetMaterials();

		UINT offset = 0;
		for (const auto& mesh : meshes)
		{
			if (!mesh) continue;

			// Determine which material to use for this mesh
			std::shared_ptr<Material> material;
			if (materials.size() == 1)
				material = materials[0];
			/*else if (mesh->GetMaterialIndex() < materials.size())
				material = materials[mesh->GetMaterialIndex()];*/ // #TODO: Mesh material index
			else
				material = nullptr;

			if (!material) continue;

			// Set shaders and resources for this material
			m_Context->VSSetShader(material->GetVertexShader()->GetVertexShader(), NULL, 0);
			m_Context->PSSetShader(material->GetPixelShader()->GetPixelShader(), NULL, 0);
			ID3D11ShaderResourceView* diffuseSRV = material->GetDiffuseTexture();
			m_Context->PSSetShaderResources(0, 1, &diffuseSRV);

			if (!material->GetConstantBuffer().IsBufferInitialized())
			{
				material->GetConstantBuffer().Initialize(m_Device.Get(), m_Context.Get());
			}
			material->GetConstantBuffer().data.alpha = 1.0f;
			material->GetConstantBuffer().ApplyChanges();
			m_Context->PSSetConstantBuffers(0, 1, material->GetConstantBuffer().GetAddressOf());

			ID3D11Buffer* vb = mesh->GetVertexBuffer();
			m_Context->IASetVertexBuffers(0, 1, &vb, mesh->GetStridePtr(), &offset);
			m_Context->IASetIndexBuffer(mesh->GetIndexBuffer(), DXGI_FORMAT_R32_UINT, 0);

			m_Context->DrawIndexed((UINT)mesh->GetIndexCount(), 0, 0);
		}
	}

	void RendererDX11::EndFrame()
	{
		ImGui::Render();
		
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