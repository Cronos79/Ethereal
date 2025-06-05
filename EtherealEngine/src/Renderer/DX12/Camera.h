#pragma once
#include <DirectXMath.h>

namespace EtherealEngine
{
	class Camera {
	public:
		Camera();
		void SetPerspective(float fovY, float aspect, float nearZ, float farZ);
		void SetLookAt(const DirectX::XMFLOAT3& eye, const DirectX::XMFLOAT3& at, const DirectX::XMFLOAT3& up);
		void SetZoom(float distance);
		float GetZoom();
		void SetYawPitch(float yaw, float pitch);
		void Move(const DirectX::XMFLOAT3& delta);

		DirectX::XMMATRIX GetView() const;
		DirectX::XMMATRIX GetProj() const;
		DirectX::XMMATRIX GetViewProj() const;

		const DirectX::XMFLOAT3& GetEye() const
		{
			return m_eye;
		}
		const DirectX::XMFLOAT3& GetAt() const
		{
			return m_at;
		}
		const DirectX::XMFLOAT3& GetUp() const
		{
			return m_up;
		}
		float GetFovY() const
		{
			return m_fovY;
		}
		
		float GetYaw() const
		{
			return m_yaw;
		}
		float GetPitch() const
		{
			return m_pitch;
		}
		

	private:
		DirectX::XMFLOAT3 m_eye, m_at, m_up;
		float m_fovY, m_aspect, m_nearZ, m_farZ;
		float m_yaw = 0.0f, m_pitch = 0.0f;
	};
}