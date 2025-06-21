#include "Core/EEApplication.h"
#include "EEContext.h"
#include "Logger.h"
#include "Renderer/Renderer.h"
#include "Assets/GameObject.h"
#include "Core/EETimer.h"

namespace Ethereal
{
  EEApplication::EEApplication()
	  : m_SceneManager(EEContext::Get().GetSceneManager())
  {
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
	  CoInitialize(0);
	  Initialize();
	  auto renderer = EEContext::Get().GetRenderer();
	 
	  EETimer* timer = new EETimer();	

	  while (EEContext::Get().IsRunning())
	  {
		  timer->Tick();
		  if (!EEContext::Get().GetWindow().ProcessMessages())
		  {
			  EEContext::Get().SetRunning(false);
			  break;
		  }
		  OnHandleInput(timer->GetDeltaTime());
		  OnUpdate(timer->GetDeltaTime());	
		  m_SceneManager.GetCurrentScene()->Render(timer->GetDeltaTime());
	  }
	  renderer->Shutdown();
	  Shutdown();
  }
  void EEApplication::Shutdown()
  {
	  OnShutdown();
  }
} // namespace Ethereal