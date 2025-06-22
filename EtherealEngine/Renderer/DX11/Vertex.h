#pragma once
#include <DirectXMath.h>

struct Vertex
{
	Vertex() { }
	Vertex(float x, float y, float z, float nx, float ny, float nz, float u, float v)
		: pos(x, y, z), normal(nx, ny, nz), texCoord(u, v)
	{
	}
	DirectX::XMFLOAT3 pos; // Position in 2D space
	DirectX::XMFLOAT3 normal; // Normal vector for lighting calculations
	DirectX::XMFLOAT2 texCoord;
};