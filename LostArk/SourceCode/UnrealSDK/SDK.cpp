#include "SDK.hpp"
#include "Vector.h"
#include "../Utils/FindPattern.h"
#include "../Utils/Tools.h"
#include "../Utils/Log.h"

/// Bone Info
//SDK::FName fnRoot = SDK::FName("B_Root");
//SDK::FName fnHead = SDK::FName("Bip001-Head");
//SDK::FName fnNeck = SDK::FName("Bip001-Neck");
//SDK::FName fnSpine = SDK::FName("Bip001-Spine");
//SDK::FName fnSpine1 = SDK::FName("Bip001-Spine1");
//SDK::FName fnSpine2 = SDK::FName("Bip001-Spine2");
//SDK::FName fnPelvis = SDK::FName("Bip001-Pelvis");
//
//SDK::FName fnLArmUp = SDK::FName("Bip001-L-UpperArm");
//SDK::FName fnLShoulder = SDK::FName("Bip001-L-Clavicle");
//SDK::FName fnLArmLow = SDK::FName("Bip001-L-Forearm");
//SDK::FName fnLHand = SDK::FName("Bip001-L-Hand");
//SDK::FName fnLLegUp = SDK::FName("bip001-l-thigh");
//SDK::FName fnLLegLow = SDK::FName("bip001-l-calf");
//SDK::FName fnLFoot = SDK::FName("bip001-l-foot");
//
//SDK::FName fnRArmUp = SDK::FName("Bip001-R-UpperArm");
//SDK::FName fnRShoulder = SDK::FName("Bip001-R-Clavicle");
//SDK::FName fnRArmLow = SDK::FName("Bip001-R-Forearm");
//SDK::FName fnRHand = SDK::FName("Bip001-R-Hand");
//SDK::FName fnRLegUp = SDK::FName("bip001-r-thigh");
//SDK::FName fnRLegLow = SDK::FName("bip001-r-calf");
//SDK::FName fnRFoot = SDK::FName("bip001-r-foot");

namespace SDK
{
	std::string             g_pWorldName{ "none" };
	FVector2D* g_pViewportSize{ };
	ULevel* g_pPersistentLevel = nullptr;
	ULocalPlayer* g_pLocalPlayer = nullptr;
	UWorld* g_pWorld = nullptr;
	UEFGameEngine* g_pEngine = nullptr;
	UEFGameViewportClient* g_pViewport = nullptr;
	UCanvas* g_pCanvas = nullptr;
	AEFPlayerController* g_pPC = nullptr;
	APawn* g_pLocalPlayerPawn = nullptr;
	AHUD* g_pHUD = nullptr;
	localplayer_info_t      g_pLocalPlayer_info = { };
	skill_info_t           g_pLocalPlayer_skill[8] = { };

	float                       g_pTickDeltaTime = 0;

	namespace colors
	{
		FColor                      empty{ 0, 0, 0, 0 };
		FColor                      white{ 255, 255, 255, 255 };
		FColor                      black{ 0, 0, 0, 255 };
		FColor                      red{ 255, 0, 0, 99 };
		FColor                      darker_red{ 153,0,0, 99 };
		FColor                      blue{ 0, 0, 255, 255 };
		FColor                      green{ 0, 255, 0, 255 };
		FColor                      darker_green{ 0, 102, 0, 255 };
		FColor                      yellow{ 255, 255, 0, 99 };
		FColor                      cyan(0, 128, 255, 99);
	}

	UCanvas* GetCanvas()
	{
		return UObject::FindObject<UCanvas>("Canvas Transient.CanvasObject");
	}

	UEFGameViewportClient* GetViewport()
	{
		return UObject::FindObject<UEFGameViewportClient>("EFGameViewportClient Transient.EFGameEngine_1.EFGameViewportClient_1");
	}

	UEFGameEngine* GetEngine()
	{
		return UObject::FindObject<UEFGameEngine>("EFGameEngine Transient.EFGameEngine_1");
	}

	ULocalPlayer* GetLocalPlayer()
	{
		return UObject::FindObject<ULocalPlayer>("LocalPlayer Transient.EFGameEngine_1.LocalPlayer_1");
	}

	bool IsVehicle(AEFPawn* pEntity)
	{
		return (pEntity->Tag.Index == ETagId::kVehicle);
	}

	bool IsNpc(AEFPawn* pEntity)
	{
		return (pEntity->Tag.Index == ETagId::kNpc);
	}

