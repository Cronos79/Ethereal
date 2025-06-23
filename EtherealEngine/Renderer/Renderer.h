#pragma once
#include "Core/EtherealIncludes.h"
#include "Assets/GameObject.h"
#include <memory>

namespace Ethereal
{
	class ETHEREAL_API Renderer
	{
	public:
		virtual void Initialize() = 0;
		virtual void BeginFrame() = 0;
		virtual void Draw(const std::shared_ptr<GameObject>& obj) = 0;
		virtual void EndFrame() = 0;
		virtual void Shutdown() = 0;

		virtual void* GetDevice() const = 0;
		virtual void* GetContext() const = 0;
	};
}