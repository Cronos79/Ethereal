#pragma once
#include "Core/EtherealIncludes.h"
#include "Renderer/Renderer.h"
#include "GameObject.h"

namespace Ethereal
{
	class ETHEREAL_API RendererDX11 : public Renderer
	{
	public:
		RendererDX11();
		~RendererDX11();

		void Initialize() override;
		void BeginFrame() override;
		void Draw(GameObject obj) override;
		void EndFrame() override;
		void Shutdown() override;
	};
}
