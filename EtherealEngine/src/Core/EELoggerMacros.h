#pragma once
#include "pch.h"
#include "EEContext.h"
#include "EELogger.h"
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

// Helper to translate HRESULT to string
inline std::string HrToString(HRESULT hr)
{
	char* msgBuf = nullptr;
	DWORD len = FormatMessageA(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		nullptr, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPSTR)&msgBuf, 0, nullptr);
	std::string result;
	if (len && msgBuf)
	{
		result = msgBuf;
		LocalFree(msgBuf);
	}
	else
	{
		std::ostringstream oss;
		oss << "Unknown HRESULT 0x" << std::hex << hr;
		result = oss.str();
	}
	return result;
}

// if _DeBUG is defined, enable debug logging
#ifdef _DEBUG
// General logging macros using spdlog-style formatting
#define LOG_INFO(fmt, ...)   EtherealEngine::EEContext::Get().GetLogger()->GetInternalLogger()->info(fmt, __VA_ARGS__)
#define LOG_WARN(fmt, ...)   EtherealEngine::EEContext::Get().GetLogger()->GetInternalLogger()->warn(fmt, __VA_ARGS__)
#define LOG_ERROR(fmt, ...)  EtherealEngine::EEContext::Get().GetLogger()->GetInternalLogger()->error(fmt, __VA_ARGS__)
#define LOG_DEBUG(fmt, ...)  EtherealEngine::EEContext::Get().GetLogger()->GetInternalLogger()->debug(fmt, __VA_ARGS__)

// Macro to log HRESULT with message
#define LOG_HRINFO(hr, msg) \
    LOG_INFO("{} (HRESULT=0x{:08X}: {})", msg, static_cast<unsigned int>(hr), HrToString(hr))
#define LOG_HRWARN(hr, msg) \
	LOG_WARN("{} (HRESULT=0x{:08X}: {})", msg, static_cast<unsigned int>(hr), HrToString(hr))
#define LOG_HRERROR(hr, msg) \
	LOG_ERROR("{} (HRESULT=0x{:08X}: {})", msg, static_cast<unsigned int>(hr), HrToString(hr))
#else
#define LOG_INFO(fmt, ...)
#define LOG_WARN(fmt, ...)
#define LOG_ERROR(fmt, ...)
#define LOG_DEBUG(fmt, ...)
#define LOG_HRINFO(hr, msg)
#define LOG_HRWARN(hr, msg)
#define LOG_HRERROR(hr, msg)
#endif