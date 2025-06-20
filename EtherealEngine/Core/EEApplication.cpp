#include "Core/EEApplication.h"
#include "EEContext.h"
#include "Logger.h"
#include "Renderer/Renderer.h"
#include "Assets/GameObject.h"
#include <chrono>

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
	  float deltaTime = 0.0f;
	  using clock = std::chrono::high_resolution_clock;
	  auto lastTime = clock::now();

#ifdef EDITOR_BUILD
	  m_Editor = std::make_unique<EditorGui>();
	  if (!m_Editor->Initialize())
	  {
		  LOG_ERROR("Failed to initialize editor GUI.");
		  return;
	  }
#endif

	  GameObject* obj = new GameObject();
	  obj->Load("Hi");
	  while (EEContext::Get().IsRunning())
	  {
		  auto currentTime = clock::now();
		  std::chrono::duration<float> elapsed = currentTime - lastTime;
		  deltaTime = elapsed.count();
		  lastTime = currentTime;

		  if (!EEContext::Get().GetWindow().ProcessMessages())
		  {
			  EEContext::Get().SetRunning(false);
			  break;
		  }
		  renderer->BeginFrame();
		  OnHandleInput(deltaTime);
		  OnUpdate(deltaTime);
		  renderer->Draw(*obj);
		  OnGui(deltaTime);
#ifdef EDITOR_BUILD
		  m_Editor->DrawUI(deltaTime);
#endif
		  renderer->EndFrame();
	  }
#ifdef EDITOR_BUILD
	  if (m_Editor)
	  {
		  m_Editor->Shutdown();
		  m_Editor.reset();
	  }
#endif
	  renderer->Shutdown();
	  Shutdown();
  }
  void EEApplication::Shutdown()
  {
	  OnShutdown();
  }
} // namespace Ethereal