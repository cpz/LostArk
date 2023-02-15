#include "Hooks.h"
#include "../Utils/Detours.h"

#include "../UnrealSDK/SDK/LA_EFGame_parameters.hpp"
#include "../UnrealSDK/Render.h"
#include "../UnrealSDK/Vector.h"

#include "../Functions/Aimbot.h"
#include "../Functions/ESP.h"

#include <atlstr.h>

#include "../Utils/Log.h"
#include "../Utils/Tools.h"

std::vector<SDK::AEFPawn*> entities;
std::vector<SDK::AEFProjectile*> projectiles;
std::vector<SDK::AEFSkeletalMeshActor*> resources;
std::vector<SDK::AEFItem*> items;
//std::vector<SDK::AActor*> DebugActor;

std::mutex entity_mutex;

std::atomic_bool is_destroyed(false);
std::atomic_int skip_ticks = 0;

auto GetLocalPlayerInfo( void ) -> void
{
	const auto u_level_base = SDK::GetLevelInfo();
	const auto u_localplayer_base = SDK::GetPlayerInfo(0x41);
	const auto u_entity_base = SDK::GetEntityInfo();
	const auto u_get_current_map_id = SDK::GetGameBase(SDK::offsets::kMapBase);

	dbg("trying to read u_level_base");
	if (u_level_base
		&& Tools::can_read_ptr( reinterpret_cast<void*>( u_level_base ) ) )
	{
		dbg("reading current level");

		const auto level = *reinterpret_cast<int*>( u_level_base + 0x1E );
		if(level < 0 || level > 50) {
			SDK::g_pLocalPlayer_info.level = 1;
		} else {
			SDK::g_pLocalPlayer_info.level = level;
		}
		
		/*std::cout << *(DWORD*)(u_level_base + 144));
		std::cout << *(std::uintptr_t*)(u_level_base + 76));*/

		/*auto p_unknown = *(std::uintptr_t*)(u_level_base + 148);
		std::cout << p_unknown);
		std::cout << *(std::uintptr_t*)(p_unknown + 16));*/
	}

	dbg("trying to read u_localplayer_base");
	if (u_localplayer_base
		&& Tools::can_read_ptr( reinterpret_cast<void*>( u_localplayer_base ) ) )
	{
		dbg("reading local player data");
		
		SDK::g_pLocalPlayer_info.max_hp  = *reinterpret_cast<int*>( u_localplayer_base + 0x498 );
		SDK::g_pLocalPlayer_info.hp      = *reinterpret_cast<int*>( u_localplayer_base + 0x49C );
		SDK::g_pLocalPlayer_info.max_mp  = *reinterpret_cast<int*>( u_localplayer_base + 0x4A0 );
		SDK::g_pLocalPlayer_info.mp      = *reinterpret_cast<int*>( u_localplayer_base + 0x4A4 );
		SDK::g_pLocalPlayer_info.b_is_ride = *reinterpret_cast<bool*>( u_localplayer_base + 0x06C4 );

		dbg("reading coins data");
		
		SDK::g_pLocalPlayer_info.silver_coin   = GetMoneyByIndex( SDK::EMoneyType::MONEY_TYPE_GOLD );
		SDK::g_pLocalPlayer_info.gold_coin     = GetMoneyByIndex( SDK::EMoneyType::MONEY_TYPE_TRADE_GOLD );
		SDK::g_pLocalPlayer_info.battle_coin   = GetMoneyByIndex( SDK::EMoneyType::MONEY_TYPE_BATTLE_COIN );
		SDK::g_pLocalPlayer_info.cashew_seed   = GetMoneyByIndex( SDK::EMoneyType::MONEY_TYPE_CASHEW_SEED );
		SDK::g_pLocalPlayer_info.lupheon_seal  = GetMoneyByIndex( SDK::EMoneyType::MONEY_TYPE_LUPHEON_SEAL );
		SDK::g_pLocalPlayer_info.crack_piece   = GetMoneyByIndex( SDK::EMoneyType::MONEY_TYPE_CRACK_PIECE );
		SDK::g_pLocalPlayer_info.pirate_coin   = GetMoneyByIndex( SDK::EMoneyType::MONEY_TYPE_PIRATE_COIN );
		SDK::g_pLocalPlayer_info.pvp_token     = GetMoneyByIndex( SDK::EMoneyType::MONEY_TYPE_PVP_TOKEN );
		SDK::g_pLocalPlayer_info.enhance_token = GetMoneyByIndex( SDK::EMoneyType::MONEY_TYPE_ENHANCE_TOKEN );

		dbg(SDK::g_pLocalPlayer_info.silver_coin);
		dbg(SDK::g_pLocalPlayer_info.gold_coin);
		dbg(SDK::g_pLocalPlayer_info.battle_coin);
		dbg(SDK::g_pLocalPlayer_info.cashew_seed);
		dbg(SDK::g_pLocalPlayer_info.lupheon_seal);
		dbg(SDK::g_pLocalPlayer_info.crack_piece);
		dbg(SDK::g_pLocalPlayer_info.pirate_coin);
		dbg(SDK::g_pLocalPlayer_info.pvp_token);
		dbg(SDK::g_pLocalPlayer_info.enhance_token);

	}

	//std::cout << "trying to read u_entity_base");
	if (u_entity_base
		&& Tools::can_read_ptr( reinterpret_cast<void*>( u_entity_base ) ) )
	{
		SDK::g_pLocalPlayer_info.object = u_entity_base;
		SDK::g_pLocalPlayer_info.object_id = *reinterpret_cast<std::uintptr_t*>( u_entity_base + 0x14 );
		
		const auto u_name_addr = *reinterpret_cast<std::uintptr_t*>( u_entity_base + 0x1C );
		if (u_name_addr 
			&& Tools::can_read_ptr( reinterpret_cast<void*>( u_name_addr ) ))
		{
			auto* const result = std::wcscpy(SDK::g_pLocalPlayer_info.wsz_name, reinterpret_cast<wchar_t*>( u_name_addr ) );

			dbg( reinterpret_cast<wchar_t**>(u_name_addr) );
			
			if (std::wcslen(result) == 0) {
				dbg("cant copy local player name to struct!");

				wcscpy_s(SDK::g_pLocalPlayer_info.wsz_name, L"none");
			}
			else {
				SDK::g_pLocalPlayer_info.name = result;
			}
			
		}

		dbg("trying to read u_entity_base_vec");
		const auto u_entity_base_vec = *reinterpret_cast<std::uintptr_t*>( u_entity_base + 0x90 );

		if (u_entity_base_vec 
			&& Tools::can_read_ptr( reinterpret_cast<void*>( u_entity_base_vec ) ) )
		{
			const auto u_base_vec = u_entity_base_vec + 0x7C;
			SDK::g_pLocalPlayer_info.position = *reinterpret_cast<SDK::FVector*>(u_base_vec);
			
			/*SDK::g_pLocalPlayer_info.position.X = *reinterpret_cast<float*>( u_entity_base_vec + 0x7C );
			SDK::g_pLocalPlayer_info.position.Y = *reinterpret_cast<float*>( u_entity_base_vec + 0x80 );
			SDK::g_pLocalPlayer_info.position.Z = *reinterpret_cast<float*>( u_entity_base_vec + 0x84 );*/
		}
	}
	
	if ( u_get_current_map_id )
	{
		SDK::g_pLocalPlayer_info.map_id = *reinterpret_cast<int*>(u_get_current_map_id);
		dbg(SDK::g_pLocalPlayer_info.map_id);

		/*auto* const wsz_name = SDK::GetMapNameByIndex( SDK::g_pLocalPlayer_info.map_id );
		auto* const result = std::wcscpy(SDK::g_pLocalPlayer_info.wsz_map_name, wsz_name);

		const auto i_len = std::wcslen(result);
		if (i_len == 0) {
			dbg("cant copy cur map name to struct!\n");

			wcscpy_s(SDK::g_pLocalPlayer_info.wsz_map_name, L"unknown");
		}
		else {
			SDK::g_pLocalPlayer_info.map_name = result;
		}*/
	}
}


