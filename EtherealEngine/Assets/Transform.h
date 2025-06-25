#pragma once
#include <DirectXMath.h>

namespace Ethereal
{
	struct Transform
	{
		Transform() = default;
		Transform(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& rotation, const DirectX::XMFLOAT3& scale)
			: m_Position(position), m_Rotation(rotation), m_Scale(scale)
		{
		}
		void SetPosition(const DirectX::XMFLOAT3& position)
		{
			m_Position = position;
		}
		void SetPosition(float x, float y, float z)
		{
			m_Position = { x, y, z };
		}
		void AddPosition(const DirectX::XMFLOAT3& deltaPosition)
		{
			m_Position.x += deltaPosition.x;
			m_Position.y += deltaPosition.y;
			m_Position.z += deltaPosition.z;
		}
		void SetRotation(const DirectX::XMFLOAT3& rotation)
		{
			m_Rotation = rotation;
		}
		void SetRotation(float pitch, float yaw, float roll)
		{
			m_Rotation = { pitch, yaw, roll };
		}
		void AddRotation(const DirectX::XMFLOAT3& deltaRotation)
		{
			m_Rotation.x += deltaRotation.x;
			m_Rotation.y += deltaRotation.y;
			m_Rotation.z += deltaRotation.z;
		}
		void SetScale(const DirectX::XMFLOAT3& scale)
		{
			m_Scale = scale;
		}
		void SetScale(float x, float y, float z)
		{
			m_Scale = { x, y, z };
		}
		void AddScale(const DirectX::XMFLOAT3& deltaScale)
		{
			m_Scale.x += deltaScale.x;
			m_Scale.y += deltaScale.y;
			m_Scale.z += deltaScale.z;
		}
		// Getters
		const DirectX::XMFLOAT3& GetPosition() const
		{
			return m_Position;
		}
		const DirectX::XMFLOAT3& GetRotation() const
		{
			return m_Rotation;
		}
		const DirectX::XMFLOAT3& GetScale() const
		{
			return m_Scale;
		}
		DirectX::XMMATRIX GetMatrix() const
		{
			using namespace DirectX;
			XMMATRIX translationMatrix = XMMatrixTranslation(m_Position.x, m_Position.y, m_Position.z);
			XMMATRIX rotationMatrix = XMMatrixRotationRollPitchYaw(XMConvertToRadians(m_Rotation.x), XMConvertToRadians(m_Rotation.y), XMConvertToRadians(m_Rotation.z));
			XMMATRIX scaleMatrix = XMMatrixScaling(m_Scale.x, m_Scale.y, m_Scale.z);
			return scaleMatrix * rotationMatrix * translationMatrix; // Scale, then rotate, then translate
		}
		DirectX::XMMATRIX GetRotationMatrix() const
		{
			using namespace DirectX;
			return XMMatrixRotationRollPitchYaw(
				XMConvertToRadians(m_Rotation.x),
				XMConvertToRadians(m_Rotation.y),
				XMConvertToRadians(m_Rotation.z)
			);
		}
	private:
		// Transform
		DirectX::XMFLOAT3 m_Position = { 0.0f, 0.0f, 0.0f };
		DirectX::XMFLOAT3 m_Rotation = { 0.0f, 0.0f, 0.0f }; // In degrees, XYZ (pitch, yaw, roll)
		DirectX::XMFLOAT3 m_Scale = { 1.0f, 1.0f, 1.0f };

	};
}