	bool IsSummonNpc(AEFPawn* pEntity)
	{
		return (pEntity->Tag.Index == ETagId::kSummonNpc);
	}

	bool IsPet(AEFPawn* pEntity)
	{
		return (pEntity->Tag.Index == ETagId::kPet);
	}

	bool IsPlayer(AEFPawn* pEntity)
	{
		return (pEntity->Tag.Index == ETagId::kPlayer);
	}

	bool IsMonster(AEFPawn* pEntity)
	{
		return (pEntity->Tag.Index == ETagId::kMonster);
	}

	bool IsLocalVehicle(void)
	{
		return IsVehicle(static_cast<AEFPawn*>(g_pLocalPlayerPawn));
	}

	bool IsVisible(AEFPawn* pEntity)
	{
		if (g_pLocalPlayer == nullptr)
			return false;

		if (g_pPC == nullptr)
			return false;

		if (g_pLocalPlayerPawn == nullptr)
			return false;

		if (g_pLocalPlayerPawn->LineOfSightTo(pEntity))
			return true;

		return false;
	}

	bool IsVisibleOnScreen(UCanvas* pCanvas, FVector Vector)
	{
		return Vector.Z > 0.f && Vector.Z < 1.f && Vector.X < pCanvas->ClipX && Vector.X > 0 && Vector.Y < pCanvas->ClipY && Vector.Y > 0;
	}

	float GetDistance(FVector from, FVector to)
	{
		Vector angle;
		angle.x = to.X - from.X;
		angle.y = to.Y - from.Y;
		angle.z = to.Z - from.Z;

		return sqrt(angle.x * angle.x + angle.y * angle.y + angle.z * angle.z);
	}

	float Get3DDistance(APawn* from, AEFPawn* to)
	{
		return sqrt
		(pow(to->Location.X - from->Location.X, 2)
			+ pow(to->Location.Y - from->Location.Y, 2)
			+ pow(to->Location.Z - from->Location.Z, 2));
	}

	void GetWorldName(void)
	{
		if (g_pWorld == nullptr)
		{
			g_pWorld = (*g_world);
			if (g_pWorld == nullptr)
				return;
		}

		g_pWorldName.clear();
		g_pWorldName = g_pWorld->GetFullName().c_str();
	}

	int GetGameStatus(void)
	{
		if (g_pWorldName.empty())
			return 10;

		if (SDK::g_pWorldName.find("lv_eflobby_ps") != std::string::npos)
			return 1; /// Lobby

		if (SDK::g_pWorldName.find("LookInfoMAT") != std::string::npos)
			return 2; /// Loading

		return 0;
	}

	auto InputKey(const FName& key, const EInputEvent& input_event) -> bool
	{
		// OnReceivedNativeInputKey ?
		using Fn = bool(__thiscall*)(UInteraction* /* interaction */, int /* i_controller_id */, FName /* i_index */, EInputEvent /* e_input_even */, float /* f_amount_depressed */, bool /* b_game_pad */);

		if (g_pPC == nullptr)
			return false;

		if (g_pPC->Interactions.Num() < 1)
			return false;

		if (g_pPC->EFPlayerInput == nullptr)
			return false;

		static auto ui_interaction = *reinterpret_cast<std::uintptr_t*>(g_pPC->Interactions[0]);
		static auto ui_func = *reinterpret_cast<std::uintptr_t*>(ui_interaction + 0x270);
		static auto procedure = reinterpret_cast<Fn>(ui_func);

		return procedure(g_pPC->EFPlayerInput, 0, key, TEnumAsByte<EInputEvent>(input_event), 1.f, false);
	}

	auto GetGameBase(const std::uintptr_t u_addr) -> std::uintptr_t
	{
		std::uintptr_t u_ret = 0;

		const auto* h_module = GetModuleHandleA(nullptr);

		if (h_module)
		{
			u_ret = u_addr + reinterpret_cast<std::uintptr_t>(h_module);
		}

		return u_ret;
	}

