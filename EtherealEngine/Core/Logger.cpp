#include "Core/Logger.h"

namespace Ethereal
{
	std::shared_ptr<spdlog::logger> Logger::s_CoreLogger;
	std::shared_ptr<spdlog::logger> Logger::s_GameLogger;

	void Logger::Init()
	{
		std::vector<spdlog::sink_ptr> engineSinks;
		engineSinks.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
		engineSinks.emplace_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>("EtherealEngine.log", true));

		engineSinks[0]->set_pattern("%^[%Y-%m-%d %H:%M:%S] [%n] [%l] %v%$");
		engineSinks[1]->set_pattern("[%Y-%m-%d %H:%M:%S] [%n] [%l] %v");

		s_CoreLogger = std::make_shared<spdlog::logger>("Ethereal", engineSinks.begin(), engineSinks.end());
		spdlog::register_logger(s_CoreLogger);
		s_CoreLogger->set_level(spdlog::level::trace);
		s_CoreLogger->flush_on(spdlog::level::trace);

		std::vector<spdlog::sink_ptr> gameSinks;
		gameSinks.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
		gameSinks.emplace_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>("KingdomOfEnoch.log", true));

		gameSinks[0]->set_pattern("%^[%Y-%m-%d %H:%M:%S] [%n] [%l] %v%$");
		gameSinks[1]->set_pattern("[%Y-%m-%d %H:%M:%S] [%n] [%l] %v");

		s_GameLogger = std::make_shared<spdlog::logger>("KingdomOfEnoch", gameSinks.begin(), gameSinks.end());
		spdlog::register_logger(s_GameLogger);
		s_GameLogger->set_level(spdlog::level::trace);
		s_GameLogger->flush_on(spdlog::level::trace);
	}

	std::shared_ptr<spdlog::logger>& Logger::GetCoreLogger()
	{
		return s_CoreLogger;
	}

	std::shared_ptr<spdlog::logger>& Logger::GetGameLogger()
	{
		return s_GameLogger;
	}
}