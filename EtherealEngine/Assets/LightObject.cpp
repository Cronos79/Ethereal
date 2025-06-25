#include "Assets/LightObject.h"
#include "Core/Logger.h"
#include "AssetFactory.h"

namespace Ethereal
{
	LightObject::LightObject()
	{
		m_TypeName = "LightObject";
		// Default position and direction can be adjusted later via transform
	}

	std::shared_ptr<GameObject> LightObject::Clone() const
	{
		auto copy = std::make_shared<LightObject>();
		copy->SetName(this->GetName());
		copy->SetTransform(this->GetTransform());
		copy->SetLightDirection(this->GetLightDirection());
		copy->SetLightColor(this->GetLightColor());
		copy->SetAmbientStrength(this->GetAmbientStrength());
		return copy;
	}

	void LightObject::Update(float deltaTime)
	{
		// Future logic: animate light, rotate, etc.
	}

	void LightObject::SetLightDirection(const DirectX::XMFLOAT3& dir)
	{
		m_LightDirection = dir;
	}

	void LightObject::SetLightColor(const DirectX::XMFLOAT3& color)
	{
		m_LightColor = color;
	}

	void LightObject::SetAmbientStrength(float strength)
	{
		m_AmbientStrength = strength;
	}

	DirectX::XMFLOAT3 LightObject::GetLightDirection() const
	{
		return m_LightDirection;
	}

	DirectX::XMFLOAT3 LightObject::GetLightColor() const
	{
		return m_LightColor;
	}

	float LightObject::GetAmbientStrength() const
	{
		return m_AmbientStrength;
	}

	// Register this type with the factory
	REGISTER_GAMEOBJECT_TYPE(LightObject);
}