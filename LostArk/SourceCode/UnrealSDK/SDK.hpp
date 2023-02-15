#pragma once

// Lost Ark (1.12.11.0) SDK


#pragma warning(disable:4099)

#include "SDK/LA_Basic.hpp"

#include "SDK/LA_Core_classes.hpp"
#include "SDK/LA_Engine_classes.hpp"
#include "SDK/LA_GameFramework_classes.hpp"
#include "SDK/LA_XAudio2_classes.hpp"
#include "SDK/LA_GFxUI_classes.hpp"
#include "SDK/LA_AkAudio_classes.hpp"
#include "SDK/LA_EFGame_classes.hpp"
#include "SDK/LA_WinDrv_classes.hpp"

#include <tchar.h>

namespace SDK
{
	///
	namespace offsets
	{
		/// <summary>
		/// Signature:  43 0F B7 3C 2A 41 B8 01 00 00 00 0F B7 D7
		/// Signature2: 44 0F B6 4A 1E 4C 63 42 18 - 0x3
		/// Text near: L"Code: 802" && L"Info" below this pointer
		/// Signature3: 48 8B 0D ? ? ? ? E8 ? ? ? ? EB 5A 48 63 47 1F
		/// Signature4: 45 33 C0 48 8D 15 ? ? ? ? 48 8B 0D ? ? ? ? E8 ? ? ? ? EB 5A 48 63 47 1F
		/// Screenshot: http://prntscr.com/tvo9yv
		/// </summary>
		constexpr auto kBase = 0x31FEE10; // 0x31FFD70;


		/// <summary>
		/// Signature:  48 C7 44 24 20 FE FF FF FF F3 0F 11 89 E8 00 00 00 - 0x3
		/// Signature3: 48 8B 0D ? ? ? ? 48 85 C9 75 36 8D 51 08 B9 ? ? ? ? FF 15 ? ? ? ? 48 89 44 24 ? 48 85 C0 74 0A 48 8B C8 E8 ? ? ? ? EB 03 49 8B C6 48 8D 48 44 48 8B 01 FF 50 08 48 8B 0D ? ? ? ?
		/// Signature4: FF 50 08 48 8B 0D ? ? ? ? 4C 8D A1 ? ? ? ?
		/// Text near: L"None"
		/// Screenshot: http://prntscr.com/tvof80
		/// </summary>
		constexpr auto kObjectBase = 0x3248BE8; //0x3249B48  //812

		/// <summary>
		/// Signature:  48 8B 96 24 04 00 00 48 85 D2 + 0x3
		/// Siganture2: 48 8B 0D ? ? ? ? 48 8B 96 ? ? ? ? 48 85 D2 0F 84 ? ? ? ? 48 83 C1 54 E8 ? ? ? ? 48 85 C0 0F 84 ? ? ? ? 48 8B C8
		/// Text near: 2x L"selectedIndex"(choose last one) && L"sys.secretdungeon.entry_fail_item_nomap" && L"sys.lightclient.failure_enter_zone"
		/// Screenshot: http://prntscr.com/tvoqga
		/// </summary>
		constexpr auto kEntityBase = 0x31FEE00;

		/// <summary>
		/// Signature: 40 57 48 83 EC 40 48 C7 44 24 20 FE FF FF FF 48 89 5C 24 58 33 FF 89 7C 24 28 - 0x3
		/// Signature2: 48 89 7C 24 50 33 FF 89 7C 24 40 - 0x14
		/// Signature3: 8B 05 ? ? ? ? 89 44 24 2C 48 8D 4C 24 ? FF 15 ? ? ? ? 48 89 7C 24 ?
		/// Text near: 2x &L"DUNGEON" && &L"ClassPreview"
		/// Screenshot: http://prntscr.com/tvp08a
		/// </summary>
		constexpr auto kMapBase = 0x320BBD8;

