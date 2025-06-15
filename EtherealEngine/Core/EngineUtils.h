#pragma once
#include <filesystem>
#include "Platform/EEWinIncludes.h"

inline std::filesystem::path GetExecutableDirectory()
{
	wchar_t buffer[MAX_PATH];
	GetModuleFileNameW(NULL, buffer, MAX_PATH);
	return std::filesystem::path(buffer).parent_path();
}

inline std::filesystem::path GetAssetsDirectory()
{
	return GetExecutableDirectory() / "Assets";
}