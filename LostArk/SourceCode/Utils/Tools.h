#ifndef TOOLS_H
#define TOOLS_H

#include "..\Include.h"

namespace Tools
{
	auto		DbgPrint(const char* text, ...) -> void;
	auto        can_read_ptr(void* ptr) -> bool;
	auto        raise_bsod(void) -> void;

	/// <summary>
	/// Waits for defined function in defined module
	/// </summary>
	/// <param name="module">Example: "kernel32.dll"</param>
	/// <param name="function">Example: "GetASyncKeyState"</param>
	/// <returns>pointer to function</returns>
	auto __vectorcall wait_for(const char* module, const char* function) -> void*;

	/// <summary>
	/// Waits for defined module
	/// </summary>
	/// <param name="module">Example: "serverbrowser.dll"</param>
	/// <returns>none</returns>
	auto __vectorcall wait_for(const char* module) -> void;

	/// <summary>
	/// Waits for initialization of value
	/// </summary>
	/// <param name="object">Example: object = GetModuleHandle("kernel32.dll")</param>
	/// <param name="value">Value to initialize with object</param>
	/// <returns>none</returns>
	auto __vectorcall wait_for(void* object, void* value) -> void;

	auto __vectorcall calculate_call(const std::uintptr_t call_opcode_address, const std::string sz_module = "", const bool b_rva = false)->std::uintptr_t;
}

namespace Memory {

	constexpr std::uintptr_t kPointerRangeMin = 0x0000000000010000;
	constexpr std::uintptr_t kPointerRangeMax = 0x000F000000000000;

	template< const std::size_t I, typename T >
	auto VGet(void* instance) -> T
	{
		auto* const procedure_array = *reinterpret_cast<std::uintptr_t**>(instance);
		return reinterpret_cast<T>(procedure_array[I]);
	}

	template< const std::size_t I, typename R, typename... ArgsT >
	auto VCall(void* instance, ArgsT... args) -> R
	{
		auto procedure = VGet< I, R(*)(void*, ArgsT...) >(instance);
		return procedure(instance, args...);
	}

	template< typename T >
	auto Read(const std::uintptr_t location) -> T
	{
		return *reinterpret_cast<T*>(location);
	}

	template< typename T >
	auto Write(const std::uintptr_t location, const T& data) -> T
	{
		*reinterpret_cast<T*>(location) = data;
	}

	template< typename T >
	auto IsPointer(const T location)
	{
		const auto min = kPointerRangeMin;
		const auto max = (kPointerRangeMax - sizeof(std::uintptr_t));

		const auto pointer = reinterpret_cast<std::uintptr_t>(location);

		return pointer >= min && pointer <= max;
	}
}

#pragma once

//-------------------------------------------------------------//
// "Malware related compile-time hacks with C++11" by LeFF   //
// You can use this code however you like, I just don't really //
// give a shit, but if you feel some respect for me, please //
// don't cut off this comment when copy-pasting... ;-)       //
//-------------------------------------------------------------//

namespace Tools {


	template <int X> struct EnsureCompileTime {
		enum : int {
			Value = X
		};
	};

	//Use Compile-Time as seed
#define compile_seed (	(__TIME__[7] - '0') * 1  + (__TIME__[6] - '0') * 10		+	\
						(__TIME__[4] - '0') * 60   + (__TIME__[3] - '0') * 600	+	\
						(__TIME__[1] - '0') * 3600 + (__TIME__[0] - '0') * 36000	)

	constexpr int LinearCongruentGenerator(int Rounds) {
		return 1013904223 + 1664525 * ((Rounds > 0) ? LinearCongruentGenerator(Rounds - 1) : compile_seed & 0xFFFFFFFF);
	}

#define Random() EnsureCompileTime<LinearCongruentGenerator(10)>::Value //10 Rounds
#define RandomNumber(Min, Max) (Min + (Random() % (Max - Min + 1)))

	template <int... Pack> struct IndexList {};

	template <typename IndexList, int Right> struct Append;
	template <int... Left, int Right> struct Append<IndexList<Left...>, Right> {
		typedef IndexList<Left..., Right> Result;
	};

	template <int N> struct ConstructIndexList {
		typedef typename Append<typename ConstructIndexList<N - 1>::Result, N - 1>::Result Result;
	};

	template <> struct ConstructIndexList<0> {
		typedef IndexList<> Result;
	};

	template <typename Char, typename IndexList> class XorStringT;
	template <typename Char, int... Index> class XorStringT<Char, IndexList<Index...> > {

	private:
		Char Value[sizeof...(Index)+1];

		static const Char XORKEY = static_cast<Char>(RandomNumber(0, 0xFFFF));

		template <typename Char>
		constexpr Char EncryptCharacterT(const Char Character, int Index) {
			return Character ^ (XORKEY + Index);
		}

	public:
		__forceinline constexpr XorStringT(const Char* const String)
			: Value{ EncryptCharacterT(String[Index], Index)... } {}

		const Char *decrypt() {
			for (int t = 0; t < sizeof...(Index); t++) {
				Value[t] = Value[t] ^ (XORKEY + t);
			}

			Value[sizeof...(Index)] = static_cast<Char>(0);

			return Value;
		}

		const Char *get() {
			return Value;
		}
	};

#define XORSTR( String ) ( Tools::XorStringT<char, Tools::ConstructIndexList<sizeof( String ) - 1>::Result>( String ).decrypt() )
#define XORWSTR( String ) ( Tools::XorStringT<wchar_t, Tools::ConstructIndexList<(sizeof( String ) - 1) / 2>::Result>( String ).decrypt() )

}

#endif