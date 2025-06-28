#pragma once
#include <DirectXMath.h>
#include "Core/EtherealIncludes.h"
#include "Platform/Mouse.h"
#include "Platform/Keyboard.h"
#include "ConstantBufferTypes.h"
#include "ConstantBuffer.h"

using namespace DirectX;

namespace Ethereal
{
	class ETHEREAL_API Camera
	{
	public:
		Camera();
		void SetProjectionValues(float fovDegrees, float aspectRatio, float nearZ, float farZ);

		const XMMATRIX& GetViewMatrix() const;
		const XMMATRIX& GetProjectionMatrix() const;

		const XMVECTOR& GetPositionVector() const;
		const XMFLOAT3& GetPositionFloat3() const;
		const XMVECTOR& GetRotationVector() const;
		const XMFLOAT3& GetRotationFloat3() const;

		void SetPosition(const XMVECTOR& pos);
		void SetPosition(float x, float y, float z);
		void AdjustPosition(const XMVECTOR& pos);
		void AdjustPosition(float x, float y, float z);
		void SetRotation(const XMVECTOR& rot);
		void SetRotation(float x, float y, float z);
		void AdjustRotation(const XMVECTOR& rot);
		void AdjustRotation(float x, float y, float z);
		void SetLookAtPos(XMFLOAT3 lookAtPos);
		const XMVECTOR& GetForwardVector();
		const XMVECTOR& GetRightVector();
		const XMVECTOR& GetBackwardVector();
		const XMVECTOR& GetLeftVector();

		void MoveForward(float amount);
		void MoveBackward(float amount);
		void MoveLeft(float amount);
		void MoveRight(float amount);
		void MoveUp(float amount);
		void MoveDown(float amount);
		void HandleInput(Mouse& mouse, Keyboard& keyboard, float deltaTime, float cameraSpeed, float mouseSensitivity);
		void UIControls();

		float GetFovDegrees() const;
		float GetNearPlane() const;
		float GetFarPlane() const;

		bool CreateConstantBuffer();
		void UpdateCB();
		ConstantBuffer<CB_PS_Camera>& GetCameraCB()
		{
			return m_CameraCB;
		}

	private:
		void UpdateViewMatrix();
		XMVECTOR posVector;
		XMVECTOR rotVector;
		XMFLOAT3 pos;
		XMFLOAT3 rot;
		XMMATRIX viewMatrix;
		XMMATRIX projectionMatrix;

		const XMVECTOR DEFAULT_FORWARD_VECTOR = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
		const XMVECTOR DEFAULT_UP_VECTOR = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
		const XMVECTOR DEFAULT_DOWN_VECTOR = XMVectorSet(0.0f, -1.0f, 0.0f, 0.0f);
		const XMVECTOR DEFAULT_BACKWARD_VECTOR = XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f);
		const XMVECTOR DEFAULT_LEFT_VECTOR = XMVectorSet(-1.0f, 0.0f, 0.0f, 0.0f);
		const XMVECTOR DEFAULT_RIGHT_VECTOR = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);

		XMVECTOR vec_forward;
		XMVECTOR vec_left;
		XMVECTOR vec_right;
		XMVECTOR vec_backward;

		float m_FovDegrees;
		float m_NearPlane;
		float m_FarPlane;

		ConstantBuffer<CB_PS_Camera> m_CameraCB;
		bool m_ShowControls = false;
	};
} // namespace Ethereal