void EntityLoop(SDK::UCanvas* p_canvas)
{
	std::lock_guard guard(entity_mutex);

	/*for (SDK::AActor* pDebugActor : DebugActor)
	{
		if (pDebugActor->bPendingDelete || pDebugActor->bDeleteMe)
			continue;

		auto Center = pCanvas->Project(pDebugActor->Location);
		if (!Center.X || !Center.Y)
			return;

		CA2W pszTag(pDebugActor->GetFullName().c_str());
		Renderer::DrawString(pCanvas, Center.X, Center.Y, pszTag.m_psz, 1.0f, 1.0f, SDK::White, SDK::g_pEngine->SmallFont);
	}*/
	
	for (const auto& p_entities : entities)
	{		
		if (SDK::g_pLocalPlayerPawn == nullptr)
			break;

		if (p_entities == SDK::g_pLocalPlayerPawn)
			continue;

		if (p_entities->bPlayedDeath || p_entities->bDeleteMe || p_entities->bHidden || p_entities->bPendingDelete
			|| (p_entities->Physics.GetValue() == SDK::EPhysics::PHYS_RigidBody))
			continue;

		if (!p_entities->IsAliveAndWell())
			continue;

		if (SDK::IsLocalVehicle())
			if (const auto p_vehicle = p_entities->DrivenEFVehicle)
				if (p_vehicle == SDK::g_pLocalPlayerPawn)
					continue;

		if(p_entities->IsA(SDK::AEFTransit::StaticClass()))
			continue;

		/// Aimbot
		g_pAimbot->Run(p_canvas, p_entities);

		/// ESP
		g_pESP->Run(p_canvas, p_entities);
	}

	for (const auto& p_projectiles : projectiles)
	{
		if (p_projectiles->bPendingDelete || p_projectiles->bDeleteMe || p_projectiles->bHidden)
			continue;

		if (p_projectiles->DestroyCalled || p_projectiles->RemoveCalled || p_projectiles->SendedRemovePacket)
			continue;

		const auto center = p_canvas->Project(p_projectiles->Location);
		if (!center.X || !center.Y)
			return;

		Renderer::DrawString(p_canvas, center.X, center.Y, L"[+]", 1.0f, 1.0f, SDK::colors::white, SDK::g_pEngine->SmallFont);

		std::wstring target_type{ L"TargetType: " };
		switch (p_projectiles->TargetType)
		{
		case SDK::ESkillEffectTarget::SKILL_EFFECT_TARGET_NONE:
			target_type += L"None";
			break;
		case SDK::ESkillEffectTarget::SKILL_EFFECT_TARGET_PARTY:
			target_type += L"Party";
			break;
		case SDK::ESkillEffectTarget::SKILL_EFFECT_TARGET_ENEMY:
			target_type += L"Enemy";
			break;
		case SDK::ESkillEffectTarget::SKILL_EFFECT_TARGET_ALLY:
			target_type += L"Ally";
			break;
		case SDK::ESkillEffectTarget::SKILL_EFFECT_TARGET_ALL:
			target_type += L"All";
			break;
		case SDK::ESkillEffectTarget::SKILL_EFFECT_TARGET_ZONE_PC:
			target_type += L"Zone PC";
			break;
		case SDK::ESkillEffectTarget::SKILL_EFFECT_TARGET_ZONE_OBJECT:
			target_type += L"Zone Object";
			break;
		case SDK::ESkillEffectTarget::SKILL_EFFECT_TARGET_MAX:
			break;
		default:
			break;
		}

		Renderer::DrawString(p_canvas, center.X, center.Y, target_type.c_str(), 1.0f, 1.0f, SDK::colors::white, SDK::g_pEngine->SmallFont);
	}

	for (const auto& p_items : items)
	{
		if (p_items->bPendingDelete || p_items->bDeleteMe || p_items->bHidden)
			continue;

		const auto center = p_canvas->Project(p_items->Location);
		if (!center.X || !center.Y)
			return;

		const CA2W psz_name(p_items->GetName().c_str());
		Renderer::DrawString(p_canvas, center.X, center.Y, psz_name.m_psz, 1.0f, 1.0f, SDK::colors::white, SDK::g_pEngine->SmallFont);

		/// Teleports Items to LocalPlayer
		//pItems->SetLocationForceUpdateComponent(SDK::g_pLocalPlayerPawn->Location);
	}

	for (const auto& p_resources : resources)
	{
		if (p_resources->bPendingDelete || p_resources->bDeleteMe || p_resources->bHidden || p_resources->bStatic)
			continue;

		if (p_resources->bWorldGeometry)
			continue;

		if (p_resources->bTraceGameLand)
			continue;

		const auto center = p_canvas->Project(p_resources->Location);
		if (!center.X || !center.Y)
			return;

		const CA2W psz_name(p_resources->GetName().c_str());
		Renderer::DrawString(p_canvas, center.X, center.Y, psz_name.m_psz, 1.0f, 1.0f, SDK::colors::white, SDK::g_pEngine->SmallFont);
	}
}

