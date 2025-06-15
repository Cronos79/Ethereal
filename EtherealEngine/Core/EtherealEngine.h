#pragma once
#include "Core/Logger.h"
#include "Platform/EEWinIncludes.h"
#include <memory>
#include <string>

class StringConverter
{
	public:
	static std::wstring toWString(const std::string& str)
	{
		return std::wstring(str.begin(), str.end());
	}
	static std::string toString(const std::wstring& wstr)
	{
		return std::string(wstr.begin(), wstr.end());
	}
};