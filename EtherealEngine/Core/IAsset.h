#pragma once
#include <string>

namespace Ethereal
{
	class IAsset
	{
	public:
		virtual ~IAsset() = default;
		virtual bool Load(const std::string& path) = 0;
	};
}