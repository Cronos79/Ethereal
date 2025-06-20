#pragma once
#include "Core/EtherealIncludes.h"
#include "Renderer/Renderer.h"
#include "Platform/EEWinIncludes.h"
#include <wrl/client.h>
#include "Renderer/DX11/AdapterReader.h"
#include <memory>

#include "Assets/Shaders.h"
#include "Assets/GameObject.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"
#include "Camera.h"

namespace Ethereal
{
	class ETHEREAL_API RendererDX11 : public Renderer
	{
	public:
		RendererDX11();
		~RendererDX11();

		void Initialize() override;
		void InitImGui(HWND hwnd);
		void BeginFrame() override;
		void Draw(GameObject obj) override;
		void EndFrame() override;
		void Shutdown() override;

		// Assessors
		void* GetDevice() const override
		{
			return m_Device.Get();
		}
		void* GetContext() const override
		{
			return m_Context.Get();
		}
		IDXGISwapChain* GetSwapChain() const
		{
			return m_SwapChain.Get();
		}
		ID3D11RenderTargetView* GetRenderTargetView() const
		{
			return m_RenderTargetView.Get();
		}
		ID3D11DepthStencilView* GetDepthStencilView() const
		{
			return m_DepthStencilView.Get();
		}

	private:
		Microsoft::WRL::ComPtr<ID3D11Device> m_Device;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_Context;
		Microsoft::WRL::ComPtr<IDXGISwapChain> m_SwapChain;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_RenderTargetView;

		Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_RasterizerState;

		Microsoft::WRL::ComPtr<ID3D11Texture2D> m_DepthStencilBuffer;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_DepthStencilView;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_DepthStencilState;


		
		//Microsoft::WRL::ComPtr<ID3D11VertexShader> m_VertexShader;
	
		Microsoft::WRL::ComPtr<ID3D11PixelShader> m_PixelShader;	

		std::shared_ptr<Shaders> m_VertexShaderAsset;
		std::shared_ptr<Shaders> m_PixelShaderAsset;

		Microsoft::WRL::ComPtr<ID3D11SamplerState> m_SamplerState;	
	};
}
