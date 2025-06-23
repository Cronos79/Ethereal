#pragma once
#include <memory>
#include <stdint.h>
#include "Model.h"
#include "Transform.h"
#include "Assets/IAsset.h"
#include "Core/EtherealIncludes.h"

namespace Ethereal
{
	class ETHEREAL_API GameObject : public IAsset
	{
	public:
		GameObject();
		GameObject(const std::string& name);
		~GameObject();

		using GameObjectFactoryFunc = std::function<std::shared_ptr<GameObject>()>;

		static void RegisterType(const std::string& typeName, GameObjectFactoryFunc factory);
		static std::shared_ptr<GameObject> CreateByType(const std::string& typeName);

		virtual std::string GetTypeName()
		{
			return m_TypeName;
		}

		virtual std::shared_ptr<GameObject> Clone() const;

		virtual void DrawUI(float deltaTime);
		virtual void Update(float deltaTime);

		void PickNewTarget();
		void PickNewRandomRotation();

		void MoveTo(const DirectX::XMFLOAT3& targetPosition);
		void MoveTo(float x, float y, float z);
		
		void Move(float deltaTime, float moveSpeed, float rotatationSpeed);
		void StopMoving();
		
		// Assessors
		bool IsVisible() const
		{
			return m_IsVisible;
		}
		void SetVisible(bool visible)
		{
			m_IsVisible = visible;
		}
		const std::string& GetName() const
		{
			return m_Name;
		}
		void SetName(const std::string& name)
		{
			m_Name = name;
		}
		int32_t GetID() const
		{
			return m_ID;
		}
		void NewID()
		{
			m_ID = s_NextID++;
		}
		std::shared_ptr<Model> GetModelPtr()
		{
			return m_Model;
		}
		Model& GetModel()
		{
			return *m_Model;
		}
		void SetModel(const std::shared_ptr<Model>& model)
		{
			m_Model = model;
		}

		bool operator==(const GameObject& other) const
		{
			// Compare by unique ID, name, or whatever makes sense for your game objects
			return this->GetID() == other.GetID();
		}
		Transform& GetTransform()
		{
			return m_Transform;
		}
		const Transform& GetTransform() const
		{
			return m_Transform;
		}
		void SetTransform(const Transform& transform)
		{
			m_Transform = transform;
		}
		void SetTransform(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& rotation, const DirectX::XMFLOAT3& scale)
		{
			m_Transform.SetPosition(position);
			m_Transform.SetRotation(rotation);
			m_Transform.SetScale(scale);
		}
		void SetPosition(const DirectX::XMFLOAT3& position)
		{
			m_Transform.SetPosition(position);
		}
		void SetPosition(float x, float y, float z)
		{
			m_Transform.SetPosition(DirectX::XMFLOAT3(x, y, z));
		}
		void AddPosition(const DirectX::XMFLOAT3& deltaPosition)
		{
			m_Transform.AddPosition(deltaPosition);
		}
		void AddPosition(float deltaX, float deltaY, float deltaZ)
		{
			m_Transform.AddPosition(DirectX::XMFLOAT3(deltaX, deltaY, deltaZ));
		}
		void SetRotation(const DirectX::XMFLOAT3& rotation)
		{
			m_Transform.SetRotation(rotation);
		}
		void SetRotation(float pitch, float yaw, float roll)
		{
			m_Transform.SetRotation(DirectX::XMFLOAT3(pitch, yaw, roll));
		}
		void AddRotation(const DirectX::XMFLOAT3& deltaRotation)
		{
			m_Transform.AddRotation(deltaRotation);
		}
		void AddRotation(float deltaPitch, float deltaYaw, float deltaRoll)
		{
			m_Transform.AddRotation(DirectX::XMFLOAT3(deltaPitch, deltaYaw, deltaRoll));
		}
		void SetScale(const DirectX::XMFLOAT3& scale)
		{
			m_Transform.SetScale(scale);
		}
		void SetScale(float scaleX, float scaleY, float scaleZ)
		{
			m_Transform.SetScale(DirectX::XMFLOAT3(scaleX, scaleY, scaleZ));
		}
		void SetScale(float uniformScale)
		{
			m_Transform.SetScale(DirectX::XMFLOAT3(uniformScale, uniformScale, uniformScale));
		}
		void AddScale(const DirectX::XMFLOAT3& deltaScale)
		{
			m_Transform.AddScale(deltaScale);
		}
		void AddScale(float deltaX, float deltaY, float deltaZ)
		{
			m_Transform.AddScale(DirectX::XMFLOAT3(deltaX, deltaY, deltaZ));
		}


		void SetTypeName(const std::string& typeName)
		{
			m_TypeName = typeName;
		}
		const std::string& GetTypeName() const
		{
			return m_TypeName;
		}
	protected:
		DirectX::XMFLOAT3 m_TargetPosition;
		DirectX::XMFLOAT3 m_RndRotation;
		float m_Speed = 10.0f; // Units per second
		float m_RotationSpeed = 0.01f; 
		bool m_IsMoving = false;
		bool m_CanMove = true;	
		std::shared_ptr<Model> m_Model;
		bool m_IsVisible = true;
		std::string m_Name;
		int32_t m_ID = 0;
		static int32_t s_NextID;
		Transform m_Transform;
		std::string m_TypeName = "GameObject";
	};
}