	auto GetPlayerInfo(const std::uintptr_t ui_index) -> std::uintptr_t
	{
		std::uintptr_t ui_ret = 0;

		auto u_base = GetGameBase(offsets::kBase);

		if (u_base
			&& Tools::can_read_ptr(reinterpret_cast<void*>(u_base)))
		{
			u_base = *reinterpret_cast<std::uintptr_t*>(u_base);
			if (u_base
				&& Tools::can_read_ptr(reinterpret_cast<void*>(u_base)))
			{
				u_base = *reinterpret_cast<std::uintptr_t*>(u_base + 0x5C);

				if (u_base
					&& Tools::can_read_ptr(reinterpret_cast<void*>(u_base)))
				{
					u_base = *reinterpret_cast<std::uintptr_t*>(u_base + 0x2F8);

					if (u_base
						&& Tools::can_read_ptr(reinterpret_cast<void*>(u_base)))
					{
						ui_ret = *reinterpret_cast<std::uintptr_t*>(u_base + (ui_index * 5) * 4 + 4);
					}
				}
			}
		}

		return ui_ret;
	}


	auto GetEntityInfo(void) -> std::uintptr_t
	{
		std::uintptr_t ui_ret = 0;
		auto u_base = GetGameBase(offsets::kEntityBase);

		if (u_base
			&& Tools::can_read_ptr(reinterpret_cast<void*>(u_base)))
		{
			u_base = *reinterpret_cast<std::uintptr_t*>(u_base);

			if (u_base
				&& Tools::can_read_ptr(reinterpret_cast<void*>(u_base)))
			{
				u_base = *reinterpret_cast<std::uintptr_t*>(u_base + 0x440);

				if (u_base
					&& Tools::can_read_ptr(reinterpret_cast<void*>(u_base)))
				{
					ui_ret = *reinterpret_cast<std::uintptr_t*>(u_base + 0x18);
				}
			}
		}
		return ui_ret;
	}

	auto GetLevelInfo(void) -> std::uintptr_t
	{
		std::uintptr_t ui_ret = 0;

		auto u_base = GetGameBase(offsets::kObjectBase);

		if (u_base
			&& Tools::can_read_ptr(reinterpret_cast<void*>(u_base)))
		{
			u_base = *reinterpret_cast<std::uintptr_t*>(u_base);

			if (u_base
				&& Tools::can_read_ptr(reinterpret_cast<void*>(u_base)))
			{
				ui_ret = *reinterpret_cast<std::uintptr_t*>(u_base + 0x94);

				std::cout << (u_base + 1644) << '\n';
			}
		}

		return ui_ret;
	}

	auto GetMoneyByIndex(const EMoneyType e_money_type) -> int
	{
		using Fn = int(__thiscall*)(std::uintptr_t /*kObjectBase*/, EMoneyType /*Money type*/);

		auto i_result = 0;

		auto u_base = GetGameBase(offsets::kObjectBase);

		static auto ui_call_addr = PatternScan(std::string(), "4C 0F BF 09 45 33 C0 4D 85 C9");
		static auto procedure = reinterpret_cast<Fn>(ui_call_addr);

		/// Call it;
		if (u_base
			&& ui_call_addr
			&& Tools::can_read_ptr(reinterpret_cast<void*>(u_base))
			&& Tools::can_read_ptr(reinterpret_cast<void*>(ui_call_addr)))
		{
			u_base = *reinterpret_cast<std::uintptr_t*>(u_base);

			if (u_base
				&& Tools::can_read_ptr(reinterpret_cast<void*>(u_base)))
			{
				const auto localplayer_money_data = u_base + 208;

				if (u_base == -208)
					return i_result;

				/*u_base += 26;*/

				if (localplayer_money_data
					&& Tools::can_read_ptr(reinterpret_cast<void*>(localplayer_money_data)))
				{
					i_result = procedure(localplayer_money_data, TEnumAsByte<EMoneyType>(e_money_type));
				}
			}
		}

		return i_result;
	}

