#include "Core/EELoggerMacros.h"
#include "Core/EEContext.h"

int main()
{
	EtherealEngine::EEContext::Get().Initialize();
	LOG_INFO("Ethereal Engine started");
	return 0;
}