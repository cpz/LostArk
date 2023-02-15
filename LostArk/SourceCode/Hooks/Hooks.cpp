#include "../Hooks/Hooks.h"
#include "../Utils/FindPattern.h"
#include "../Utils/Tools.h"

#include <kiero/kiero.h>

#include "../Utils/Log.h"

#if KIERO_INCLUDE_D3D9
# include <impl/d3d9_impl.h>
#endif

#if KIERO_INCLUDE_D3D10
# include <impl/d3d10_impl.h>
#endif

#if KIERO_INCLUDE_D3D11
# include <impl/d3d11_impl.h>
#endif

PLH::VFuncMap game_viewport_client_hook;
PLH::VFuncMap hud_hook;
PLH::VFuncMap player_controller_hook;

PLH::VFuncSwapHook* vfunc_gameviewport_hook;
PLH::VFuncSwapHook* vfunc_player_controller_hook;
PLH::VFuncSwapHook* vfunc_hud_hook;


std::vector< std::unique_ptr< PLH::IHook> >  vfunc_hooks;

func_hook processeventhook;

#include "polyhook2/Detour/x64Detour.hpp"

 uint64_t inputTramp = NULL;
 uint64_t eventTramp = NULL;

using InputKeyFn = int(__thiscall*)(SDK::UInteraction* interaction, unsigned int __edx, int index, SDK::EInputEvent input_event, float a5, unsigned int a6);

NOINLINE int h_input(SDK::UInteraction* interaction, unsigned int __edx, int index, SDK::EInputEvent input_event, float a5, unsigned int a6) {
	std::cout << interaction->GetFullName() << '\n';
	std::cout << __edx << '\n';
	std::cout << index << '\n';
	std::cout << static_cast<int>(input_event) << '\n';
	std::cout << a5 << '\n';
	std::cout << a6 << '\n';

	char buffer[256];
	sprintf_s(buffer, "{%s} <%i> <%i> {%s} :%f: <%i>", interaction->GetFullName().c_str(), __edx, index, magic_enum::enum_name<SDK::EInputEvent>(input_event), a5, a6);
	Log->Write(buffer, LogType::kDebug);
	
	return PLH::FnCast( inputTramp, InputKeyFn() ) ( interaction, __edx, index, input_event, a5, a6 );
}

NOINLINE void h_event(SDK::UObject* pUObj, SDK::UFunction* pUFunc, void* pParams, void* pResult) {
	std::cout << pUObj->GetFullName() << '\n';
	std::cout << pUFunc->GetFullName() << '\n';

	PLH::FnCast( eventTramp, processevent_fn() ) (pUObj, pUFunc, pParams, pResult);
}


void Hooked_dtProcessEvent(SDK::UObject* pUObj, SDK::UFunction* pUFunc, void* pParams, void* pResult)
{
	char func[256];
	sprintf_s(func, "{%s} <%i>", pUFunc->GetFullName().c_str(), pUFunc->ObjectInternalInteger);
	Log->Write(func, LogType::kDebug);

	unhook(&processeventhook);
	processevent_fn call = (processevent_fn)processeventhook.call_addr;
	call(pUObj, pUFunc, pParams, pResult);
	rehook(&processeventhook);
}

auto Hook( void ) -> bool
{
	PLH::VFuncMap vfunc_redirect = { { static_cast<uint16_t>( 67 ), reinterpret_cast<uint64_t>( &Hooked_ProcessEvent ) } };

	SDK::UEFGameViewportClient* m_game_viewport_client;

	if (SDK::g_pViewport != nullptr)
	{
		dbg("Using EFEngine GameViewportClient");

		m_game_viewport_client = SDK::g_pViewport;
	}
	else
	{
		dbg("Using Engine GameViewportClient");


		m_game_viewport_client = reinterpret_cast< SDK::UEFGameViewportClient* > ( SDK::g_pEngine->GameViewport );
	}

	if (m_game_viewport_client  == nullptr)
	{
		dbg("Can't apply 'game viewport client' swap");

		return false;
	}

	auto gameviewport_hook = std::make_unique< PLH::VFuncSwapHook > ( reinterpret_cast< char* >( m_game_viewport_client ), vfunc_redirect, &game_viewport_client_hook );
	vfunc_hooks.push_back( std::move( gameviewport_hook ) );
	 

	if (SDK::g_pPC == nullptr)
	{
		dbg("Can't apply 'player controller' swap");

		return false;
	}

	auto controller_hook = std::make_unique< PLH::VFuncSwapHook > ( reinterpret_cast<char*>( SDK::g_pPC ), vfunc_redirect, &player_controller_hook );
	vfunc_hooks.push_back( std::move( controller_hook ) );

	if (SDK::g_pPC->myHUD == nullptr)
	{
		dbg("Can't apply 'hud' swap");

		return false;
	}

	auto _hud_hook = std::make_unique< PLH::VFuncSwapHook > ( reinterpret_cast<char*>( SDK::g_pPC->myHUD ), vfunc_redirect, &hud_hook );
	vfunc_hooks.push_back( std::move( _hud_hook ) );

	dbg("hooking functions");
	for(const auto& h : vfunc_hooks) {
		if ( !h->hook() ) {
			dbg("something went wrong with hook!");
			return false;
		}
		
		dbg("hooking: ", h);
	}
	
//	if( !vfunc_hud_hook->hook() )
//	{
//#if DEBUG_PRINT_
//		dbg("%s:%i: Can't apply 'my HUD' hook\n", __FUNCTION__, __LINE__);
//#endif
//
//		return false;
//	}
	
	/*if (!kierohook())
	{
#if DEBUG_PRINT_
		dbg("Can't apply 'Direct X' detour!\n");
#endif
		
		return false;
	}*/

	
	//auto interaction = SDK::g_pPC->Interactions[0];
	//dbg("Interaction: %I64X\n", interaction);

	//auto func_addr = *reinterpret_cast<std::uintptr_t*>(interaction);
	//dbg("%I64X\n", func_addr);

	//func_addr = *reinterpret_cast<std::uintptr_t*>(func_addr + 0x270);
	//dbg("%I64X\n", func_addr);

	//PLH::CapstoneDisassembler dis(PLH::Mode::x64);
	//PLH::x64Detour input((char*)func_addr, (char*)&h_input, &inputTramp, dis);
	//input.hook();


	/*auto p_process_event = PatternScan("EFEngine.dll", "40 55 41 56 41 57 48 81 EC A0 00 00 00 48 8D 6C 24 30");
	hook_init(&processeventhook, p_process_event, Hooked_dtProcessEvent, "processevent::detours");
	rehook(&processeventhook);*/
	
	/*PLH::x64Detour event((char*)p_process_event, (char*)&h_event, &eventTramp, dis);
	input.hook()*/;

	/*Canvas Transient.CanvasObject
	Function Engine.Canvas.GetDefaultCanvasFont*/
	
	return true;
}