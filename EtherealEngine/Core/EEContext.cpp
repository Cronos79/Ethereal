#include "Core/EEContext.h"
#include "Core/Logger.h"

namespace Ethereal
{

	EEContext::EEContext()
	{
	
	}

	EEContext::~EEContext()
	{
		m_Window.reset(); // Reset the unique pointer to release the window resources
		LOG_INFO("Ethereal Engine Context Destroyed");
	}

	void EEContext::Initialize()
	{
		// Initialize the Logger
		Logger::Init();
		// Initialize the unique pointers
		m_Window = std::make_unique<EEWindows>();

		LOG_INFO("Ethereal Engine Context Initialized");
	}

} // namespace Ethereal