void OnEfGameEfgFxHudWrapperPostRender(void)
{
	auto b_status = false;
	
	if (SDK::g_pEngine == nullptr)
		return;

	if (SDK::g_pEngine->TinyFont == nullptr)
		return;

	if (SDK::g_pEngine->MediumFont == nullptr)
		return;

	if (SDK::g_pEngine->bScreenshotRequested)
		return;

	if (SDK::g_pEngine->GameViewport)
	{
		SDK::g_pEngine->GameViewport->GetViewportSize(SDK::g_pViewportSize);
	}

	if (SDK::g_pLocalPlayer == nullptr)
		return;

	if (SDK::g_pPC == nullptr)
		return;

	if (SDK::g_pPC->myHUD == nullptr)
		return;

	if (SDK::g_pHUD == nullptr)
	{
		SDK::g_pHUD = SDK::g_pPC->myHUD;
		if (SDK::g_pHUD == nullptr)
			return;
	}

	if (SDK::g_pCanvas == nullptr)
	{
		if (SDK::g_pHUD->Canvas == nullptr)
			return;

		SDK::g_pCanvas = SDK::g_pHUD->Canvas;
		if (SDK::g_pCanvas == nullptr)
			return;
	}

	float X = 0;
	float Y = 0;

	if (!SDK::g_pWorldName.empty())
	{
		if (SDK::g_pWorldName.find("lv_eflobby_ps") != std::string::npos)
		{
			b_status = false;
			X = 15;
			Y = 15;
		}
		else
		{
			b_status = true;
			X = 15;
			Y = 60;
		}
	}

	[[maybe_unused]] auto* p_pc = static_cast<SDK::AEFPlayerController*>(SDK::g_pPC);

	/// Watermark
	Renderer::DrawString(SDK::g_pCanvas, X, Y, L"*redacted* :: Lost Ark Pre-Alpha", 1.0f, 1.0f, SDK::colors::white, SDK::g_pEngine->MediumFont);



	{
		X += 30;
		Y += 80;

		const auto& font = SDK::g_pEngine->MediumFont;
		
		if( SDK::g_pLocalPlayer_info.name.IsValid() )
			Renderer::DrawString(SDK::g_pCanvas, X, Y, SDK::g_pLocalPlayer_info.name, 1.0f, 1.0f, SDK::colors::white, font);

		Y += 15;
		if (SDK::g_pLocalPlayer_info.map_name.IsValid())
			Renderer::DrawString(SDK::g_pCanvas, X, Y, SDK::g_pLocalPlayer_info.map_name, 1.0f, 1.0f, SDK::colors::white, font);

		Y += 15;
		const auto ws_object(L"Object: " + std::to_wstring(SDK::g_pLocalPlayer_info.object));
		Renderer::DrawString(SDK::g_pCanvas, X, Y, ws_object.c_str(), 1.0f, 1.0f, SDK::colors::white, font);
		
		Y += 15;
		const auto ws_objectid(L"GameID: " + std::to_wstring(SDK::g_pLocalPlayer_info.object_id));
		Renderer::DrawString(SDK::g_pCanvas, X, Y, ws_objectid.c_str(), 1.0f, 1.0f, SDK::colors::white, font);

		Y += 15;
		const auto ws_engineid(L"EngineID: " + std::to_wstring(SDK::g_pLocalPlayerPawn->EFObjectUid.A));
		Renderer::DrawString(SDK::g_pCanvas, X, Y, ws_engineid.c_str(), 1.0f, 1.0f, SDK::colors::white, font);


		Y += 15;
		const auto ws_poisition(L"Position: " + std::to_wstring(SDK::g_pLocalPlayer_info.position.X) + L" " + std::to_wstring(SDK::g_pLocalPlayer_info.position.Y) + L" " + std::to_wstring(SDK::g_pLocalPlayer_info.position.Z));
		Renderer::DrawString(SDK::g_pCanvas, X, Y, ws_poisition.c_str(), 1.0f, 1.0f, SDK::colors::white, font);
		
		Y += 15;
		const auto ws_level ( L"Level: " + std::to_wstring(SDK::g_pLocalPlayer_info.level) );
		Renderer::DrawString(SDK::g_pCanvas, X, Y, ws_level.c_str(), 1.0f, 1.0f, SDK::colors::white, font);

		Y += 15;
		const auto ws_health(L"Health: " + std::to_wstring(SDK::g_pLocalPlayer_info.hp));
		Renderer::DrawString(SDK::g_pCanvas, X, Y, ws_health.c_str(), 1.0f, 1.0f, SDK::colors::white, font);

		Y += 15;
		const auto ws_maxhealth(L"Max Health: " + std::to_wstring(SDK::g_pLocalPlayer_info.max_hp));
		Renderer::DrawString(SDK::g_pCanvas, X, Y, ws_maxhealth.c_str(), 1.0f, 1.0f, SDK::colors::white, font);

		Y += 15;
		const auto ws_mana(L"Mana: " + std::to_wstring(SDK::g_pLocalPlayer_info.mp));
		Renderer::DrawString(SDK::g_pCanvas, X, Y, ws_mana.c_str(), 1.0f, 1.0f, SDK::colors::white, font);

		Y += 15;
		const auto ws_maxmana(L"Max Mana: " + std::to_wstring(SDK::g_pLocalPlayer_info.max_mp));
		Renderer::DrawString(SDK::g_pCanvas, X, Y, ws_maxmana.c_str(), 1.0f, 1.0f, SDK::colors::white, font);

		Y += 15;
		const auto ws_gold(L"Gold: " + std::to_wstring(SDK::g_pLocalPlayer_info.gold_coin));
		Renderer::DrawString(SDK::g_pCanvas, X, Y, ws_gold.c_str(), 1.0f, 1.0f, SDK::colors::white, font);
	}
	
	/// Debug by game
	/*SDK::g_pHUD->bShowDebugInfo = 1;
	SDK::g_pHUD->bShowBadConnectionAlert = 1;
	SDK::g_pHUD->bShowDirectorInfoDebug = 1;
	SDK::g_pHUD->bShowDirectorInfoHUD = 1;*/

	/// Crosshair
	Renderer::DrawCosshair(SDK::g_pCanvas);

	/// Entity Loop
	if (b_status)
	{
		EntityLoop(SDK::g_pCanvas);

		/*SDK::g_pPC->EnableCheats();
		SDK::g_pPC->AddCheats(true);*/

		//if (auto Cheat = SDK::g_pPC->CheatManager)
		//{
		//	//Cheat->KillAllPawns(SDK::AEFPlayer::StaticClass());
		//	//Cheat->ViewClass(SDK::AEFMonster::StaticClass());

		//	/*Cheat->Slomo(3);
		//	Cheat->SetGravity(2);
		//	Cheat->SetJumpZ(5);
		//	Cheat->ChangeSize(5);*/
		//}
	}
}

