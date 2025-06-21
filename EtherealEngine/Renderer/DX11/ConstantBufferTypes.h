#pragma once
#include <DirectXMath.h>

struct CB_VS_vertexshader
{
	DirectX::XMMATRIX mat; // World matrix for vertex shader
};

struct CB_PS_pixelshader
{
	float alpha = 1.0f;
};