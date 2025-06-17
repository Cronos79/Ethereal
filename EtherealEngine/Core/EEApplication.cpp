#include "Core/EEApplication.h"
#include "EEContext.h"
#include "Logger.h"
#include "Renderer/Renderer.h"

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
	  auto renderer = EEContext::Get().GetRenderer();
	  while (EEContext::Get().IsRunning())
	  {
		  if (!EEContext::Get().GetWindow().ProcessMessages())
		  {
			  EEContext::Get().SetRunning(false);
			  break;
		  }
		  renderer->BeginFrame();
		  OnHandleInput();
		  OnUpdate();
		  //renderer->Draw();
		  renderer->EndFrame();
	  }
	  renderer->Shutdown();
	  Shutdown();
  }
  void EEApplication::Shutdown()
  {
	  LOG_INFO("Shutting down Ethereal Application...");
	  OnShutdown();
  }
} // namespace Ethereal