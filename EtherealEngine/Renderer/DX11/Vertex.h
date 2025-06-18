#pragma once
#include <DirectXMath.h>

struct Vertex
{
	Vertex() { }
	Vertex(float x, float y, float z, float r, float g, float b)
		: pos(x, y, z), color(r,g,b)
	{

	}
	DirectX::XMFLOAT3 pos; // Position in 2D space
	DirectX::XMFLOAT3 color;
};