		/// <summary>
		/// Signature: 44 89 6C 24 20 45 33 C9 45 8B C7 48 8D 55 A0 + 0x3
		/// Signature2: 48 8B 0D ? ? ? ? 44 89 6C 24 ? 45 33 C9 45 8B C7 48 8D 55 A0 E8 ? ? ? ? 90 4C 8D 66 28
		/// Text near: L"L10N" && L"True" && "BoxTitleByItemName
		/// STR: "CEFUIWindow_NormalBox::SendDataProvider_ItemInfo"
		/// Screenshot: http://prntscr.com/tvp08a
		/// </summary>
		constexpr auto kNameBase = 0x31FEDE8; //0x31FFD48

	    /// <summary>
		/// Can be found via Xrefs of Function (call. 1st param).
		/// Signature: 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 85 C0 74 16 8B D7
		/// </summary>
		constexpr auto kSkillBase = 0x3204AC0; 

		/// <summary>
		/// Screenshot: http://prntscr.com/tv8mwt
		/// Text near: L"MaxCount" && L"PC" above this func
		/// Signature: 4C 0F BF 09 45 33 C0 4D 85 C9 7E 14 48 83 C1 02 38 11 74 0F 49 FF C0 48 83 C1 1E 4D 3B C1 7C F0 33 C0 C3
		/// Signatere2: 4C 0F BF 09 45 33 C0 4D 85 C9
		/// Signature3: E8 ? ? ? ? BA ? ? ? ? 48 8D 8D ? ? ? ? E8 ? ? ? ? 90 48 8D 15 ? ? ? ?
		/// Text in func: "ShipRepairDialog" && "sys.voyage.repair_desc_durability" && "sys.voyage.repair_desc" && "sys.voyage.repair_title"
		/// Func screenshot: http://prntscr.com/tv8nr3
		/// </summary>
		constexpr auto kGetMoneybyIdCall = 0x17FFC20;


		/// <summary>
		/// Signature: 48 C7 45 1F FE FF FF FF 48 89 58 08 48 89 70 10 48 89 78 18 45 8B F9 4C 8B E2 C7 45 E7 00 00 00 00 8B 15 16 CB EC 02 - 0x17
		/// Signature2: E8 ? ? ? ? 48 8B 44 24 ? 48 85 C0 74 04 0F B7 70 10 48 8B 4C 24 ? 48 85 C9 74 2A + 0x1
		/// __int64 __fastcall GetItemInfobyId(__int64 Base, __int64 unknown, __int64 item_id, unsigned int item_type, int unknown_1)
		/// Func screenshot: http://prntscr.com/tv8piw
		/// </summary>
		constexpr auto kGetItemInfobyIdCall = 0x03DA070;

		/// <summary>
		/// Text near: L"ZoneBase" && L"CCE5CCAAAE2E735C91FDC59206152A74" inside func
		/// Signature: 48 C7 45 C7 FE FF FF FF 48 89 58 10 48 89 70 18 48 89 78 20 48 8B 05 0E 74 D3 02 - 0x17
		/// Signature2: E8 ? ? ? ? 48 8B F0 41 8B FC
		/// Signature3: 48 8B C4 55 41 54 41 55 41 56 41 57 48 8D 68 A1 48 81 EC ? ? ? ? 48 C7 45 ? ? ? ? ? 48 89 58 10 48 89 70 18 48 89 78 20 48 8B 05 ? ? ? ? 48 33 C4 48 89 45 27 4C 8B E1 48 89 4D BF
		/// int __fastcall GetMapName(__int64 base)
		/// Func screenshot: http://prntscr.com/tv8ss0
		/// </summary>
		constexpr auto kGetMapnnameCall = 0x04B5000;// 0x04B6230;

