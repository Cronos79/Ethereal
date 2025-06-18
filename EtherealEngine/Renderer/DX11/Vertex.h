#pragma once
#include <DirectXMath.h>

struct Vertex
{
	Vertex() { }
	Vertex(float x, float y, float z, float u, float v)
		: pos(x, y, z), texCoord(u,v)
	{

	}
	DirectX::XMFLOAT3 pos; // Position in 2D space
	DirectX::XMFLOAT2 texCoord;
};