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
		LOG_INFO("Found {} adapters", adapters.size());

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

		m_Context->OMSetRenderTargets(1, m_RenderTargetView.GetAddressOf(), nullptr); // Set the render target view

		InitializeShaders(); // Initialize shaders

		// Finished initializing DirectX 11
		LOG_INFO("DirectX 11 initialized successfully");
	}

	void RendererDX11::InitializeShaders()
	{
		// D3D11_APPEND_ALIGNED_ELEMENT
		D3D11_INPUT_ELEMENT_DESC layout[] = 
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};
		
		EEContext::Get().GetAssetManager().LoadShader("VertexShader", ShaderType::VERTEX_SHADER);
		// make uniqe_ptr to the shader asset
		m_VertexShaderAsset = EEContext::Get().GetAssetManager().Get<Shaders>("VertexShader");
		m_VertexBuffer = m_VertexShaderAsset->GetShaderBuffer();

		HRESULT hr = m_Device->CreateInputLayout(
			layout,
			ARRAYSIZE(layout),
			m_VertexBuffer->GetBufferPointer(), // Shader bytecode will be provided later
			m_VertexBuffer->GetBufferSize(), // Size of the shader bytecode
			&m_InputLayout // Output input layout
		);
		if (FAILED(hr))
		{
			LOG_ERROR("Failed to create input layout: {}", hr);
			return;
		}
	}

	void RendererDX11::BeginFrame()
	{
		float bgcolor[] = { 1.0f, 0.0f, 0.0f, 1.0f };
		m_Context->ClearRenderTargetView(m_RenderTargetView.Get(), bgcolor); // Clear the render target view
	}

	void RendererDX11::Draw(GameObject obj)
	{

	}

	void RendererDX11::EndFrame()
	{
		m_SwapChain->Present(1, 0); // Present the swap chain
	}

	void RendererDX11::Shutdown()
	{
	}

}