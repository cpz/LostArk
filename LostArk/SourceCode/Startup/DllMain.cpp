#include "../Include.h"
#include "../Hooks/Hooks.h"
#include "../Utils/Log.h"

#include <direct.h>

#include "../Utils/Console.h"
#include "../Utils/Tools.h"

#ifdef ManualMapInjection
Shared::Vars* Shared::g_pVars;
#endif

extern "C" BOOL WINAPI _CRT_INIT(HMODULE module, DWORD reason, LPVOID reserved);

auto startup ( void ) -> bool;

auto leave( void* mod_instance [[maybe_unused]] ) -> void
{
	/*if (vfunc_gameviewport_hook) {
		vfunc_gameviewport_hook->unHook();
		delete vfunc_gameviewport_hook;
	}

	if (vfunc_player_controller_hook) {
		vfunc_player_controller_hook->unHook();
		delete vfunc_player_controller_hook;
	}

	if(vfunc_hud_hook) {
		vfunc_hud_hook->unHook();
		delete vfunc_hud_hook;
	}*/

	dbg("exiting routine has been called!");


	dbg("removing hooks");
	for(auto& h : vfunc_hooks) {
		if(h) {
			dbg("Unhooking: ", h);
			h->unHook();
		}
	}

#ifdef ManualMapInjection
	dbg("deinitialize crt");
	_CRT_INIT(static_cast<HMODULE>(mod_instance), DLL_PROCESS_DETACH, nullptr);
#endif
	
	sleep_for(200ms);
}

auto run( void* mod_instance [[maybe_unused]] ) -> void
{
	dbg("waiting for efengine.dll");
	Tools::wait_for("EFEngine.dll");

	dbg("getting error log singleton");
	auto& error_log = PLH::ErrorLog::singleton();

	dbg("creating class for console");
	auto console = new �R�Ronsole;

#if DEBUG_PRINT_
	auto log_level = PLH::ErrorLevel::INFO;
#else
	auto log_level = PLH::ErrorLevel::NONE;
#endif

	
	auto sz_log_level = magic_enum::enum_name(log_level);
	
	if (sz_log_level.empty())
	{
		dbg( "set log level to:", (log_level) );
	} else {
		dbg( "set log level to:", (sz_log_level.front()) );
		dbg("set log level to:", (sz_log_level.back()));
		dbg("set log level to:", (sz_log_level));
	}
	
	error_log.setLogLevel(log_level);

#if DEBUG_PRINT_
	dbg("starting console");
	console->start("*redacted* :: Lost Ark");

	dbg("getting current path");
	const auto cwd = std::filesystem::current_path();
	const auto path = cwd / "redacted_module_log.txt";
	dbg(cwd);
	dbg(path);
	
	dbg("initialize log");
	Log->Initialize( path.string() );

	dbg("module has been loaded!");
	Log->Write( "Module has been loaded!", LogType::kEvent );
#endif

	dbg("creating lambda function for exit procedure");
	const auto exit_procedure = [&]( )
	{
#if DEBUG_PRINT_
		dbg("launching console stop routine");
		console->stop();
#endif
		
		dbg("launching leave routine");
		leave( mod_instance );

		dbg("free librabry and exit thread");
		//FreeLibraryAndExitThread( static_cast< HMODULE > ( mod_instance ), EXIT_SUCCESS );
		LI_FN(FreeLibraryAndExitThread).get()(static_cast<HMODULE>(mod_instance), ERROR_SUCCESS);
	};

	dbg("launching startup routine");
	if (!startup())
	{
		dbg("startup routined failed. exiting.");

		sleep_for( 1s );
		exit_procedure( );
	}

	dbg("getting async_key_state");
	static auto async_key_state = LI_FN(GetAsyncKeyState).safe_cached();
	if(async_key_state == nullptr) {
		dbg("cannot get async_key_state! exiting.");

		sleep_for(1s);
		exit_procedure();
	}

	dbg("enter in to void");
	while ( !async_key_state( VK_END ) )
	{
#if DEBUG_PRINT_
		console->update();
#endif
		sleep_for( 50ms );
	}


	dbg("panic key was pressed! exiting.");
	exit_procedure( );
}


#ifdef WindowsHookInjection
std::once_flag initialized;

