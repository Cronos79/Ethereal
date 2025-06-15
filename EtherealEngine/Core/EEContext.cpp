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
		// Load config files with window title, size and other settings
		
		// Initialize the unique pointers
		m_Window = std::make_unique<EEWindows>();
		m_Window->Initialize();

		LOG_INFO("Ethereal Engine Context Initialized");
	}

} // namespace Ethereal