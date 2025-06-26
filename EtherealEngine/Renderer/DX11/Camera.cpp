#include "Renderer/DX11/Camera.h"
#include "imgui/imgui.h"
#include "Core/EEContext.h"

namespace Ethereal
{
	Camera::Camera()
	{
		m_FovDegrees = 45.0f; // Default FOV
		m_NearPlane = 0.1f; // Default near plane distance
		m_FarPlane = 1000.0f; // Default far plane distance
		this->pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		this->posVector = XMLoadFloat3(&this->pos);
		this->rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		this->rotVector = XMLoadFloat3(&this->rot);
		this->UpdateViewMatrix();
		m_CameraBuffer = ConstantBuffer<CB_Camera>();
		if (!InitializeBuffer())
		{
			LOG_ERROR("Failed to initialize camera constant buffer.");
		}
	}

	void Camera::SetProjectionValues(float fovDegrees, float aspectRatio, float nearZ, float farZ)
	{
		float fovRadians = (fovDegrees / 360.0f) * XM_2PI;
		this->projectionMatrix = XMMatrixPerspectiveFovLH(fovRadians, aspectRatio, nearZ, farZ);
	}

	const XMMATRIX& Camera::GetViewMatrix() const
	{
		return this->viewMatrix;
	}

	const XMMATRIX& Camera::GetProjectionMatrix() const
	{
		return this->projectionMatrix;
	}

	const XMVECTOR& Camera::GetPositionVector() const
	{
		return this->posVector;
	}

	const XMFLOAT3& Camera::GetPositionFloat3() const
	{
		return this->pos;
	}

	const XMVECTOR& Camera::GetRotationVector() const
	{
		return this->rotVector;
	}

	const XMFLOAT3& Camera::GetRotationFloat3() const
	{
		return this->rot;
	}

	void Camera::SetPosition(const XMVECTOR& pos)
	{
		XMStoreFloat3(&this->pos, pos);
		this->posVector = pos;
		this->UpdateViewMatrix();
	}

	void Camera::SetPosition(float x, float y, float z)
	{
		this->pos = XMFLOAT3(x, y, z);
		this->posVector = XMLoadFloat3(&this->pos);
		this->UpdateViewMatrix();
	}

	void Camera::AdjustPosition(const XMVECTOR& pos)
	{
		this->posVector += pos;
		XMStoreFloat3(&this->pos, this->posVector);
		this->UpdateViewMatrix();
	}

	void Camera::AdjustPosition(float x, float y, float z)
	{
		this->pos.x += x;
		this->pos.y += y;
		this->pos.z += z;
		this->posVector = XMLoadFloat3(&this->pos);
		this->UpdateViewMatrix();
	}

	void Camera::SetRotation(const XMVECTOR& rot)
	{
		this->rotVector = rot;
		XMStoreFloat3(&this->rot, rot);
		this->UpdateViewMatrix();
	}

	void Camera::SetRotation(float x, float y, float z)
	{
		this->rot = XMFLOAT3(x, y, z);
		this->rotVector = XMLoadFloat3(&this->rot);
		this->UpdateViewMatrix();
	}

	void Camera::AdjustRotation(const XMVECTOR& rot)
	{
		this->rotVector += rot;
		XMStoreFloat3(&this->rot, this->rotVector);
		this->UpdateViewMatrix();
	}

	void Camera::AdjustRotation(float x, float y, float z)
	{
		this->rot.x += x;
		this->rot.y += y;
		this->rot.z += z;
		this->rotVector = XMLoadFloat3(&this->rot);
		this->UpdateViewMatrix();
	}