auto GetCurrentModule ( void ) -> HMODULE 
{
	// NB: XP+ solution!
	HMODULE h_module = nullptr;
	GetModuleHandleExA(
		GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,
		reinterpret_cast<LPCSTR>(GetCurrentModule),
		&h_module);

	return h_module;
};

extern "C" __declspec(dllexport)
auto __fastcall initialization_routine(const int32_t code [[maybe_unused]],
	const WPARAM wparam [[maybe_unused]],
	const LPARAM lparam [[maybe_unused]] ) -> HRESULT
{
	auto* const pmsg = reinterpret_cast<MSG*>(lparam);

	// handle race condition from calling hook multiple times
	std::call_once(initialized, [&]()
	{
			if (pmsg->message == 0xd34dc0d3)
			{
				UnhookWindowsHookEx( reinterpret_cast<HHOOK>(lparam) );

				
				std::thread(&run, GetCurrentModule()).detach();
			}
	});
	
	// call next hook in queue
	return CallNextHookEx(nullptr, code, wparam, lparam);
}
#endif

#ifdef ManualMapInjection
auto base_hwid( void* mod_instance, std::uintptr_t mod_base ) -> void
{
	char sz_hwid[12];

	auto p_hwid = *reinterpret_cast<char*>(mod_base);
	strcpy(sz_hwid, &p_hwid);

	if ( std::strcmp( Shared::g_pVars->m_hwid, sz_hwid ) != 0 ) {
		//FreeLibraryAndExitThread(static_cast<HMODULE> (mod_instance), EXIT_SUCCESS);

		LI_FN(FreeLibraryAndExitThread).get()(static_cast<HMODULE>(mod_instance), ERROR_SUCCESS);
	}
}

auto generated_hwid( void ) -> void
{

}

auto connect_to_server( void ) -> void
{

}

extern "C" __declspec(dllexport)
auto injection_routine( void* mod_instance [[maybe_unused]],
                        const std::uintptr_t mod_base [[maybe_unused]],
	                    void* mod_struct [[maybe_unused]] ) -> void
{

	if (!_CRT_INIT(static_cast<HMODULE>(mod_instance), DLL_PROCESS_ATTACH, nullptr))
		return;
	
	//Verify that MZh was overwritten with hwid
	if ( reinterpret_cast<std::uint16_t>( mod_instance ) == 0x5A4D)
	{
		//MessageBoxA(nullptr, "read our MZ!", 0, 0);
		// bsod target cos there shouldn be MZ.
		// Tools::raise_bsod();
	} else {
		Shared::g_pVars = static_cast<Shared::Vars*>( mod_struct );

		base_hwid(mod_instance, mod_base);

		generated_hwid(); // verify base hwid with shared hwid + gen hwid

		connect_to_server(); // connect with all data and verify that this data is right (login, hwid, jwt token)
		
		if( !Shared::g_pVars->m_login ) {
			//FreeLibraryAndExitThread(h_module, EXIT_SUCCESS);

			LI_FN(FreeLibraryAndExitThread).get()(static_cast<HMODULE>(mod_instance), ERROR_SUCCESS);
		}

		Shared::m_fsLoader = Shared::g_pVars->m_loader;
		if( Shared::m_fsLoader.empty() ) {
			//FreeLibraryAndExitThread(h_module, EXIT_SUCCESS);

			LI_FN(FreeLibraryAndExitThread).get()(static_cast<HMODULE>(mod_instance), ERROR_SUCCESS);
		}
	}

	
	std::thread(&run, &mod_instance).detach();
}
#endif

bool __stdcall DllMain(void* mod_instance [[maybe_unused]],
	                   const std::uint32_t call_reason [[maybe_unused]],
	                   void* reserved [[maybe_unused]])
{
	/*dbg("initialize crt");
	if (!_CRT_INIT(static_cast<HMODULE>(mod_instance), call_reason, reserved))
		return false;*/

	dbg(call_reason);
	if (call_reason == DLL_PROCESS_ATTACH)
	{
		dbg("disabling thread library calls");
		if (!LI_FN(DisableThreadLibraryCalls).get()(static_cast<HMODULE>(mod_instance)))
			return false;
		
		dbg("creating thread for run routine");
		std::thread(&run, &mod_instance).detach();
	}

	return true;
}