#pragma once
#include "pch.h"

namespace spdlog
{
	class logger;
}

namespace EtherealEngine
{
	class EELogger
	{
	public:
		EELogger();
		~EELogger();

		void Info(const std::string& msg);
		void Warn(const std::string& msg);
		void Error(const std::string& msg);
		void Debug(const std::string& msg);

		std::shared_ptr<spdlog::logger> GetInternalLogger() const
		{
			return logger;
		}

	private:
		std::shared_ptr<spdlog::logger> logger;
	};
}
