#include "../Utils/Tools.h"

#include <bcrypt.h>

#define _INTSIZEOF(n)   ( (sizeof(n) + sizeof(int) - 1) & ~(sizeof(int) - 1) )

namespace Tools
{
	auto DbgPrint(const char* text, ...) -> void
	{
		char buf[1024];

		const char* format;

		char* arg;

		int	len;

		if (!text)
			return;

		format = text;
		arg = (char*)&text + _INTSIZEOF(text);
		len = 0;

		if (!arg)
			return;

		len = vsprintf_s(buf, format, arg);

		buf[len + 0] = '\r';
		buf[len + 1] = '\n';
		buf[len + 2] = '\0';

		len += 2;

		OutputDebugStringA(buf);
		std::printf(buf);
	}

	/// <summary>
	/// can_read_ptr - verifies that pointer can be used for write or read operations;
	/// </summary>
	/// <param name="ptr"> Pointer to verify </param>
	/// <returns> True if can be used for reading\write, false if not; </returns>
	auto can_read_ptr(void* ptr) -> bool
	{
		/*if ( Memory::IsPointer( ptr ) )
			return false;*/
		
		MEMORY_BASIC_INFORMATION mbi;
		
		// VirtualQuery( ptr, &mbi, sizeof( mbi ) );
		// LI_FN(VirtualQuery).get()( ptr, &mbi, sizeof(mbi) );
		static auto virtual_query = LI_FN(VirtualQuery).in(LI_MODULE("kernel32.dll").cached());
		virtual_query(ptr, &mbi, sizeof(mbi));

		return (!(mbi.Protect & PAGE_GUARD) && mbi.Protect != PAGE_NOACCESS && mbi.State == MEM_COMMIT &&
			mbi.Protect & (PAGE_READONLY | PAGE_EXECUTE_READ | PAGE_READWRITE | PAGE_EXECUTE_READWRITE));
	}

	auto raise_bsod( void ) -> void
	{
		using NtraiseharderrorFn = NTSTATUS(NTAPI*)(NTSTATUS, ULONG, ULONG OPTIONAL, PULONG_PTR, ULONG, PULONG);
		using RtladjustprivilegeFn = NTSTATUS(NTAPI*)(ULONG, BOOLEAN, BOOLEAN, PBOOLEAN);
		
		BOOLEAN b_enabled = 0;
		ULONG u_resp = 0;

		const auto h_handle = LI_MODULE("ntdll.dll").safe_cached();
		
		static auto adjust_func = LI_FN(GetProcAddress).safe_cached()( static_cast<HMODULE>(h_handle), "RtlAdjustPrivilege" );
		if (adjust_func == nullptr)
			return;
		
		static auto raise_func = LI_FN(GetProcAddress).safe_cached()( static_cast<HMODULE>(h_handle), "NtRaiseHardError" );
		if (raise_func == nullptr)
			return;

		static auto adjust_call = reinterpret_cast<RtladjustprivilegeFn>   ( adjust_func );
		static auto raise_call  = reinterpret_cast<NtraiseharderrorFn>     ( raise_func );

		adjust_call(19, TRUE, FALSE, &b_enabled);
		raise_call(STATUS_ENCLAVE_VIOLATION, 0, 0, nullptr, 6, &u_resp);
	}

	auto __vectorcall wait_for(const char* module, const char* function) -> void*
	{
		volatile void* p_ret = nullptr; //we use volatile to prevent aggressive compiler optimization
		while (!(p_ret = LI_FN(GetProcAddress)(LI_FN(GetModuleHandleA)(module), function))) {
			sleep_for(50ms);
		}
		
		return const_cast<void*>(p_ret);
	}

	auto __vectorcall wait_for(const char* module) -> void
	{
		while (LI_FN(GetModuleHandleA)(module) == nullptr)
		{
			sleep_for(50ms);
		}
	}

	auto __vectorcall wait_for(void* object, void* value) -> void
	{
		while (!(value = (object)))
		{
			sleep_for(50ms);
		}
	}

    auto __vectorcall calculate_call(const std::uintptr_t call_opcode_address, const std::string sz_module, const bool b_rva) -> std::uintptr_t
	{
		// You don't need this line, it's just for showing the RVA
		const auto call_opcode_address_rva = call_opcode_address - reinterpret_cast<std::uintptr_t>( LI_FN( GetModuleHandleA ).get()( sz_module.c_str() ) );

		//std::printf("Call OP code address at 0x%X, relative funciton offset is at 0x%X\n", call_opcode_address_rva, call_opcode_address_rva + 1);
		dbg("Call OP code address at:", (dbg::hex(call_opcode_address_rva)));
		dbg("Relative funciton offset is at:", (dbg::hex(call_opcode_address_rva + 1)));

		// Get the relative function call address
		const auto relative_call_address = *reinterpret_cast<std::intptr_t*>(call_opcode_address + 1);
		
		//std::printf("Relative function call address is 0x%X\n", relative_call_address);
		dbg("Relative function call address is:", (dbg::hex(relative_call_address)));

		/*
			Get the real function address.
			What we have to do to get it is to skip the call opcode, and additionally skip
			the bytes which make up the relative address.
		*/
		const auto real_function_address = call_opcode_address + 1 /*Skip E8 opcode*/ + sizeof(std::int32_t) /*Skip bytes of relative address*/ + relative_call_address;

		// Again, substracting only so we have the RV
		const auto rva_address = real_function_address - reinterpret_cast<std::uintptr_t>( LI_FN( GetModuleHandleA ).get() ( sz_module.c_str() ) );
		//std::printf("Real function address is at 0x%X\n", rva_address );
		dbg("RVA of real function address is at:", (dbg::hex(rva_address)));

		// Real function address
		//std::printf("function address is at 0x%X\n", real_function_address);
		dbg("Real function address is at:", (dbg::hex(real_function_address)));

		std::uintptr_t ui_result = 0;
		if (b_rva)
			ui_result = rva_address;
		else
			ui_result = real_function_address;

		return ui_result;
	}

}