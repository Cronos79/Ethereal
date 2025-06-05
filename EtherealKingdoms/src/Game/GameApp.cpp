#include "GameApp.h"
#include "Core/EELoggerMacros.h"
#include "imgui.h"
#include "Game/EEScene.h"
#include "Core/EEContext.h"
#include "TestObject.h"

bool GameApp::Initialize()
{
	LOG_INFO("GameApp: Init");
	EtherealEngine::EEContext::Get().GetAssetManager()->LoadModel("test_model");
	EtherealEngine::EEContext::Get().GetAssetManager()->LoadVertexShader("test_vs");
	EtherealEngine::EEContext::Get().GetAssetManager()->LoadPixelShader("test_ps");

	auto obj = std::make_shared<TestObject>();
	m_CurrentScene->AddGameObject(obj);
	return true;
}

void GameApp::Update(float deltaTime)
{
	
}

void GameApp::DrawUI()
{
	float aspect = static_cast<float>(EtherealEngine::EEContext::Get().GetWindowWidth()) / EtherealEngine::EEContext::Get().GetWindowHeight();
	float nearZ = 0.1f;
	float farZ = 10000.0f;
	auto& camera = EtherealEngine::EEContext::Get().GetRenderer()->GetCamera();

	static float eye[3] = { camera.GetEye().x, camera.GetEye().y, camera.GetEye().z };
	static float at[3] = { camera.GetAt().x,  camera.GetAt().y,  camera.GetAt().z };
	static float up[3] = { camera.GetUp().x,  camera.GetUp().y,  camera.GetUp().z };
	static float fov = camera.GetFovY();
	static float yaw = camera.GetYaw();
	static float pitch = camera.GetPitch();
	static float moveStep = 10.0f; // Movement step size

	ImGui::Begin("Camera Controls");
	bool changed = false;
	changed |= ImGui::InputFloat3("Eye (Camera Pos)", eye);
	changed |= ImGui::InputFloat3("At (Look At)", at);
	changed |= ImGui::InputFloat3("Up", up);
	changed |= ImGui::SliderFloat("FOV", &fov, 10.0f, 120.0f);
	ImGui::SliderFloat("Move Step", &moveStep, 0.1f, 100.0f, "%.2f");

	// Yaw/Pitch controls
	if (ImGui::SliderAngle("Yaw", &yaw, -180.0f, 180.0f) |
		ImGui::SliderAngle("Pitch", &pitch, -89.0f, 89.0f))
	{
		camera.SetYawPitch(yaw, pitch);
	}

	// FPS-style movement controls
	if (ImGui::Button("Forward (W)"))
	{
		// Move along forward vector
		float yawRad = yaw;
		float pitchRad = pitch;
		eye[0] += moveStep * cosf(pitchRad) * sinf(yawRad);
		eye[1] += moveStep * sinf(pitchRad);
		eye[2] += moveStep * cosf(pitchRad) * cosf(yawRad);
		at[0] += moveStep * cosf(pitchRad) * sinf(yawRad);
		at[1] += moveStep * sinf(pitchRad);
		at[2] += moveStep * cosf(pitchRad) * cosf(yawRad);
		changed = true;
	}
	ImGui::SameLine();
	if (ImGui::Button("Backward (S)"))
	{
		float yawRad = yaw;
		float pitchRad = pitch;
		eye[0] -= moveStep * cosf(pitchRad) * sinf(yawRad);
		eye[1] -= moveStep * sinf(pitchRad);
		eye[2] -= moveStep * cosf(pitchRad) * cosf(yawRad);
		at[0] -= moveStep * cosf(pitchRad) * sinf(yawRad);
		at[1] -= moveStep * sinf(pitchRad);
		at[2] -= moveStep * cosf(pitchRad) * cosf(yawRad);
		changed = true;
	}
	if (ImGui::Button("Left (A)"))
	{
		// Move along right vector (negative)
		float rightYaw = yaw - DirectX::XM_PIDIV2;
		eye[0] += moveStep * sinf(rightYaw);
		eye[2] += moveStep * cosf(rightYaw);
		at[0] += moveStep * sinf(rightYaw);
		at[2] += moveStep * cosf(rightYaw);
		changed = true;
	}
	ImGui::SameLine();
	if (ImGui::Button("Right (D)"))
	{
		float rightYaw = yaw + DirectX::XM_PIDIV2;
		eye[0] += moveStep * sinf(rightYaw);
		eye[2] += moveStep * cosf(rightYaw);
		at[0] += moveStep * sinf(rightYaw);
		at[2] += moveStep * cosf(rightYaw);
		changed = true;
	}

	if (ImGui::Button("Reset Camera"))
	{
		eye[0] = 0; eye[1] = 0; eye[2] = -100;
		at[0] = 0; at[1] = 0; at[2] = 0;
		up[0] = 0; up[1] = 1; up[2] = 0;
		fov = 0.785f; // ~45 deg
		camera.SetLookAt({ eye[0], eye[1], eye[2] }, { at[0], at[1], at[2] }, { up[0], up[1], up[2] });
		camera.SetPerspective(fov, aspect, nearZ, farZ);
	}

	if (changed)
	{
		camera.SetLookAt({ eye[0], eye[1], eye[2] }, { at[0], at[1], at[2] }, { up[0], up[1], up[2] });
		camera.SetPerspective(fov, aspect, nearZ, farZ);
	}
	ImGui::End();
}

void GameApp::Shutdown()
{
	LOG_INFO("GameApp: Shutdown");
}