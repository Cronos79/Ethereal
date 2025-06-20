#pragma once
#include "Core/EtherealIncludes.h"
#include "Camera.h"
#include <vector>

namespace Ethereal
{
	class ETHEREAL_API CameraManager
	{
	public:
		CameraManager();
		virtual ~CameraManager();

		Camera& GetCurrentCamera()
		{
			return *m_CurrentCamera;
		}

		void AddCamera(Camera* camera);
		void RemoveCamera(Camera* camera);

		void NewCamera(float fov, float nearPlane, float farPlane);


	private:
		Camera* m_CurrentCamera;
		std::vector<Camera*> m_Cameras;

	};
}
