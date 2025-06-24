#pragma once
#include <filesystem>
#include "Platform/EEWinIncludes.h"
#include <string>
#include <locale>
#include <codecvt>
#include "Assets/Shaders.h"
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

// Converts WCHAR* (UTF-16) to std::string (UTF-8)
inline std::string WCharToString(const WCHAR* wstr)
{
	if (!wstr) return {};
	int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, nullptr, 0, nullptr, nullptr);
	if (size_needed <= 0) return {};
	std::string strTo(size_needed - 1, 0); // exclude null terminator
	WideCharToMultiByte(CP_UTF8, 0, wstr, -1, &strTo[0], size_needed, nullptr, nullptr);
	return strTo;
}

// Converts std::string (UTF-8) to std::wstring (UTF-16)
inline std::wstring StringToWChar(const std::string& str)
{
	if (str.empty()) return {};
	int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);
	if (size_needed <= 0) return {};
	std::wstring wstrTo(size_needed - 1, 0); // exclude null terminator
	MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &wstrTo[0], size_needed);
	return wstrTo;
}

inline const char* ShaderTypeToString(Ethereal::ShaderType type)
{
	switch (type)
	{
	case Ethereal::ShaderType::VERTEX_SHADER:   return "VERTEX_SHADER";
	case Ethereal::ShaderType::PIXEL_SHADER:    return "PIXEL_SHADER";
	case Ethereal::ShaderType::GEOMETRY_SHADER: return "GEOMETRY_SHADER";
	case Ethereal::ShaderType::HULL_SHADER:     return "HULL_SHADER";
	case Ethereal::ShaderType::DOMAIN_SHADER:   return "DOMAIN_SHADER";
	case Ethereal::ShaderType::COMPUTE_SHADER:  return "COMPUTE_SHADER";
	default:                          return "UNKNOWN_SHADER";
	}
}

inline float RandomFloatRange(float min, float max)
{
	return min + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (max - min)));
}

inline DXGI_FORMAT FormatStringToDXGI(const std::string& formatStr)
{
	if (formatStr == "R32G32B32_FLOAT") return DXGI_FORMAT_R32G32B32_FLOAT;
	if (formatStr == "R32G32_FLOAT")    return DXGI_FORMAT_R32G32_FLOAT;
	if (formatStr == "R8G8B8A8_UNORM")  return DXGI_FORMAT_R8G8B8A8_UNORM;
	// Add more as needed
	throw std::runtime_error("Unknown DXGI format: " + formatStr);
}