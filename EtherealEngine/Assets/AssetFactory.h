#pragma once
#include "Core/EtherealIncludes.h"
#include <functional>
#include "GameObject.h"

namespace Ethereal
{
	using GameObjectFactoryFunc = std::function<std::shared_ptr<GameObject>(const std::string& type)>;
}