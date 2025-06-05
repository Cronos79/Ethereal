#include "pch.h"
#include "Camera.h"
using namespace DirectX;

namespace EtherealEngine
{
	Camera::Camera()
		: m_eye(0, 0, -5), m_at(0, 0, 0), m_up(0, 1, 0),
		m_fovY(XM_PIDIV4), m_aspect(16.0f / 9.0f), m_nearZ(0.1f), m_farZ(1000.0f)
	{
	}

	void Camera::SetPerspective(float fovY, float aspect, float nearZ, float farZ)
	{
		m_fovY = fovY; m_aspect = aspect; m_nearZ = nearZ; m_farZ = farZ;
	}
	void Camera::SetLookAt(const XMFLOAT3& eye, const XMFLOAT3& at, const XMFLOAT3& up)
	{
		m_eye = eye; m_at = at; m_up = up;
	}

	void Camera::SetZoom(float distance)
	{
		using namespace DirectX;
		XMVECTOR atV = XMLoadFloat3(&m_at);
		XMVECTOR eyeV = XMLoadFloat3(&m_eye);
		XMVECTOR dir = eyeV - atV;
		float len = XMVectorGetX(XMVector3Length(dir));
		if (len < 1e-4f)
		{
			// Default direction if degenerate
			dir = XMVectorSet(0, 0, -1, 0);
		}
		else
		{
			dir = XMVector3Normalize(dir);
		}
		XMVECTOR newEye = atV + dir * distance;
		XMStoreFloat3(&m_eye, newEye);
	}


	float Camera::GetZoom()
	{
		using namespace DirectX;
		XMVECTOR atV = XMLoadFloat3(&m_at);
		XMVECTOR eyeV = XMLoadFloat3(&m_eye);
		XMVECTOR diff = eyeV - atV;
		return XMVectorGetX(XMVector3Length(diff));
	}


	void Camera::SetYawPitch(float yaw, float pitch)
	{
		m_yaw = yaw;
		m_pitch = pitch;
		// Calculate forward vector
		using namespace DirectX;
		XMVECTOR forward = XMVectorSet(
			cosf(m_pitch) * sinf(m_yaw),
			sinf(m_pitch),
			cosf(m_pitch) * cosf(m_yaw),
			0.0f
		);
		XMVECTOR eyeV = XMLoadFloat3(&m_eye);
		XMVECTOR atV = eyeV + forward;
		XMStoreFloat3(&m_at, atV);
		// Up is always (0,1,0) for FPS
		m_up = { 0, 1, 0 };
	}


	void Camera::Move(const DirectX::XMFLOAT3& delta)
	{
		using namespace DirectX;
		XMVECTOR move = XMLoadFloat3(&delta);
		XMVECTOR eyeV = XMLoadFloat3(&m_eye);
		XMVECTOR atV = XMLoadFloat3(&m_at);
		eyeV += move;
		atV += move;
		XMStoreFloat3(&m_eye, eyeV);
		XMStoreFloat3(&m_at, atV);
	}

	XMMATRIX Camera::GetView() const
	{
		return XMMatrixLookAtLH(XMLoadFloat3(&m_eye), XMLoadFloat3(&m_at), XMLoadFloat3(&m_up));
	}
	XMMATRIX Camera::GetProj() const
	{
		return XMMatrixPerspectiveFovLH(m_fovY, m_aspect, m_nearZ, m_farZ);
	}
	XMMATRIX Camera::GetViewProj() const
	{
		return XMMatrixMultiply(GetView(), GetProj());
	}
}