		/// <summary>
		/// Text near: L"NaviMesh.epf" - above our func, so our func is third
		/// Signature: 40 55 56 57 41 54 41 55 41 56 41 57 48 8D 6C 24 F1 48 81 EC C0 00 00 00 48 C7 45 C7 FE FF FF FF 48 89 9C 24 08 01 00 00
		/// Signature2: 40 55 56 57 41 54 41 55 41 56 41 57 48 8D 6C 24 ? 48 81 EC ? ? ? ? 48 C7 45 ? ? ? ? ? 48 89 9C 24 ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 45 FF 66 44 89 4D ? 
		/// Signature3: 40 55 56 57 41 54 41 55 41 56 41 57 48 8D 6C 24 F1
		/// Signature4: E8 ? ? ? ? 66 89 45 B3
		/// Text near: L"Purchase Error : shopBuy Barter stackCount <= 0",
		/// "E:\\teamcity\\tripod_build_staging\\tripod_program\\Russia\\RURC\\Source\\Client\\Src\\EFGame\\Game\\EFShopManager.cpp"
		/// int __fastcall EquipRoleSlotItem(__int64 base, int some_enum?, unsigned int item_id?, __int16 unknown)
		/// Func screenshot: http://prntscr.com/tv8uyv
		/// </summary>
		constexpr auto kGetEquipRoleSlotCall = 0x17FFC20;

		/// <summary>
		/// Text near: '\n' inside func
		/// Signature: 48 8B C4 44 89 40 18 88 50 10 48 89 48 08 55 56 57 41 54 41 55 41 56 41 57
		/// Signature2: 48 8B C4 44 89 40 18 88 50 10 48 89 48 08
		/// Signature3: E8 ? ? ? ? E9 ? ? ? ? 48 8D 4D A0 E8 ? ? ? ? 90 48 8D 15 ? ? ? ? 48 8D 4D 80 E8 ? ? ? ? 90 41 B9 ? ? ? ? 4C 8D 45 80 33 D2 48 8D 4D A0 E8 ? ? ? ? 90 48 89 7D 88
		/// int __fastcall EquipItem(__int64 base, unsigned __int8 a2, int dwSlot, signed int Slot, int a5)
		/// Func screenshot: http://prntscr.com/tv8uyv
		/// </summary>
		constexpr auto kEquipItemCall = 0x17EC880;

		/// <summary>
		/// Signature: CC 48 89 5C 24 08 48 8B D9 8B 89 90 00 00 00 4C 8B DA 85 C9 + 0x1
		/// Signature2: 48 89 5C 24 ? 48 8B D9 8B 89 ? ? ? ? 4C 8B DA 85 C9 74 62 48 8B C2 4C 8D 8B ? ? ? ? 4D 8B 51 04 48 C1 F8 20 44 6B C0 17 44 03 C2 8D 41 FF 48 63 C8 33 C0 49 63 D0 48 23 D1 4D 85 D2 0F 94 C0
		/// Signature3: E8 ? ? ? ? 48 85 C0 74 0D B2 01
		/// Text near: L"fixedHP" && L"fixedPlayerPosX" && L"fixedPlayerPosY"
		/// __int64 __fastcall GetEntityBase(__int64 base, __int64 entity_id)
		/// Func screenshot: http://prntscr.com/tv901v
		/// Comment: +3F
		/// </summary>
		constexpr auto kGetHpBaseCall = 0x1807560;
		// see what is entitybase + 712

	    /// <summary>
		/// Signature: 48 8B C4 57 48 83 EC 70 48 C7 40 A8 FE FF FF FF 48 89 58 10 48 89 70 18 48 8B FA 48 8B D9 48 83 C1 30
		/// Signature2: 48 8B C4 57 48 83 EC 70 48 C7 40 ? ? ? ? ? 48 89 58 10 48 89 70 18 48 8B FA 48 8B D9 48 83 C1 30
		/// Signature3: E8 ? ? ? ? 85 C0 74 19 45 33 C9
		/// Signature4: 48 8D 56 1C 48 8B C8
		/// int __fastcall WalkToPosition(__int64 Struct0, __int64 Object)
		/// </summary>
		constexpr auto kWalkToPos = 0x1807560;
		// see what is entitybase + 712

