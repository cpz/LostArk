#include <kiero/kiero.h>

#include "../../SourceCode/Utils/Detours.h"
#include "../../SourceCode/Utils/VMTHook.h"

func_hook reset_scene = {};
func_hook end_scene = {};

VTHook* direct_x_hook {};

PLH::VFuncMap direct3d9_hook;

#if KIERO_INCLUDE_D3D9

#include "d3d9_impl.h"
#include <d3d9.h>
#include <assert.h>

#include <imgui/imgui.h>
#include <imgui/examples/imgui_impl_win32.h>
#include <imgui/examples/imgui_impl_dx9.h>

using ResetSceneFn = HRESULT(__thiscall*)(LPDIRECT3DDEVICE9, D3DPRESENT_PARAMETERS*);
using PresentSceneFn = HRESULT(__thiscall*)(LPDIRECT3DDEVICE9, RECT*, RECT*, HWND, RGNDATA*);

PLH::CapstoneDisassembler* capstone_dis;

PLH::x64Detour* present_hook;
PLH::x64Detour* reset_hook;

char* present_hook_func;
char* reset_hook_func;

uint64_t present_hook_tramp = NULL;
uint64_t reset_hook_tramp = NULL;

std::once_flag initialize;

HRESULT Hooked_Reset_Scene(LPDIRECT3DDEVICE9 pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters)
{
	std::printf("hkReset\n");
	
	ImGui_ImplDX9_InvalidateDeviceObjects();

	//const auto result = reinterpret_cast<ResetSceneFn>( direct3d9_hook.at ( 16 ) ) ( pDevice, pPresentationParameters );
	const auto result = PLH::FnCast( reset_hook_tramp, ResetSceneFn() ) ( pDevice, pPresentationParameters );
	
	ImGui_ImplDX9_CreateDeviceObjects();

	return result;
}

HRESULT Hooked_Present_Scene(LPDIRECT3DDEVICE9 pDevice, RECT* pSourceRect, RECT* pDestRect, HWND hDestWindowOverride, RGNDATA* pDirtyRegion)
{
	std::printf("%s\n", __FUNCTION__);
	
	std::call_once(initialize, [&]()
	{
			D3DDEVICE_CREATION_PARAMETERS params;
			pDevice->GetCreationParameters(&params);

			ImGui::CreateContext();
			ImGui_ImplWin32_Init( FindWindowA("EFLaunchUnrealUWindowsClient", nullptr) );
			ImGui_ImplDX9_Init(pDevice);
	});


	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	//impl::showExampleWindow("D3D9");
	
	ImGui::ShowDemoWindow(nullptr);

	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());

	//return reinterpret_cast<present_scene_fn>( direct3d9_hook.at( 17 ) ) ( pDevice, pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion );
	return PLH::FnCast( present_hook_tramp, PresentSceneFn() ) ( pDevice, pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion );
}

void impl::d3d9::init()
{
	const auto directx9_device = kiero::getMethodsTable();

#if DEBUG_PRINT_
	std::printf("d3d9 device: 0x%p\n", directx9_device);
#endif

	if (!PLH::IsValidPtr(reinterpret_cast<void*>(directx9_device)))
	{
#if DEBUG_PRINT_
		std::printf("d3d9 device is not valid pointer.\n");
#endif

		return;
	}

	reset_hook_func = (char*)directx9_device[16];
	present_hook_func = (char*)directx9_device[17];
	
	capstone_dis = new PLH::CapstoneDisassembler(PLH::Mode::x64);
	
	reset_hook = new PLH::x64Detour(reset_hook_func, (char*)&Hooked_Reset_Scene, &reset_hook_tramp, *capstone_dis);
	present_hook = new PLH::x64Detour(present_hook_func, (char*)&Hooked_Present_Scene, &present_hook_tramp, *capstone_dis);

	if (!present_hook || !reset_hook || !capstone_dis)
	{
#if DEBUG_PRINT_
		std::printf("failed to allocate hooking stuff.\n");
#endif

		return;
	}

	if (!present_hook->hook() /*|| !reset_hook->hook()*/)
	{
		delete present_hook;
		delete reset_hook;
		delete capstone_dis;
		
#if DEBUG_PRINT_
		std::printf("unable to hook d3d9 functions.\n");
#endif
	}

//#if defined _M_X64
//	oWndProc = (WNDPROC)SetWindowLongPtr(FindWindowA(NULL, "GAME WINDOW NAME"), GWLP_WNDPROC, (LONG_PTR)WndProc);
//#elif defined _M_IX86
//	oWndProc = (WNDPROC)SetWindowLongPtr(FindWindowA(NULL, "GAME WINDOW NAME"), GWL_WNDPROC, (LONG_PTR)WndProc);
//#endif
}

#endif // KIERO_INCLUDE_D3D9