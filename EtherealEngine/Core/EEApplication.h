#pragma once
#include "Core/EtherealIncludes.h"

namespace Ethereal
{
	class ETHEREAL_API EEApplication
	{
	public:
		EEApplication();
		~EEApplication();
		void Initialize();
		void Run();
		void Shutdown();

		virtual void OnInitialize() = 0;
		virtual void OnHandleInput() = 0;
		virtual void OnUpdate() = 0;
		virtual void OnShutdown() = 0;
	};



}
