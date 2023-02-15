#include "../Include.h"

#include "../UnrealSDK/SDK.hpp"
#include "../Hooks/Hooks.h"
#include "../Utils/Tools.h"

auto startup( void ) -> bool
{
	dbg("startup routine has been called!");

	while (!(SDK::ObjectInitialize()))
		sleep_for(100ms);

	dbg("found objects");

	while (!(SDK::NamesInitialize()))
		sleep_for(100ms);

	dbg("found names");
	
	while (!(SDK::WorldInitialize()))
		sleep_for(100ms);

	dbg("found world");

	while (!(SDK::EngineInitialize()))
		sleep_for(100ms);

	dbg("found engine");
	
	SDK::g_pWorld = (*SDK::g_world);
	if (SDK::g_pWorld == nullptr)
		return false;

	dbg("world pointer initialized");
	
	SDK::g_pEngine = SDK::GetEngine();
	if (SDK::g_pEngine == nullptr)
	{
		dbg("game engine pointer is empty, using global engine pointer");

		SDK::g_pEngine = (*SDK::g_engine);
		if (SDK::g_pEngine == nullptr)
			return false;
	}

	dbg("engine pointer initialized");
	
	SDK::g_pViewport = SDK::GetViewport();
	while (SDK::g_pViewport == nullptr)
	{
		SDK::g_pViewport = SDK::GetViewport();
		sleep_for(100ms);
	}

	dbg("viewport pointer initialized");
	
	SDK::g_pLocalPlayer = SDK::GetLocalPlayer();
	if (SDK::g_pLocalPlayer == nullptr)
		return false;

	dbg("localplayer pointer initialized");
	
	if (SDK::g_pLocalPlayer->Actor == nullptr)
		return false;
	
	dbg("player controller is not empty, continue");

	SDK::g_pPC = reinterpret_cast<SDK::AEFPlayerController*>(SDK::g_pLocalPlayer->Actor);
	if (SDK::g_pPC == nullptr)
		return false;
	
	dbg("player controller pointer initialized");

	SDK::g_pHUD = reinterpret_cast<SDK::AHUD*>(SDK::g_pPC->myHUD);
	if (SDK::g_pHUD == nullptr)
		return false;

	dbg("hud pointer initialized");

	if (!Hook())
		return false;

	dbg("all your functions belongs to us", "startup routine ends with successsful!");

	return true;
}