	void Camera::SetLookAtPos(XMFLOAT3 lookAtPos)
	{
		//Verify that look at pos is not the same as cam pos. They cannot be the same as that wouldn't make sense and would result in undefined behavior.
		if (lookAtPos.x == this->pos.x && lookAtPos.y == this->pos.y && lookAtPos.z == this->pos.z)
			return;

		lookAtPos.x = this->pos.x - lookAtPos.x;
		lookAtPos.y = this->pos.y - lookAtPos.y;
		lookAtPos.z = this->pos.z - lookAtPos.z;

		float pitch = 0.0f;
		if (lookAtPos.y != 0.0f)
		{
			const float distance = sqrt(lookAtPos.x * lookAtPos.x + lookAtPos.z * lookAtPos.z);
			pitch = atan(lookAtPos.y / distance);
		}

		float yaw = 0.0f;
		if (lookAtPos.x != 0.0f)
		{
			yaw = atan(lookAtPos.x / lookAtPos.z);
		}
		if (lookAtPos.z > 0)
			yaw += XM_PI;

		this->SetRotation(pitch, yaw, 0.0f);
	}

	const XMVECTOR& Camera::GetForwardVector()
	{
		return this->vec_forward;
	}

	const XMVECTOR& Camera::GetRightVector()
	{
		return this->vec_right;
	}

	const XMVECTOR& Camera::GetBackwardVector()
	{
		return this->vec_backward;
	}

	const XMVECTOR& Camera::GetLeftVector()
	{
		return this->vec_left;
	}

	void Camera::MoveForward(float amount)
	{
		// Move along the current forward vector
		XMVECTOR forward = XMVector3Normalize(this->GetForwardVector());
		XMVECTOR movement = XMVectorScale(forward, amount);
		this->AdjustPosition(movement);
	}

	void Camera::MoveBackward(float amount)
	{
		// Move along the current backward vector
		XMVECTOR backward = XMVector3Normalize(this->GetBackwardVector());
		XMVECTOR movement = XMVectorScale(backward, amount);
		this->AdjustPosition(movement);
	}

	void Camera::MoveLeft(float amount)
	{
		// Move along the current left vector
		XMVECTOR left = XMVector3Normalize(this->GetLeftVector());
		XMVECTOR movement = XMVectorScale(left, amount);
		this->AdjustPosition(movement);
	}

	void Camera::MoveRight(float amount)
	{
		// Move along the current right vector
		XMVECTOR right = XMVector3Normalize(this->GetRightVector());
		XMVECTOR movement = XMVectorScale(right, amount);
		this->AdjustPosition(movement);
	}

	void Camera::MoveUp(float amount)
	{
		// Move along the current up vector
		XMVECTOR up = XMVector3TransformCoord(this->DEFAULT_UP_VECTOR, XMMatrixRotationRollPitchYaw(0.0f, this->rot.y, 0.0f));
		XMVECTOR movement = XMVectorScale(up, amount);
		this->AdjustPosition(movement);
	}

	void Camera::MoveDown(float amount)
	{
		// Move along the current down vector
		XMVECTOR down = XMVector3TransformCoord(this->DEFAULT_DOWN_VECTOR, XMMatrixRotationRollPitchYaw(0.0f, this->rot.y, 0.0f));
		XMVECTOR movement = XMVectorScale(down, amount);
		this->AdjustPosition(movement);
	}

	void Camera::HandleInput(Mouse& mouse, Keyboard& keyboard, float deltaTime, float cameraSpeed, float mouseSensitivity)
	{
		// Handle keyboard input for camera movement
		if (keyboard.IsKeyDown('W'))
		{
			MoveForward(0.1f * cameraSpeed);
		}
		if (keyboard.IsKeyDown('S'))
		{
			MoveBackward(0.1f * cameraSpeed);
		}
		if (keyboard.IsKeyDown('A'))
		{
			MoveLeft(0.1f * cameraSpeed);
		}
		if (keyboard.IsKeyDown('D'))
		{
			MoveRight(0.1f * cameraSpeed);
		}
		if (keyboard.IsKeyDown('Q'))
		{
			MoveUp(0.1f * cameraSpeed);
		}
		if (keyboard.IsKeyDown('E'))
		{
			MoveDown(0.1f * cameraSpeed);
		}

		// Handle mouse movement for camera rotation
		while (auto rawDelta = mouse.ReadRawDelta())
		{

			auto rot = GetRotationFloat3();
			float newPitch = rot.x + static_cast<float>(rawDelta->y) * mouseSensitivity;
			float newYaw = rot.y + static_cast<float>(rawDelta->x) * mouseSensitivity;
			SetRotation(newPitch, newYaw, 0.0f);
		}
	}