void InvisibleAll(SDK::AEFPawn* p_entity)
{
	if (p_entity == nullptr || !p_entity->IsAliveAndWell())
		return;

	SDK::g_pPC->ServerUpdateLevelVisibility(p_entity->GetPackageName(), false);
}

void OnEngineGameViewportClientLayoutPlayers()
{
	entity_mutex.lock();

	entities.clear();
	projectiles.clear();
	resources.clear();

	//DebugActor.clear();

	entity_mutex.unlock();

	GetLocalPlayerInfo();
	
	if (SDK::g_pWorld == nullptr)
	{
		SDK::g_pWorld = (*SDK::g_world);
		if (SDK::g_pWorld == nullptr)
			return;
	}

	if (SDK::g_pLocalPlayer == nullptr)
	{
		SDK::g_pLocalPlayer = SDK::GetLocalPlayer();
		if (SDK::g_pLocalPlayer == nullptr)
			return;
	}

	if (SDK::g_pPC == nullptr)
	{
		SDK::g_pPC = static_cast<SDK::AEFPlayerController*>(SDK::g_pLocalPlayer->Actor);
		if (SDK::g_pPC == nullptr)
			return;
	}

	if (auto world_info = SDK::g_pPC->WorldInfo)
	{
		SDK::g_pWorldName = world_info->GetMapName(false).ToString();
	}

	if (SDK::g_pLocalPlayerPawn == nullptr)
	{
		SDK::g_pLocalPlayerPawn = static_cast<SDK::APawn*>(SDK::g_pPC->AcknowledgedPawn);
		if (SDK::g_pLocalPlayerPawn == nullptr)
			return;
	}

	/// CustomFOV
	if (SDK::g_pPC->GetFOVAngle() != 100.0f)
	{
		dbg("Set Custom FOV");

		SDK::g_pPC->FOV(100.0f);
	}


	//struct FEFInputCommand                             m_CurInputCommand;                                        // 0x079C(0x0040) (Transient)
	//TEnumAsByte<EFPlayerControlInputCondition>         m_CurInputCondition;
	
	/// Bullet hack
	/*if (g_pLocalPlayerPawn->ShotCount)
	{
	g_pLocalPlayerPawn->ShotCount = (g_pLocalPlayerPawn->ShotCount + 3);
	}*/

	/// More dmg
	/*if (g_pLocalPlayerPawn->DamageScaling)
	{
	g_pLocalPlayerPawn->DamageScaling = (g_pLocalPlayerPawn->DamageScaling + 100.0f);
	}*/


	SDK::g_pPersistentLevel = SDK::g_pWorld->PersistentLevel;
	if (!SDK::g_pPersistentLevel)
		return;

	auto m_actors = &SDK::g_pPersistentLevel->Actors;
	if (!m_actors)
		return;

	if (m_actors->Num() <= 1)
		return;
	
	for (auto i = 0; i < m_actors->Num(); i++)
	{
		if (!m_actors->IsValidIndex(i))
			continue;

		const auto actor = m_actors->GetByIndex(i);
		if (actor == nullptr)	
			continue;

#if DEBUG_PRINT_
		//dbg("Actor: %s:[%i:%i]\n", actor->GetFullName().c_str(), actor->ObjectInternalInteger, actor->Tag.Index);

		/*char actors[255];
		sprintf_s(actors, "Actor: %s:[%i:%i]\n", actor->GetFullName().c_str(), actor->ObjectInternalInteger, actor->Tag.Index);

		Log->Write(actors, LogType::Info);*/
		//DebugActor.emplace_back(actor);

		/*std::call_once(g_isInitialized, [&]() {
			Log->Initialize("C:\\SDK_GEN\\actor_list.txt");
			});

		char alqaeda[512];
		sprintf_s(alqaeda, "Actor [Current: %i @ Max: %i]: %s @ %i\n", i, m_AActors->Num(), actor->GetFullName().c_str(), actor->ObjectInternalInteger);
		Log->Write(alqaeda, LogType::Info);*/
#endif
		
		if (actor->IsA(SDK::AEFPawn::StaticClass()))
		{
			auto entity = static_cast<SDK::AEFPawn*>(actor);
			if (entity == nullptr
				|| entity->bDeleteMe
				|| entity->bPendingDelete
				|| entity->bHidden)
				continue;


			if (entity == SDK::g_pLocalPlayerPawn)
				continue;
			
			if (std::find(entities.begin(), entities.end(), entity) != entities.end())
				continue;

			entities.emplace_back(entity);
		}

		if (actor->IsA(SDK::AEFProjectile::StaticClass()))
		{
			auto projectile = static_cast<SDK::AEFProjectile*>(actor);
			if (projectile == nullptr
				|| projectile->bDeleteMe
				|| projectile->bPendingDelete
				|| projectile->bHidden)
				continue;

			if (std::find(projectiles.begin(), projectiles.end(), projectile) != projectiles.end())
				continue;

			projectiles.emplace_back(projectile);
		}

		if (actor->IsA(SDK::AEFItem::StaticClass()))
		{
			auto Item = static_cast<SDK::AEFItem*>(actor);
			if (Item == nullptr || Item->bDeleteMe
				|| Item->bPendingDelete
				|| Item->bHidden)
				continue;


			if (std::find(items.begin(), items.end(), Item) != items.end())
				continue;

			items.emplace_back(Item);
		}

		//if (actor->IsA(SDK::AEFBarrier::StaticClass()))
		//{
		//	auto Barrier = static_cast<SDK::AEFBarrier*>(actor);
		//	if (Barrier == nullptr
		//		|| Barrier->bDeleteMe
		//		|| Barrier->bPendingDelete)
		//		continue;

		//	//dbg("Barrier: %s\n", Barrier->GetName().c_str());
		//}

		//if (actor->IsA(SDK::AEFTransit::StaticClass()))
		//{
		//	auto Transit = static_cast<SDK::AEFTransit*>(actor);
		//	if (Transit == nullptr
		//		|| Transit->bDeleteMe
		//		|| Transit->bPendingDelete)
		//		continue;

		//	//dbg("Transit: %s\n", Transit->GetName().c_str());
		//}

		//if (actor->IsA(SDK::AEFTrap::StaticClass()))
		//{
		//	auto Trap = static_cast<SDK::AEFTrap*>(actor);
		//	if (Trap == nullptr
		//		|| Trap->bDeleteMe
		//		|| Trap->bPendingDelete)
		//		continue;

		//	//dbg("Trap: %s\n", Trap->GetName().c_str());
		//}

		if (actor->IsA(SDK::AEFSkeletalMeshActor::StaticClass()))
		{
			auto* Resource = static_cast<SDK::AEFSkeletalMeshActor*>(actor);
			if (Resource == nullptr
				|| Resource->bDeleteMe
				|| Resource->bPendingDelete)
				continue;

			if (std::find(resources.begin(), resources.end(), Resource) != resources.end())
				continue;

			resources.emplace_back(Resource);
		}
	}
}


