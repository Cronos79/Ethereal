#include "Platform/EEWindows.h"
#include "Core/Logger.h"


namespace Ethereal
{

	EEWindows::EEWindows()
	{
		LOG_INFO("Windows platform initialized.");
	}

	EEWindows::~EEWindows()
	{
		LOG_INFO("Windows platform destroyed.");
	}

}