	void Camera::UIControls()
	{
		ImGui::Begin("Camera Controls");
		ImGui::Text("Use WASDQE to move the camera.");
		ImGui::Text("Use mouse to look around.");
		ImGui::SliderFloat("FOV", &m_FovDegrees, 1.0f, 120.0f);
		ImGui::SliderFloat("Near Plane", &m_NearPlane, 0.01f, 10.0f);
		ImGui::SliderFloat("Far Plane", &m_FarPlane, 100.0f, 10000.0f);
		// Set position
		if (ImGui::InputFloat3("Position", &this->pos.x))
		{
			this->SetPosition(this->pos.x, this->pos.y, this->pos.z);
		}
		ImGui::End();
	}

	float Camera::GetFovDegrees() const
	{
		return m_FovDegrees;
	}

	float Camera::GetNearPlane() const
	{
		return m_NearPlane;
	}

	float Camera::GetFarPlane() const
	{
		return m_FarPlane;
	}

	ConstantBuffer<CB_Camera>& Camera::GetConstantBuffer()
	{
		return m_CameraBuffer;
	}

	bool Camera::InitializeBuffer()
	{
		ID3D11Device* device = static_cast<ID3D11Device*>(EEContext::Get().GetDevice());
		ID3D11DeviceContext* context = static_cast<ID3D11DeviceContext*>(EEContext::Get().GetContext());
		HRESULT hr = m_CameraBuffer.Initialize(device, context);
		if (FAILED(hr))
		{
			LOG_ERROR("Failed to initialize camera constant buffer: {}", hr);
			return false;
		}
		return true;
	}

	void Camera::UpdateBuffer()
	{
		m_CameraBuffer.data.view = DirectX::XMMatrixTranspose(this->viewMatrix);
		m_CameraBuffer.data.projection = DirectX::XMMatrixTranspose(this->projectionMatrix);
		m_CameraBuffer.ApplyChanges();
	}

	void Camera::UpdateViewMatrix() //Updates view matrix and also updates the movement vectors
	{
		//Calculate camera rotation matrix
		XMMATRIX camRotationMatrix = XMMatrixRotationRollPitchYaw(this->rot.x, this->rot.y, this->rot.z);
		//Calculate unit vector of cam target based off camera forward value transformed by cam rotation matrix
		XMVECTOR camTarget = XMVector3TransformCoord(this->DEFAULT_FORWARD_VECTOR, camRotationMatrix);
		//Adjust cam target to be offset by the camera's current position
		camTarget += this->posVector;
		//Calculate up direction based on current rotation
		XMVECTOR upDir = XMVector3TransformCoord(this->DEFAULT_UP_VECTOR, camRotationMatrix);
		//Rebuild view matrix
		this->viewMatrix = XMMatrixLookAtLH(this->posVector, camTarget, upDir);

		XMMATRIX vecRotationMatrix = XMMatrixRotationRollPitchYaw(0.0f, this->rot.y, 0.0f);
		this->vec_forward = XMVector3TransformCoord(this->DEFAULT_FORWARD_VECTOR, vecRotationMatrix);
		this->vec_backward = XMVector3TransformCoord(this->DEFAULT_BACKWARD_VECTOR, vecRotationMatrix);
		this->vec_left = XMVector3TransformCoord(this->DEFAULT_LEFT_VECTOR, vecRotationMatrix);
		this->vec_right = XMVector3TransformCoord(this->DEFAULT_RIGHT_VECTOR, vecRotationMatrix);
	}
} // namespace Ethereal