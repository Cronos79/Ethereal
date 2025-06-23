#include "Renderer/DX11/Camera.h"
#include "CameraManager.h"
#include "Core/EEContext.h"

namespace Ethereal
{
	CameraManager::CameraManager()
	{
		NewCamera(45.0f, 0.1f, 5000.0f); // Create a new camera with default parameters
	}
	CameraManager::~CameraManager()
	{
		for (auto camera : m_Cameras)
		{
			delete camera; // Clean up allocated cameras
		}
		m_Cameras.clear(); // Clear the list of cameras
	}

	void CameraManager::AddCamera(Camera* camera)
	{
		m_Cameras.push_back(camera);
		if (m_Cameras.size() == 1)
		{
			m_CurrentCamera = camera; // Set the first camera as the current camera
		}
	}

	void CameraManager::RemoveCamera(Camera* camera)
	{
		auto it = std::find(m_Cameras.begin(), m_Cameras.end(), camera);
		if (it != m_Cameras.end())
		{
			if (m_CurrentCamera == camera)
			{
				m_CurrentCamera = nullptr; // Clear current camera if it is being removed
			}
			delete camera; // Clean up allocated camera
			m_Cameras.erase(it); // Remove camera from the list
		}
	}

	void CameraManager::NewCamera(float fov, float nearPlane, float farPlane)
	{
		Camera* newCamera = new Camera();
		newCamera->SetPosition(0.0f, 0.0f, -800.0f); // Set camera position
		newCamera->SetProjectionValues(fov, (float)EEContext::Get().GetWidth() / (float)EEContext::Get().GetHeight(), nearPlane, farPlane); // Set projection values
		
		AddCamera(newCamera); // Add the new camera to the manager
	}

}