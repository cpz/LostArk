#include "../Include.h"

#include "Log.h"
#include <ctime>

#ifdef LAZY_IMPORTER_HPP
#define CLOG_USE_LAZY_IMPORTER
#endif

std::unique_ptr<CLog> Log = std::make_unique<CLog>();

void CLog::Initialize(const std::string& path)
{
	file_.open(path);

	if (!file_.is_open())
		std::runtime_error("Could not open file: " + path);
}

void CLog::Write(const std::string& text, const LogType type)
{

	auto t_time = time(nullptr);
	tm now { };
	localtime_s(&now, &t_time);


	const auto time_prefix = "[" + std::to_string(now.tm_hour) + ":" + std::to_string(now.tm_min) + ":" + std::to_string(now.tm_sec) + "]";
	const auto type_prefix = "[" + TypeToString(type) + "]";
	const auto output = time_prefix + " " + type_prefix + " " + text;

	write_mutex_.lock_shared();
	file_ << output << std::endl;
	write_mutex_.unlock_shared();
}

auto CLog::IsOpen() const -> bool
{
	return this->file_.is_open();
}

std::string CLog::TypeToString(const LogType type)
{
	switch (type)
	{
	case LogType::kInfo:		return "Info";
	case LogType::kWarning:	    return "Warning";
	case LogType::kError:	    return "Error";
	case LogType::kDebug:	    return "Debug";
	case LogType::kEvent:       return "Event";
	case LogType::kConsole:     return "Console";
	default:
		std::invalid_argument("Incorrect log type specified.");
		return "";
	}
}
