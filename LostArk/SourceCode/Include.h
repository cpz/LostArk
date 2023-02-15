#ifndef INCLUDE_H
#define INCLUDE_H

#pragma once

#if !defined(_CRT_SECURE_NO_WARNINGS)
#define _CRT_SECURE_NO_WARNINGS
#endif

#if !defined(WIN32_LEAN_AND_MEAN)
#define WIN32_LEAN_AND_MEAN
#endif

#if !defined(WINDOWS_IGNORE_PACKING_MISMATCH)
#define WINDOWS_IGNORE_PACKING_MISMATCH
#endif

#include <Windows.h>
#include <Psapi.h>
#include <TlHelp32.h>
#include <thread>
#include <chrono>
#include <vector>
#include <memory>
#include <array>
#include <mutex>
#include <algorithm>
#include <optional>
#include <atomic>
#include <string>
#include <sstream>      // std::wstringstream
#include <iostream>     // std::cout, std::fixed
#include <iomanip>      // std::setprecision
#include <filesystem>


#include "Utils/cx_strenc.h"

#include "Utils/VMTHook.h"
#include "Utils/VMethod.h"

#include <magic_enum/magic_enum.hpp>

#include <dbg/dbg.h>

#include <lazy_importer/lazy_importer.hpp>

#include <polyhook2/CapstoneDisassembler.hpp>
#include <polyhook2/Detour/x64Detour.hpp>
#include <PolyHook/include/polyhook2/Virtuals/VFuncSwapHook.hpp>

#pragma comment (lib, "PolyHook_2.lib")
#pragma comment (lib, "capstone.lib")

#ifdef DrawText
#undef DrawText
#endif

#ifdef PlaySound
#undef PlaySound
#endif

#ifdef SendMessage
#undef SendMessage
#endif

#ifdef GetObject
#undef GetObject
#endif

#define DEBUG_PRINT_ 1

#if DEBUG_PRINT_
#define dbg_log_type(x, type) dbg(x);               \
	                           Log->Write(x, type)

#define dbg_log(x) dbg(x);                          \
	               Log->Write(x, LogType::kConsole)

#else
#define PrintDebug(x, type) x, type
#endif


using namespace std::chrono_literals;
using namespace std::this_thread;

//#define WindowsHookInjection 
//#define ManualMapInjection

#ifdef ManualMapInjection
namespace Shared
{
	struct Vars
	{
		char m_loader[MAX_PATH];
		char m_login[MAX_PATH];
		char m_hwid[MAX_PATH];
		char m_token[MAX_PATH];
	};

	extern Vars* g_pVars;
	extern std::filesystem::path m_fsLoader;
	extern std::wstring		     m_strConfig;

	extern std::atomic< bool >   m_bEject;
	extern std::atomic< bool >   m_bStuck;
	extern std::atomic< bool >   m_bLag;
	extern std::atomic< bool >   m_bPanic;

	extern std::atomic< bool >   m_bLoad;
	extern std::atomic< bool >   m_bSave;
}
#endif

#endif // INCLUDE_H