	auto GetMapNameByIndex(const int& i_map_id) -> wchar_t*
	{
		using Fn = std::uintptr_t(__fastcall*)(std::uintptr_t /* kNameBase */);

		wchar_t* wsz_ret = { };

		const auto u_call_base = GetGameBase(offsets::kNameBase);

		// Scan for the signature
		static auto u_address = reinterpret_cast<std::uintptr_t>(PatternScan(std::string(), "E8 ? ? ? ? 48 8B F0 41 8B FC"));
		static auto ui_address = Tools::calculate_call(u_address);
		
		static const auto u_call_addr = GetGameBase(ui_address);
		static auto procedure = reinterpret_cast<Fn>(u_call_addr);

		if (!ui_address
			&& !u_call_base
			&& !Tools::can_read_ptr(reinterpret_cast<void*> (ui_address))
			&& !Tools::can_read_ptr(reinterpret_cast<void*> (u_call_base)))
			return wsz_ret;

		const auto u_base = *reinterpret_cast<std::uintptr_t*> (u_call_base);
		if (!u_base
			&& Tools::can_read_ptr(reinterpret_cast<void*> (u_base)))
			return wsz_ret;

		auto u_map_base = procedure(u_base);

		if (!u_map_base
			&& !Tools::can_read_ptr(reinterpret_cast<void*> (u_call_base)))
			return wsz_ret;

		const auto i_max_count = *reinterpret_cast<int*> (u_map_base + 0x18);
		u_map_base = *reinterpret_cast<std::uint64_t*> (u_map_base + 0x10);

		if (i_max_count <= 1)
			return wsz_ret;

		if (!u_map_base
			&& !Tools::can_read_ptr(reinterpret_cast<void*> (u_call_base)))
			return wsz_ret;

		for (auto i = 0; i <= i_max_count; i++)
		{
			auto u_map_data = *reinterpret_cast<std::uint64_t*> (u_map_base + 0x10 * i);

			if (!u_map_data
				&& !Tools::can_read_ptr(reinterpret_cast<void*> (u_map_data)))
				continue;

			auto i_cur_id = *reinterpret_cast<int*> (u_map_data + 0x8);

			if (i_map_id == i_cur_id)
			{
				const auto wsz_name = *reinterpret_cast<wchar_t**>(u_map_data + 0x40);

				if (wsz_name
					&& Tools::can_read_ptr(reinterpret_cast<void*> (wsz_name))) {
					auto* const result = std::wcscpy(wsz_ret, wsz_name);

					if (std::wcslen(result) == 0) {
						std::printf("cant copy map name to ret!\n");
					}
				}

				break;
			}
		}

		return wsz_ret;
	}

	auto WalkToPosition(const FVector& fv_position) -> int
	{
		using Fn = int(__fastcall*)(BYTE* /* buff data */, std::uintptr_t /* kObject */);

		BYTE buff[0x100] = { 0 };
		memset(buff, 0, 0x100);

		int i_ret = { 0 };

		static const auto call_opcode_address = reinterpret_cast<std::uintptr_t>(PatternScan(std::string(), "E8 ? ? ? ? 48 8B F0 41 8B FC"));
		static const auto relative_call_address = *reinterpret_cast<std::intptr_t*>(call_opcode_address + 1);
		static const auto real_function_address = call_opcode_address + 1 /*Skip E8 opcode*/ + sizeof(std::int32_t) /*Skip bytes of relative address*/ + relative_call_address;

		if (!real_function_address
			&& !Tools::can_read_ptr(reinterpret_cast<void*> (real_function_address)))
			return i_ret;

		static auto procedure = reinterpret_cast<Fn>(real_function_address);

		if (!procedure
			&& !Tools::can_read_ptr(reinterpret_cast<void*> (procedure)))
			return i_ret;

		if (g_pLocalPlayer_info.hp > 1)
		{
			*reinterpret_cast<std::uintptr_t*>(buff) = 0x42C0; // walking data? #17088
			*reinterpret_cast<int*>(buff + 4) = 1;

			*reinterpret_cast<float*>(buff + 0x30) = g_pLocalPlayer_info.position.X;
			*reinterpret_cast<float*>(buff + 0x34) = g_pLocalPlayer_info.position.Y;
			*reinterpret_cast<float*>(buff + 0x38) = g_pLocalPlayer_info.position.Z;

			*reinterpret_cast<float*>(buff + 0x3C) = fv_position.X;
			*reinterpret_cast<float*>(buff + 0x40) = fv_position.Y;
			*reinterpret_cast<float*>(buff + 0x44) = fv_position.Z;

			const auto object = g_pLocalPlayer_info.object + 0xF4;

			i_ret = procedure(buff, object);
		}

		return i_ret;
	}

