#ifndef EZ_CONSOLE_HPP
#define EZ_CONSOLE_HPP

#pragma once

#include "../Include.h"

#include <chrono>

#ifdef LAZY_IMPORTER_HPP
#define CONSOLE_USE_LAZY_IMPORTER
#endif

class „R„Ronsole
{
public:
	„R„Ronsole()
	{

	}

	static constexpr std::uint8_t character_sequence_[] =
	{
		'|', '/', '-', '\\'
	};

	auto start(const std::string& title) -> void
	{
		auto status{ false };

		{
			if (this->m_active_)
			{
				dbg("cannot initialise console object more than once!");
				return;
			}

			this->sz_title_ += title;
			this->m_active_ = true;
		}
		

#ifndef CONSOLE_USE_LAZY_IMPORTER
		this->h_console_window_ = GetConsoleWindow();
#else
		this->h_console_window_ = LI_FN(GetConsoleWindow).get()();
#endif

		if (this->h_console_window_ == nullptr)
		{
			if (!AllocConsole())
			{
				dbg("cannot alloc console!");
				return;
			}
#ifndef CONSOLE_USE_LAZY_IMPORTER
			this->h_console_window_ = GetConsoleWindow();
#else
			this->h_console_window_ = LI_FN(GetConsoleWindow).get()();
#endif
			if (this->h_console_window_ == nullptr)
			{
				dbg("cannot get console window!");
				return;
			}
		}

		{
			std::freopen("CONIN$", "r", stdin);
			std::freopen("CONOUT$", "w", stdout);
			std::freopen("CONOUT$", "w", stderr);
		}

		///
		{
			std::cout.clear();
			std::clog.clear();
			std::cerr.clear();
			std::cin.clear();
		}

#ifndef CONSOLE_USE_LAZY_IMPORTER
		const auto h_con_out = CreateFileA("CONOUT$",
			GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			nullptr,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			nullptr);

		const auto h_con_in = CreateFileA("CONIN$",
			GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			nullptr,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			nullptr);
#else
		const auto h_con_out = LI_FN(CreateFileA).get()("CONOUT$",
			GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			nullptr,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			nullptr);

		const auto h_con_in = LI_FN(CreateFileA).get()("CONIN$",
			GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			nullptr,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			nullptr);
#endif

		///
		{
#ifndef CONSOLE_USE_LAZY_IMPORTER
			status = SetStdHandle(STD_OUTPUT_HANDLE, h_con_out);
			if (!status) {
				dbg("cannot set std output handle!");
			}

			status = SetStdHandle(STD_ERROR_HANDLE, h_con_out);
			if (!status) {
				dbg("cannot set std error handle!");
			}

			status = SetStdHandle(STD_INPUT_HANDLE, h_con_in);
			if (!status) {
				dbg("cannot set std input handle!");
			}
#else
			status = LI_FN(SetStdHandle).get()(STD_OUTPUT_HANDLE, h_con_out);
			if (!status) {
				dbg("cannot set std output handle!");
			}

			status = LI_FN(SetStdHandle).get()(STD_ERROR_HANDLE, h_con_out);
			if (!status) {
				dbg("cannot set std error handle!");
			}

			status = LI_FN(SetStdHandle).get()(STD_INPUT_HANDLE, h_con_out);
			if (!status) {
				dbg("cannot set std input handle!");
			}
#endif
		}


		///
		{
			std::wcout.clear();
			std::wclog.clear();
			std::wcerr.clear();
			std::wcin.clear();
		}

		{
#ifndef CONSOLE_USE_LAZY_IMPORTER
			status = SetConsoleOutputCP(CP_UTF8);
			if (!status) {
				dbg("cannot set console output cp!");
			}

			status = SetConsoleCP(CP_UTF8);
			if (!status) {
				dbg("cannot set console cp!");
			}

			status = SetConsoleTitleA(this->sz_title_.c_str());
			if (!status) {
				dbg("cannot set console title!");
			}
#else
			status = LI_FN(SetConsoleOutputCP).get()(CP_UTF8);
			if (!status) {
				dbg("cannot set console output cp!");
			}

			status = LI_FN(SetConsoleCP).get()(CP_UTF8);
			if (!status) {
				dbg("cannot set console cp!");
			}

			status = LI_FN(SetConsoleTitleA).get()( this->sz_title_.c_str() );
			if (!status) {
				dbg("cannot set console title!");
			}
#endif
		}
	}

	auto update() -> void
	{
		if (!this->m_active_)
		{
			dbg("cannot initialise spinner object without initialization!");
			return;
		}

		// GET TIME SINCE LAST UPDATE
		using clock_t = std::chrono::high_resolution_clock;
		const auto time = clock_t::now();
		const auto time_delta = time - m_last_update_time_;
		const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(time_delta);


		// ONLY UPDATE SPINNER EVERY x MS
		constexpr auto update_time = 150;
		if (duration.count() <= update_time)
		{
			return;
		}

		// UPDATE THE SPINNER CHARACTER
		this->sz_title_.at(1) = static_cast<char>(character_sequence_[this->m_seq_index_]);

#ifndef CONSOLE_USE_LAZY_IMPORTER
		const auto status = SetConsoleTitleA(sz_title_.c_str());
		if (!status) {
			dbg("cannot set console title!");
		}
#else
		const auto status = LI_FN(SetConsoleTitleA).get()(this->sz_title_.c_str());
		if (!status) {
			dbg("cannot set console title!");
		}
#endif

		// INCREMENT AND WRAP
		this->m_seq_index_++;
		this->m_seq_index_ %= sizeof(character_sequence_);

		this->m_last_update_time_ = time;
	}

	auto stop(void) -> void
	{
		std::fclose(stdin);
		std::fclose(stdout);
		std::fclose(stderr);

		if (this->h_console_window_)
		{
#ifndef CONSOLE_USE_LAZY_IMPORTER
			PostMessageA(this->h_console_window_, WM_CLOSE, 0, 0);
#else+
			 LI_FN(PostMessageA).get()(this->h_console_window_, WM_CLOSE, 0, 0);
#endif
		}

#ifndef CONSOLE_USE_LAZY_IMPORTER
		const auto status = FreeConsole();
#else+
		const auto status = LI_FN(FreeConsole).get()();
#endif

		if (!status)
		{
			dbg("cannot free console!");
		}

		if (!this->m_active_)
		{
			dbg("console is not active!");
			return;
		}

		this->m_active_ = false;
	}

private:
	HWND h_console_window_ = nullptr;
	bool m_active_ = false;
	std::string sz_title_ = { "[-] " };
	std::size_t m_seq_index_ = 0;
	std::chrono::steady_clock::time_point m_last_update_time_;
};

#endif