#pragma once
#include <string>
#include <memory>
#include <DirectXMath.h>
#include "Assets/ModelAsset.h"

namespace EtherealEngine
{
	class EEGameObject
	{
	public:
		EEGameObject();
		virtual ~EEGameObject();

		// Basic transform (expand as needed)
		void SetPosition(const DirectX::XMFLOAT3& pos)
		{
			m_Position = pos;
		}
		const DirectX::XMFLOAT3& GetPosition() const
		{
			return m_Position;
		}

		// Optional: unique name or ID
		void SetName(const std::string& name)
		{
			m_Name = name;
		}
		const std::string& GetName() const
		{
			return m_Name;
		}

		// Model asset accessors
		void SetModel(const std::shared_ptr<ModelAsset>& model)
		{
			m_Model = model;
		}
		const std::shared_ptr<ModelAsset>& GetModel() const
		{
			return m_Model;
		}		

		// Update and draw stubs
		virtual void Update(float deltaTime);
		virtual void DrawUI();

	protected:
		std::string m_Name;
		DirectX::XMFLOAT3 m_Position{ 0.0f, 0.0f, 0.0f };
		std::shared_ptr<ModelAsset> m_Model;
	};
}