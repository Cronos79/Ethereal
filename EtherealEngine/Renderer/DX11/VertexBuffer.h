#pragma once
#include <d3d11.h>
#include <wrl/client.h>

class VertexBuffer
{
private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_Buffer;
	UINT m_Stride = 0;
	UINT m_VertexCount = 0;

public:
	VertexBuffer() = default;

	ID3D11Buffer* Get() const
	{
		return m_Buffer.Get();
	}
	ID3D11Buffer* const* GetAddressOf() const
	{
		return m_Buffer.GetAddressOf();
	}
	const UINT Stride() const
	{
		return m_Stride;
	}
	const UINT* StridePtr() const
	{
		return &m_Stride;
	}
	UINT VertexCount() const
	{
		return m_VertexCount;
	}

	HRESULT Initialize(ID3D11Device* device, const void* data, UINT totalSizeBytes, UINT stride)
	{
		if (m_Buffer) m_Buffer.Reset();

		m_Stride = stride;
		m_VertexCount = totalSizeBytes / stride;

		D3D11_BUFFER_DESC vbDesc = {};
		vbDesc.Usage = D3D11_USAGE_DEFAULT;
		vbDesc.ByteWidth = totalSizeBytes;
		vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA initData = {};
		initData.pSysMem = data;

		return device->CreateBuffer(&vbDesc, &initData, m_Buffer.GetAddressOf());
	}
};