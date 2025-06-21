#pragma once
#include <memory>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <filesystem>
#include <comdef.h>
#include "EtherealIncludes.h"
#include "EngineUtils.h"

namespace Ethereal
{
	class ETHEREAL_API Logger
	{
	public:
		static void Init();

		static std::shared_ptr<spdlog::logger>& GetCoreLogger();
		static std::shared_ptr<spdlog::logger>& GetGameLogger();

	private:
		static std::shared_ptr<spdlog::logger> s_CoreLogger;
		static std::shared_ptr<spdlog::logger> s_GameLogger;
	};

	namespace Internal
	{
		inline std::string ExtractFilename(const char* fullPath)
		{
			return std::filesystem::path(fullPath).filename().string();
		}
	}
}

// Logging macros
#define LOG_TRACE(fmt, ...)    ::Ethereal::Logger::GetCoreLogger()->trace("[{}:{}]\n " fmt, ::Ethereal::Internal::ExtractFilename(__FILE__), __LINE__, ##__VA_ARGS__)
#define LOG_INFO(fmt, ...)     ::Ethereal::Logger::GetCoreLogger()->info("[{}:{}]\n " fmt, ::Ethereal::Internal::ExtractFilename(__FILE__), __LINE__, ##__VA_ARGS__)
#define LOG_WARN(fmt, ...)     ::Ethereal::Logger::GetCoreLogger()->warn("[{}:{}]\n " fmt, ::Ethereal::Internal::ExtractFilename(__FILE__), __LINE__, ##__VA_ARGS__)
#define LOG_ERROR(fmt, ...)    ::Ethereal::Logger::GetCoreLogger()->error("[{}:{}]\n " fmt, ::Ethereal::Internal::ExtractFilename(__FILE__), __LINE__, ##__VA_ARGS__)
#define LOG_CRITICAL(fmt, ...) ::Ethereal::Logger::GetCoreLogger()->critical("[{}:{}]\n " fmt, ::Ethereal::Internal::ExtractFilename(__FILE__), __LINE__, ##__VA_ARGS__)

#define GAME_LOG_TRACE(fmt, ...)    ::Ethereal::Logger::GetGameLogger()->trace("[{}:{}]\n " fmt, ::Ethereal::Internal::ExtractFilename(__FILE__), __LINE__, ##__VA_ARGS__)
#define GAME_LOG_INFO(fmt, ...)     ::Ethereal::Logger::GetGameLogger()->info("[{}:{}]\n " fmt, ::Ethereal::Internal::ExtractFilename(__FILE__), __LINE__, ##__VA_ARGS__)
#define GAME_LOG_WARN(fmt, ...)     ::Ethereal::Logger::GetGameLogger()->warn("[{}:{}]\n " fmt, ::Ethereal::Internal::ExtractFilename(__FILE__), __LINE__, ##__VA_ARGS__)
#define GAME_LOG_ERROR(fmt, ...)    ::Ethereal::Logger::GetGameLogger()->error("[{}:{}]\n " fmt, ::Ethereal::Internal::ExtractFilename(__FILE__), __LINE__, ##__VA_ARGS__)
#define GAME_LOG_CRITICAL(fmt, ...) ::Ethereal::Logger::GetGameLogger()->critical("[{}:{}]\n " fmt, ::Ethereal::Internal::ExtractFilename(__FILE__), __LINE__, ##__VA_ARGS__)

namespace Ethereal
{
	class COMException
	{
	public:
		COMException(HRESULT hr, const std::string& msg, const std::string& file, const std::string& function, int line)
		{
			_com_error error(hr);
			whatmsg = L"Msg: " + StringToWChar(std::string(msg)) + L"\n";
			whatmsg += StringToWChar(error.ErrorMessage());
			whatmsg += L"\nFile: " + StringToWChar(file);
			whatmsg += L"\nFunction: " + StringToWChar(function);
			whatmsg += L"\nLine: " + StringToWChar(std::to_string(line));
			LOG_ERROR("COMException: {}", WCharToString(whatmsg.c_str()).c_str());
		}

		const wchar_t* what() const
		{
			return whatmsg.c_str();
		}
	private:
		std::wstring whatmsg;
	};
}

#define COM_ERROR_IF_FAILED( hr, msg ) if( FAILED( hr ) ) throw Ethereal::COMException( hr, msg, __FILE__, __FUNCTION__, __LINE__ )

