#include "Assets/GameObject.h"
#include "Core/Logger.h"
#include "Core/EEContext.h"
#include "imgui/imgui.h"

namespace Ethereal
{
	int32_t GameObject::s_NextID = 0;

	GameObject::GameObject()
	{
		m_ID = s_NextID++;
		m_Name = "GameObject" + std::to_string(m_ID);		
	}

	GameObject::GameObject(const std::string& name)
	{
		m_ID = s_NextID++;
		m_Name = name + std::to_string(m_ID);			
	}

	GameObject::~GameObject()
	{

	}

	void GameObject::DrawUI(float deltaTime)
	{
		ImGui::Begin("Info");
		ImGui::Text("Name: %s", m_Name.c_str());
		ImGui::Text("ID: %d", m_ID);
		// Display position, rotation, scale
		ImGui::Text("Position: (%.2f, %.2f, %.2f)", m_Transform.GetPosition().x, m_Transform.GetPosition().y, m_Transform.GetPosition().z);
		ImGui::Text("Rotation: (%.2f, %.2f, %.2f)", m_Transform.GetRotation().x, m_Transform.GetRotation().y, m_Transform.GetRotation().z);
		ImGui::Text("Scale: (%.2f, %.2f, %.2f)", m_Transform.GetScale().x, m_Transform.GetScale().y, m_Transform.GetScale().z);
		ImGui::End();
	}

	void GameObject::Update(float deltaTime)
	{
		LOG_INFO("Updating GameObject: %s", m_Name.c_str());
	}

	void GameObject::PickNewTarget()
	{
		m_TargetPosition = DirectX::XMFLOAT3(
			static_cast<float>(rand() % 2000 - 1000),
			static_cast<float>(rand() % 2000 - 1000),
			static_cast<float>(rand() % 2000 - 1000)
		);
		// Set random move speed between 10 and 150 units per second
		m_Speed = static_cast<float>(rand() % 140 + 10);
	}

	void GameObject::PickNewRandomRotation()
	{
		m_RndRotation = DirectX::XMFLOAT3(
			static_cast<float>(rand() % 360),
			static_cast<float>(rand() % 360),
			static_cast<float>(rand() % 360)
		);
		// Set Random rotation speed between .01 and .5 per second
		m_RotationSpeed = static_cast<float>((rand() % 50 + 1)) / 100.0f;
	}

	void GameObject::MoveTo(const DirectX::XMFLOAT3& targetPosition)
	{
		m_TargetPosition = targetPosition;
		m_CanMove = true;
	}

	void GameObject::MoveTo(float x, float y, float z)
	{
		MoveTo(DirectX::XMFLOAT3(x, y, z));
	}

	void GameObject::Move(float deltaTime)
	{
		if (!m_CanMove)
			return;

		m_IsMoving = true;
		using namespace DirectX;
		XMFLOAT3 currentPos = m_Transform.GetPosition();
		XMVECTOR curr = XMLoadFloat3(&currentPos);
		XMVECTOR target = XMLoadFloat3(&m_TargetPosition);
		XMVECTOR toTarget = XMVectorSubtract(target, curr);
		float distance = XMVectorGetX(XMVector3Length(toTarget));

		if (distance < 1.0f) // Close enough, pick a new target
		{
			PickNewTarget();
		}
		else
		{
			XMVECTOR direction = XMVector3Normalize(toTarget);
			XMVECTOR move = XMVectorScale(direction, m_Speed * deltaTime);
			XMFLOAT3 moveDelta;
			XMStoreFloat3(&moveDelta, move);
			AddPosition(moveDelta.x, moveDelta.y, moveDelta.z);
		}
	}

	void GameObject::StopMoving()
	{
		m_CanMove = false;
		m_IsMoving = false;
		XMFLOAT3 currentPos = m_Transform.GetPosition();
		m_TargetPosition = currentPos; // Set target to current position to stop movement logic
	}

}