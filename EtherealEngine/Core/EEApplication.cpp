#include "Core/EEApplication.h"
#include "EEContext.h"
#include "Logger.h"

namespace Ethereal
{
  EEApplication::EEApplication()
  {
	// Constructor implementation
  }
  EEApplication::~EEApplication()
  {
	// Destructor implementation
  }
  void EEApplication::Initialize()
  {
	  EEContext::Get().Initialize();
	  LOG_INFO("Ethereal Application initialized successfully.");
	  OnInitialize();
  }
  void EEApplication::Run()
  {
	  Initialize();
	  while (EEContext::Get().IsRunning())
	  {
		  if (!EEContext::Get().GetWindow().ProcessMessages())
		  {
			  EEContext::Get().SetRunning(false);
			  break;
		  }
		  OnHandleInput();
		  OnUpdate();
	  }
	  Shutdown();
  }
  void EEApplication::Shutdown()
  {
	  LOG_INFO("Shutting down Ethereal Application...");
	  OnShutdown();
  }
} // namespace Ethereal