	   //44 0F BE CB 89 7C 24 20 45 8B C4 48 8D 54 24 40
	}
	///

	///
	struct localplayer_info_t
	{
		localplayer_info_t() = default;

		int map_id;
		wchar_t wsz_map_name[32];
		FString map_name;

		std::uintptr_t object;
		std::uintptr_t object_id;

		wchar_t wsz_name[32];
		FString name;
		FVector position;

		int level;    //0x001E 
		int hp;       //0x0498 
		int max_hp;   //0x049C 
		int mp;       //0x04A0 
		int max_mp;   //0x04A4
		bool b_is_ride; //0x06C4

		int silver_coin;
		int gold_coin;
		int battle_coin;
		int lupheon_seal;
		int crack_piece;
		int cashew_seed;
		int pirate_coin;
		int pvp_token;
		int enhance_token;
	};

	struct skill_info_t
	{
		skill_info_t() = default;
		
		std::uintptr_t skill;
		std::uintptr_t skill_id;

		wchar_t wsz_name[80];
		FString name;
		
		int lvl;
		float cd_time;
		float max_cd_time;
	};

	struct inventory_info_t
	{
		inventory_info_t() = default;
		
		std::uintptr_t object;
		std::uintptr_t object_value;

		wchar_t wsz_name[32];
		FString name;
		
		int slot;
		int number;
		int item_id;
		int value_type;
		int type;
		int grade;
		int need_lvl;
		int item_lvl;
		int add_lvl;
		int last;
		int max_last;
		int equip_slot;
		bool b_is_can_use;
		bool b_is_decompose;
		bool b_is_trade;
	};
	///

	///
	struct global_names_t
	{
		int index;
		wchar_t name[0x20];
	};

	inline global_names_t g_pGlobalNames[] =
	{
		{ 0, L"inventoryBagTab" },
		{ 1, L"bt_disassembly" },
		{ 2, L"mainBag" },
		{ 3, L"bt_autosort" },
		{ 4, L"currencyMc" },
		{ 5, L"costMc" },
		{ 6, L"bt_quest" },
		{ 7, L"itemSortingGroup" },
		{ 8, L"petBag" },
		{ 9, L"petContinueBtn2" },
		{ 10, L"petMoveBtn2" },
		{ 11, L"petPeriodLabel" },
		{ 12, L"petContinueBtn" },
		{ 13, L"petMoveBtn" },
		{ 14, L"questBag" },
		{ 15, L"form_inventory1" },
	};
	///
	///
	///


