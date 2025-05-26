#include "pch.h"
#include "EELogger.h"
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace EtherealEngine
{
	EELogger::EELogger()
	{
		logger = spdlog::stdout_color_mt("Ethereal");
		spdlog::set_level(spdlog::level::trace); // Set global level
		spdlog::set_pattern("[%T] [%^%l%$] %v");
	}

	EELogger::~EELogger()
	{
		
	}

	void EELogger::Info(const std::string& msg)
	{
		logger->info(msg);
	}
	void EELogger::Warn(const std::string& msg)
	{
		logger->warn(msg);
	}
	void EELogger::Error(const std::string& msg)
	{
		logger->error(msg);
	}
	void EELogger::Debug(const std::string& msg)
	{
		logger->debug(msg);
	}
}