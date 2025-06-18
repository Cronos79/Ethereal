#include "Core/EEApplication.h"
#include "EEContext.h"
#include "Logger.h"
#include "Renderer/Renderer.h"
#include "Assets/GameObject.h"

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
	  OnInitialize();
  }
  void EEApplication::Run()
  {
	  CoInitialize(NULL);
	  Initialize();
	  auto renderer = EEContext::Get().GetRenderer();

	  GameObject* obj = new GameObject();
	  obj->Load("Hi");
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
		  renderer->Draw(*obj);
		  renderer->EndFrame();
	  }
	  renderer->Shutdown();
	  Shutdown();
  }
  void EEApplication::Shutdown()
  {
	  OnShutdown();
  }
} // namespace Ethereal