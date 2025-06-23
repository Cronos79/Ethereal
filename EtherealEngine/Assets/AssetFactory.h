#pragma once
#include "Core/EtherealIncludes.h"
#include <functional>
#include "GameObject.h"

namespace Ethereal
{
	using GameObjectFactoryFunc = std::function<std::shared_ptr<GameObject>(const std::string& type)>;
}

#define REGISTER_GAMEOBJECT_TYPE(TypeName) \
	static bool _registered_##TypeName = []() { \
		Ethereal::GameObject::RegisterType(#TypeName, []() -> std::shared_ptr<Ethereal::GameObject> { \
			return std::make_shared<TypeName>(); \
		}); \
		return true; \
	}()