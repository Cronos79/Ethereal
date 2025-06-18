#pragma once
#include <DirectXMath.h>

struct Vertex
{
	Vertex() { }
	Vertex(float x, float y, float r, float g, float b)
		: pos(x, y), color(r,g,b)
	{

	}
	DirectX::XMFLOAT2 pos; // Position in 2D space
	DirectX::XMFLOAT3 color;
};