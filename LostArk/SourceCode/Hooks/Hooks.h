#ifndef HOOKS_H
#define HOOKS_H

#include "../Include.h"
#include "../UnrealSDK/SDK.hpp"

#include "../Utils/Detours.h"

bool Hook();
int kierohook();

void Hooked_ProcessEvent(SDK::UObject* pUObj, SDK::UFunction* pUFunc, void* pParams, void* pResult);
void Hooked_dtProcessEvent(SDK::UObject* pUObj, SDK::UFunction* pUFunc, void* pParams, void* pResult);

using processevent_fn = void(__thiscall*)(class SDK::UObject *, class SDK::UFunction *, void *, void *);
extern processevent_fn origprocessevent;

extern PLH::VFuncMap game_viewport_client_hook;
extern PLH::VFuncMap hud_hook;
extern PLH::VFuncMap player_controller_hook;

extern PLH::VFuncSwapHook* vfunc_gameviewport_hook;
extern PLH::VFuncSwapHook* vfunc_player_controller_hook;
extern PLH::VFuncSwapHook* vfunc_hud_hook;

extern std::vector< std::unique_ptr< PLH::IHook> >  vfunc_hooks;

extern func_hook processeventhook;
#endif