#pragma once
#include "Game/EEGameObject.h"
#include "imgui.h"
#include "Assets/VertexShaderAsset.h"
#include "Core/EEContext.h"
#include "Core/EELoggerMacros.h"

//Test EEGameObject
class
	TestObject : public EtherealEngine::EEGameObject
{
public:
	TestObject()
	{
		SetName("TestObj");	
		auto model = EtherealEngine::EEContext::Get().GetAssetManager()->Get<EtherealEngine::ModelAsset>("test_model");
		if (model)
		{
			LOG_INFO("GameApp: Model loaded successfully");
			SetModel(model);
			// Set shader names for all materials in the model
			for (auto& material : model->GetMaterials())
			{
				material.SetVertexShader("test_vs");
				material.SetPixelShader("test_ps");
			}
		}
		else
		{
			LOG_ERROR("GameApp: Failed to load model");
		}
		// Test code to make sure shader can be used
		auto shader = EtherealEngine::EEContext::Get().GetAssetManager()->Get<EtherealEngine::VertexShaderAsset>(model->GetMaterials().front().GetVertexShader());
		if (shader)
		{
			LOG_INFO("GameApp: Shader loaded successfully");
			//SetVertexShader(shader);
		}
		else
		{
			LOG_ERROR("GameApp: Failed to load shader");
		}
		LOG_INFO("TestObject: Created");
	}
	~TestObject()
	{
		//LOG_INFO("TestObject: Destroyed");
	}
	void DrawUI() override
	{
		ImGui::Begin("Game FPS");
		ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
		ImGui::End();
	}
};