#ifndef CLOG_HPP
#define CLOG_HPP

#pragma once

#include <fstream>
#include <string>
#include <memory>
#include <shared_mutex>

enum class LogType
{
	kInfo,
	kWarning,
	kError,
	kDebug,
	kEvent,
	kConsole
};

class CLog
{
public:
	void Initialize(const std::string& path);
	void Write(const std::string& text, LogType type);

	auto IsOpen(void) const -> bool;

private:
	static std::string TypeToString(LogType type);

	std::shared_timed_mutex write_mutex_;
	std::ofstream file_;
};

extern std::unique_ptr<CLog> Log;

#endif