bool ReHook(void)
{
	if (!is_destroyed)
		return false;

	/*if (SDK::g_pWorld == nullptr)
	{
		SDK::g_pWorld = (*SDK::g_world);
		if (SDK::g_pWorld == nullptr)
			return false;
	}

	SDK::g_pLocalPlayer = SDK::GetLocalPlayer();
	if (SDK::g_pLocalPlayer == nullptr)
		return false;

	SDK::g_pPC = static_cast<SDK::AEFPlayerController*>(SDK::g_pLocalPlayer->Actor);
	if (SDK::g_pPC == nullptr)
		return false;

	SDK::g_pLocalPlayerPawn = static_cast<SDK::APawn*>(SDK::g_pPC->AcknowledgedPawn);
	if (SDK::g_pLocalPlayerPawn == nullptr)
		return false;

	player_controller_hook = new VTHook( reinterpret_cast<PDWORD64*>(SDK::g_pPC) );
	if (!player_controller_hook)
	{
#ifdef __DebugPrint__
		dbg("Can't apply 'PlayerController' vmt swap!\n");
#endif
		return false;
	}

	origprocessevent = reinterpret_cast<processevent_fn>( player_controller_hook->dwHookMethod( reinterpret_cast<DWORD64>(&Hooked_ProcessEvent), 67 ) );

	SDK::g_pHUD = SDK::g_pPC->myHUD;
	if (SDK::g_pHUD == nullptr)
		return false;

	hud_hook = new VTHook( reinterpret_cast<PDWORD64*>(SDK::g_pHUD) );

	if (!hud_hook)
	{
#ifdef __DebugPrint__
		dbg("Can't apply 'HUD' vmt swap!\n");
#endif
		return false;
	}

	origprocessevent = reinterpret_cast<processevent_fn> ( hud_hook->dwHookMethod( reinterpret_cast<DWORD64>(&Hooked_ProcessEvent), 67 ) );*/

	return true;
}