	auto GetSkillNamebyIndex(const int& skill_id, const int& skill_lvl) -> std::uintptr_t
	{
		// __int64 __fastcall GetSkillNamebyIndex(int base, __int64 buff, __int64 skill_id, unsigned int skill_lvl, int unknown)
		using Fn = std::uintptr_t(__fastcall*)(std::uintptr_t /* kNameBase */, DWORD64* /* buff */, int /* skill_id */, int /* skill_lvl */, int /* unknown*/);
		std::uintptr_t ui_result = 0;
		DWORD64 buff[3] = { 0 };

		static auto u_base = GetGameBase(offsets::kNameBase);

		if (!u_base
			&& !Tools::can_read_ptr(reinterpret_cast<void*> (u_base)))
			return ui_result;

		static auto ui_call_addr = GetGameBase(0x16DCF0);

		if (!ui_call_addr
			&& !Tools::can_read_ptr(reinterpret_cast<void*> (ui_call_addr)))
			return ui_result;

		static auto procedure = reinterpret_cast<Fn>(ui_call_addr);

		if (!procedure
			&& !Tools::can_read_ptr(reinterpret_cast<void*> (procedure)))
			return ui_result;

		auto result = procedure(u_base, buff, skill_id, skill_lvl, 0);

		ui_result = buff[0];
		return ui_result;
	}

	auto GetSkillInfobyIndex(const int& skill_id) -> std::uintptr_t
	{
		using Fn = std::uintptr_t(__fastcall*)(std::uintptr_t /* kSkillBase */, int /* skill_id */, int /* unknown*/);

		std::uintptr_t ui_result = 0;

		static auto u_base = GetGameBase(offsets::kSkillBase);

		if (!u_base
			&& !Tools::can_read_ptr(reinterpret_cast<void*> (u_base)))
			return ui_result;

		static auto* ui_call_addr = PatternScan(std::string(), "48 8B C4 57 48 83 EC 70 48 C7 40 ? ? ? ? ? 48 89 58 10 48 89 70 18 48 8B FA 48 8B D9 48 83 C1 30");

		if (!ui_call_addr
			&& !Tools::can_read_ptr(reinterpret_cast<void*> (ui_call_addr)))
			return ui_result;

		static auto procedure = reinterpret_cast<Fn>(ui_call_addr);

		if (!procedure
			&& !Tools::can_read_ptr(reinterpret_cast<void*> (procedure)))
			return ui_result;

		ui_result = procedure(u_base, skill_id, 0);
		return ui_result;
	}

	auto UpdateSkillData(void) -> void
	{
		auto u_base = GetGameBase(offsets::kObjectBase);

		if (!u_base
			&& !Tools::can_read_ptr(reinterpret_cast<void*> (u_base)))
			return;

		u_base = *reinterpret_cast<std::uintptr_t*> (u_base + 0x94);

		if (!u_base
			&& !Tools::can_read_ptr(reinterpret_cast<void*> (u_base)))
			return;

		u_base = *reinterpret_cast<std::uintptr_t*> (u_base + 0xAF3A8);

		if (!u_base
			&& !Tools::can_read_ptr(reinterpret_cast<void*> (u_base)))
			return;

		auto ui_head = *reinterpret_cast<std::uintptr_t*> (u_base + 0x97F);

		if (!ui_head
			&& !Tools::can_read_ptr(reinterpret_cast<void*> (ui_head)))
			return;

		std::uintptr_t ui_id = 0;
		
		for (auto& i : g_pLocalPlayer_skill) {
			ui_id = *reinterpret_cast<std::uintptr_t*> (ui_head + 1);

			if (ui_id != 0)
			{
				i.skill_id = ui_id;

				const auto ui_skill_info = GetSkillInfobyIndex(ui_id);
				
				if (!ui_skill_info
					&& !Tools::can_read_ptr(reinterpret_cast<void*> (ui_skill_info)))
					continue;

				auto i_lvl = *reinterpret_cast<int*> (ui_skill_info + 4);

				i.lvl = i_lvl;
				i.cd_time = *reinterpret_cast<float*> (ui_skill_info + 8);
				i.max_cd_time = *reinterpret_cast<float*> (ui_skill_info + 20);

				const auto ui_name_info = GetSkillNamebyIndex(ui_id, i_lvl);
				if (!ui_name_info
					&& !Tools::can_read_ptr(reinterpret_cast<void*> (ui_name_info)))
					continue;

				auto* const wsz_name = *reinterpret_cast<wchar_t**>(ui_name_info + 0x10);

				auto* const result = std::wcscpy(i.wsz_name, wsz_name);

				const auto i_len = std::wcslen(result);
				if (i_len == 0) {
					std::printf("cant copy cur map name to struct!\n");

					wcscpy_s(i.wsz_name, L"unknown");
				}
				else {
					i.name = result;
				}
			}
			
			ui_head = *reinterpret_cast<std::uintptr_t*> (ui_head + 5);
		}
	}
}