	enum class GameKey : std::uint16_t {
		k0 = 17071,
		k1 = 17072,
		k2 = 17073,
		k3 = 17074,
		k5 = 17075,
		k6 = 17076,
		k7 = 17077,
		k9 = 17078,
		A = 17079,
		B = 17080,
		C = 17081,
		D = 17082,
		E = 17083,
		F = 17084,
		G = 17085,
		H = 17086,
		I = 17087,
		J = 17088,
		K = 17089,
		L = 17090,
		M = 17091,
		N = 17092,
		O = 17093,
		P = 17094,
		Q = 17095,
		R = 17096,
		S = 17097,
		T = 17098,
		W = 17099,
		F1 = 17100,
		F2 = 17101,
		F3 = 17102,
		F4 = 17103,
		F5 = 17104,
		F6 = 17105,
		F7 = 17106,
		F8 = 17107,
		F9 = 17108,
		F10 = 17109,
		F11 = 17110,
		F12 = 17111,
		Zero = 17112,
		One = 17113,
		Two = 17114,
		Three = 17115,
		Four = 17116,
		Five = 17117,
		Six = 17118,
		Seven = 17119,
		Eight = 17120,
		Nine = 17121,
		NumPadZero = 17122,
		NumPadOne = 17123,
		NumPadTwo = 17124,
		NumPadThree = 17125,
		NumPadFour = 17126,
		NumPadFive = 17127,
		NumPadSix = 17128,
		NumPadSeven = 17129,
		NumPadEight = 17130,
		NumPadNine = 17131,
		Multiply = 17132,
		Decimal = 17133,
		Divide = 17134,
		Backspace = 17135,
		Tab = 17136,
		Clear = 17137,
		Enter = 17138,
		LeftShift = 17139,
		LeftControl = 17140,
		LeftAlt = 17141,
		RightShift = 17142,
		RightControl = 17143,
		RightAlt = 17144,
		CapsLock = 17145,
		Escape = 17146,
		SpaceBar = 17147,
		PageUp = 17148,
		PageDown = 17149,
		End = 17150,
		Home = 17151,
		Left = 17152,
		Up = 17153,
		Right = 17154,
		Down = 17155,
		NumLock = 17156,
		ScrollLock = 17157,
		Semicolon = 17158,
		Equal = 17159,
		Comma = 17160,
		Underscore = 17161,
		Period = 17162,
		Slash = 17163,
		Bar = 17164,
		LeftBracket = 17165,
		Backslash = 17166,
		RightBracket = 17167,
		Quote = 17168,
		LeftMouseButton = 17169,
		RightMouseButton = 17170,
		MiddleMouseButton = 17171,
		MouseScrollUp = 17172,
		MouseScrollDown = 17173,
		GAMEPAD_RS_Up = 17174,
		GAMEPAD_RS_Down = 17175,
		GAMEPAD_RS_Left = 17176,
		GAMEPAD_RS_Right = 17177,
		GAMEPAD_A = 17178,
		GAMEPAD_B = 17179,
		GAMEPAD_X = 17180,
		GAMEPAD_Y = 17181,
		GAMEPAD_L1 = 17182,
		GAMEPAD_L2 = 17183,
		GAMEPAD_L3 = 17184,
		GAMEPAD_R1 = 17185,
		GAMEPAD_R2 = 17186,
		GAMEPAD_R3 = 17187,
		BACK = 17188
	};

	struct MapData_t
	{
		char pad_0x0000[0x10]; //0x0000
		std::uintptr_t base; //0x0010 
		int max_count; //0x0018 
	};

	struct CurMapData_t
	{
		char pad_0x0000[0x8]; //0x0000
		int index; //0x0008 
		char pad_0x000C[0x34]; //0x000C
		TCHAR name[0x20]; //0x0040 
	};//Size=0x0060
	///
	///
	///

	///
	///
	///
	extern std::string            g_pWorldName;
	extern FVector2D* g_pViewportSize;
	extern ULevel* g_pPersistentLevel;
	extern ULocalPlayer* g_pLocalPlayer;
	extern UWorld* g_pWorld;
	extern UEFGameEngine* g_pEngine;
	extern UEFGameViewportClient* g_pViewport;
	extern UCanvas* g_pCanvas;
	extern AEFPlayerController* g_pPC;
	extern APawn* g_pLocalPlayerPawn;
	extern AHUD* g_pHUD;
	extern localplayer_info_t     g_pLocalPlayer_info;
	extern skill_info_t           g_pLocalPlayer_skill[8];

	///
	extern float                  g_pTickDeltaTime;

	//
	namespace colors
	{
		extern FColor                  empty;
		extern FColor                  white;
		extern FColor                  black;
		extern FColor                  red;
		extern FColor                  darker_red;
		extern FColor                  blue;
		extern FColor                  green;
		extern FColor                  darker_green;
		extern FColor                  yellow;
		extern FColor                  cyan;
	}

	///
	UEFGameViewportClient* GetViewport();
	UEFGameEngine* GetEngine();
	ULocalPlayer* GetLocalPlayer();

	///
	bool IsVehicle(AEFPawn* pEntity);
	bool IsNpc(AEFPawn* pEntity);
	bool IsSummonNpc(AEFPawn* pEntity);
	bool IsPet(AEFPawn* pEntity);
	bool IsPlayer(AEFPawn* pEntity);
	bool IsMonster(AEFPawn* pEntity);

