#pragma once
#include <DirectXMath.h>

struct Vertex
{
	Vertex() { }
	Vertex(float x, float y)
		: POSITION(x, y)
	{

	}
	DirectX::XMFLOAT2 POSITION; // Position in 2D space
};