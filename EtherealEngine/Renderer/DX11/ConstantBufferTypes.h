#pragma once
#include <DirectXMath.h>

struct CB_VS_PerObject
{
	DirectX::XMMATRIX worldViewProj;
	DirectX::XMMATRIX world;
};

struct CB_PS_pixelshader
{
	float alpha = 1.0f;
};

struct CB_PS_Light
{
	DirectX::XMFLOAT3 lightDirection = { 0.0f, -1.0f, -1.0f };
	float padding = 0.0f; // Padding to 16-byte alignment
	DirectX::XMFLOAT3 lightColor = { 1.0f, 1.0f, 1.0f };
	float ambientStrength = 0.2f;
};

struct CB_PS_Camera
{
	DirectX::XMMATRIX viewProj; // View * Projection matrix (transposed)
	DirectX::XMFLOAT3 cameraPosition;
	float padding; // maintain 16-byte alignment
	DirectX::XMFLOAT3 viewDirection;  // Forward vector from camera
	float padding1;                   // Padding again for alignment
};