	bool IsLocalVehicle(void);

	///
	bool IsVisible(AEFPawn* pEntity);
	bool IsVisibleOnScreen(UCanvas* pCanvas, FVector Vector);

	///
	float GetDistance(FVector from, FVector to);
	float Get3DDistance(APawn* from, AEFPawn* to);

	///
	void GetWorldName(void);
	int  GetGameStatus(void);

	/// <summary>
	/// Lost Ark engine function to pressing any key.
	/// </summary>
	/// <param name="FName">Index of Key to press</param>
	/// <param name="input_event">Pressed|Released|Repeat and etc</param>
	/// <param name="amount_depressed">??</param>
	/// <returns></returns>
	auto InputKey(const FName& key, const EInputEvent& input_event) -> bool;

	/// <summary>
	/// Lost Ark does have unique objects for handling sensitive information about entities;
	///
	///

	/// <summary>
	/// GetGameBase - gives us game base address and adds to it ui_addr;
	/// </summary>
	/// <param name="ui_addr"> Pointer add to game base </param>
	/// <returns> (ui_addr + lostark.exe) pointer </returns>
	auto GetGameBase(const std::uintptr_t ui_addr)->std::uintptr_t;

	/// <summary>
	/// GetPlayerInfo - gives us local player info data pointer (For example can be used for get local player hp);
	/// </summary>
	/// <param name="ui_index">Index to local player</param>
	/// <returns> (local player data) pointer </returns>
	auto GetPlayerInfo(const std::uintptr_t ui_index)->std::uintptr_t;

	/// <summary>
	/// GetLevelInfo - gives us current level data pointer (For example can be used for get current level);
	/// </summary>
	/// <param name="none"> none </param>
	/// <returns> (current level data) pointer </returns>
	auto GetLevelInfo(void)->std::uintptr_t;

	/// <summary>
	/// GetEntityInfo - gives us entity info data pointer;
	/// </summary>
	/// <param name="none"> none </param>
	/// <returns> (entity info data) pointer </returns>
	auto GetEntityInfo(void)->std::uintptr_t;

	auto GetMoneyByIndex(const EMoneyType e_money_type) -> int;
	auto GetMapNameByIndex(const int& i_map_id) -> wchar_t*;
	///
	/// </summary>

	///
	enum EFuncId : std::int32_t
	{
		kEfGameEfgFxHudWrapperPostRender = 41542,            /// EFGame.EFGFxHUDWrapper.PostRender
		kEfGameEfGameViewportClientPostRender = 41463,       /// EFGame.EFGameViewportClient.PostRender
		kEfGameEfGameViewportClientTick = 41465,             /// EFGame.EFGameViewportClient.Tick
		kEngineGameViewportClientLayoutPlayers = 19948,      /// Engine.GameViewportClient.LayoutPlayers
		kEfGameEfgFxHudWrapperDestroyed = 41430,             /// EFGame.EFGFxHUDWrapper.Destroyed
		kEnginePlayerControllerDestroyed = 44344,            /// Engine.PlayerController.Destroyed
		kEnginePlayerControllerGetFovAngle = 4416,           /// Engine.PlayerController.GetFOVAngle
		kEfGameEfPlayerControllerPlayerTick = 44490,         /// EFGame.EFPlayerController.PlayerTick
		kEngineGameViewportClientGetViewportSize = 19850,    /// Engine.GameViewportClient.GetViewportSize
		kEngineGameViewportClientGetMousePosition = 19865,  /// Engine.GameViewportClient.GetMousePosition
		kEfGameEfPlayerControllerGetPlayerViewPoint = 44505  /// EFGame.EFPlayerController.GetPlayerViewPoint
	};

	///
	enum ETagId : std::int32_t
	{
		kPlayer = 15163,
		kMonster = 15157,
		kNpc = 15159,
		kSummonNpc = 15165,
		kPet = 15161,
		kVehicle = 15171
	};
}
