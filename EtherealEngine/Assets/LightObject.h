#pragma once
#include "Assets/GameObject.h"
#include "Core/EtherealIncludes.h"
#include "AssetFactory.h"

namespace Ethereal
{
	class ETHEREAL_API LightObject : public GameObject
	{
	public:
		LightObject();
		~LightObject() override = default;

		std::string GetTypeName() override
		{
			return "LightObject";
		}

		std::shared_ptr<GameObject> Clone() const override;
		void Update(float deltaTime) override;

		void SetLightDirection(const DirectX::XMFLOAT3& dir);
		void SetLightColor(const DirectX::XMFLOAT3& color);
		void SetAmbientStrength(float strength);

		DirectX::XMFLOAT3 GetLightDirection() const;
		DirectX::XMFLOAT3 GetLightColor() const;
		float GetAmbientStrength() const;

	private:
		DirectX::XMFLOAT3 m_LightDirection = { 0.0f, -1.0f, -1.0f };
		DirectX::XMFLOAT3 m_LightColor = { 1.0f, 1.0f, 1.0f };
		float m_AmbientStrength = 0.2f;
	};
}