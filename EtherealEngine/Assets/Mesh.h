#pragma once
#include "Core/EtherealIncludes.h"
#include "Core/IAsset.h"
#include <wrl/client.h>
#include <d3d11.h>
#include <memory>
#include "Shaders.h"
#include "Renderer/DX11/Vertex.h"

namespace Ethereal
{
	class ETHEREAL_API Mesh : public IAsset
	{
	public:
		Mesh();
		~Mesh();
		virtual bool Load(const std::string& path) override;
	};
}