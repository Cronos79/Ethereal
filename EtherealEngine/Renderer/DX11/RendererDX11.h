#pragma once
#include "Core/EtherealIncludes.h"
#include "Renderer/Renderer.h"
#include "Renderer/GameObject.h"
#include "Platform/EEWinIncludes.h"
#include <wrl/client.h>
#include "Renderer/DX11/AdapterReader.h"
#include <memory>

#include "Assets/Shaders.h"

namespace Ethereal
{
	class ETHEREAL_API RendererDX11 : public Renderer
	{
	public:
		RendererDX11();
		~RendererDX11();

		void Initialize() override;
		void InitializeShaders();
		void BeginFrame() override;
		void Draw(GameObject obj) override;
		void EndFrame() override;
		void Shutdown() override;

		// Assessors
		void* GetDevice() const override
		{
			return m_Device.Get();
		}
		ID3D11DeviceContext* GetContext() const
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
		ID3D11InputLayout* GetInputLayout() const
		{
			return m_InputLayout.Get();
		}


	private:
		Microsoft::WRL::ComPtr<ID3D11Device> m_Device;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_Context;
		Microsoft::WRL::ComPtr<IDXGISwapChain> m_SwapChain;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_RenderTargetView;

		Microsoft::WRL::ComPtr<ID3D11Texture2D> m_DepthStencilBuffer;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_DepthStencilView;


		Microsoft::WRL::ComPtr<ID3D11InputLayout> m_InputLayout;
		//Microsoft::WRL::ComPtr<ID3D11VertexShader> m_VertexShader;
		Microsoft::WRL::ComPtr<ID3D10Blob> m_VertexBuffer;
		Microsoft::WRL::ComPtr<ID3D11PixelShader> m_PixelShader;	

		std::shared_ptr<Shaders> m_VertexShaderAsset;
		std::shared_ptr<Shaders> m_PixelShaderAsset;
	};
}