processevent_fn origprocessevent;
void Hooked_ProcessEvent(SDK::UObject* pUObj, SDK::UFunction* pUFunc, void* pParams, void* pResult)
{
	const auto eflags = __readeflags();

	/*if(SDK::g_pEngine->Client) 
	{
		dbg( "MinDesiredFrameRate: %f\nMaxSmoothedFrameRate: %f\nMinSmoothedFrameRate: %f\n", SDK::g_pEngine->Client->MinDesiredFrameRate, SDK::g_pEngine->MaxSmoothedFrameRate, SDK::g_pEngine->MinSmoothedFrameRate);
	}*/
	
	if (pUFunc)
	{
#if DEBUG_PRINT_
		//dbg("Func: %s:[%i]\n", pUFunc->GetFullName().c_str(), pUFunc->ObjectInternalInteger);

		char func[256];
		sprintf_s(func, "(%s) <%i>", pUFunc->GetFullName().c_str(), pUFunc->ObjectInternalInteger);
		Log->Write(func, LogType::kDebug);
#endif

		const auto i_index = pUFunc->ObjectInternalInteger;

		if (i_index == SDK::EFuncId::kEfGameEfgFxHudWrapperPostRender)
		{
			OnEfGameEfgFxHudWrapperPostRender();
		}
		else if (i_index == SDK::EFuncId::kEngineGameViewportClientLayoutPlayers)
		{
			if (!is_destroyed)
				OnEngineGameViewportClientLayoutPlayers();
		}
		else if (i_index == SDK::EFuncId::kEfGameEfGameViewportClientTick)
		{
			if (is_destroyed)
				++skip_ticks;

			if (pParams)
			{
				auto* const params = static_cast<SDK::UEFGameViewportClient_Tick_Params*>(pParams);
				if (params)
				{
					SDK::g_pTickDeltaTime = params->DeltaTime;
				}
			}
		}

		if (i_index == SDK::EFuncId::kEfGameEfgFxHudWrapperDestroyed)
		{
			if (pUObj == SDK::g_pHUD)
			{
#if DEBUG_PRINT_
				dbg("GUI Destroyed");
#endif

				/*if (hud_hook)
					hud_hook->UnHook();*/

				is_destroyed = true;
				SDK::g_pHUD = nullptr;
				SDK::g_pCanvas = nullptr;
			}
		}
		else if (i_index == SDK::EFuncId::kEnginePlayerControllerDestroyed)
		{
			if (pUObj == SDK::g_pPC)
			{
#if DEBUG_PRINT_
				dbg("PC Destroyed");
#endif
				
				/*if (player_controller_hook)
					player_controller_hook->UnHook();*/

				SDK::g_pWorld = nullptr;
				SDK::g_pLocalPlayer = nullptr;
				SDK::g_pPC = nullptr;
				SDK::g_pLocalPlayerPawn = nullptr;
			}
		}

		if (is_destroyed && skip_ticks >= 15)
		{
			if (ReHook())
			{
				skip_ticks = 0;
				is_destroyed = false;
			}
		}
	}

	__writeeflags(eflags);
	
	//origprocessevent(pUObj, pUFunc, pParams, pResult);

	reinterpret_cast<processevent_fn> ( game_viewport_client_hook.at( 67 ) ) (pUObj, pUFunc, pParams, pResult);
}