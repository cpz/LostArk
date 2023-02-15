#include "ESP.h"

#include <atlstr.h>

CEsp* g_pESP = new CEsp();

CEsp::CEsp(void)
{
	// create
}

CEsp::~CEsp(void)
{
	// destroy
}

void CEsp::Run(SDK::UCanvas* Canvas, SDK::AEFPawn* pEntity)
{
	this->PlayerESP(Canvas, pEntity);
}


void CEsp::ColorModels(SDK::APawn* ThePawnToColor, SDK::FColor Color, SDK::TEnumAsByte<SDK::ESceneDepthPriorityGroup> NewDepthPriorityGroup)
{
	if (ThePawnToColor->Mesh)
	{
		ThePawnToColor->Mesh->SetDepthPriorityGroup(NewDepthPriorityGroup);

		if (ThePawnToColor->Mesh->LightEnvironment)
		{
			if (ThePawnToColor->Mesh->LightEnvironment->IsA(SDK::UDynamicLightEnvironmentComponent::StaticClass()))
			{
				auto light_environment = static_cast<SDK::UDynamicLightEnvironmentComponent*>(ThePawnToColor->Mesh->LightEnvironment);

				light_environment->AmbientGlow = SDK::g_pPC->STATIC_MakeLinearColor(Color.R, Color.G, Color.B, Color.A);
			}
		}
	}
}

void CEsp::PlayerESP(SDK::UCanvas* Canvas, SDK::AEFPawn* pEntity)
{
	auto p_font = SDK::g_pEngine->TinyFont;
	if (p_font == nullptr)
		return;

	if (!SDK::g_pLocalPlayerPawn->Location.X || !SDK::g_pLocalPlayerPawn->Location.Y || !SDK::g_pLocalPlayerPawn->Location.Z)
		return;

	auto from = Canvas->Project(SDK::g_pLocalPlayerPawn->Location);
	if (!from.X || !from.Y)
		return;

	auto to = Canvas->Project(pEntity->Location);
	if (!to.X || !to.Y)
		return;

	if (!SDK::IsVisibleOnScreen(Canvas, to))
		return;

	/*if(!pEntity->STATIC_IsZero(To))
		return;
		pPlayerReplicationInfo->myPawn->IsInState( FName( TEXT( "Dying" ) ) )
		*/

	/// Type
	std::wstring entity_tag{ L"Tag: " };

	auto entity_color = SDK::colors::empty;
	if (IsPlayer(pEntity))
	{
		entity_tag.append( L"Player" );
		if (SDK::IsVisible(pEntity))
			entity_color = SDK::colors::green;
		else
			entity_color = SDK::colors::darker_green;
	}
	else if (IsNpc(pEntity))
	{
		entity_tag.append( L"NPC" );
		entity_color = SDK::colors::blue;
	}
	else if (IsMonster(pEntity))
	{
		entity_tag.append( L"Monster" );
		if (SDK::IsVisible(pEntity))
			entity_color = SDK::colors::darker_red;
		else
			entity_color = SDK::colors::red;
	}
	else if (IsPet(pEntity))
	{
		entity_tag.append( L"Pet" );
		entity_color = SDK::colors::yellow;
	}
	else if (IsVehicle(pEntity))
	{
		entity_tag.append( L"Vehicle" );
		entity_color = SDK::colors::cyan;
	}
	else if (IsSummonNpc(pEntity))
	{
		entity_tag.append( L"Summon NPC" );
		entity_color = SDK::colors::yellow;
	}
	else
	{
		entity_tag.append( L"Unknown" );
		entity_color = SDK::colors::black;
	}

	/// Box
	auto f_box = Renderer::GetScreenBoundingBox(Canvas, pEntity);

	auto x = f_box.Min.X;
	auto y = f_box.Min.Y;
	auto w = f_box.Max.X - f_box.Min.X;
	auto h = f_box.Max.Y - f_box.Min.Y;

	/// 2D ESP Box
	//Renderer::DrawBox(Canvas, x, y, w, h, EntityColor);

	/// 3D ESP Box
	//Renderer::Draw3DBoundingBox(Canvas, pEntity, entity_color);

	/// Laser ESP
	//Renderer::LaserESP(Canvas, pEntity, EntityColor);

	/// Bones ESP
	//Renderer::DrawPlayerSkeleton(Canvas, pEntity, EntityColor);

	/// Circle around head
	//Renderer::HeadCircle(Canvas, pEntity, 350, EntityColor);

	/// Snaplines
	//Canvas->Draw2DLine(From.X, From.Y, To.X, To.Y, EntityColor);

	/// Num of features
	auto i_num = -1;

	/// Type
	/*Renderer::DrawString(Canvas, x + w + 3, y + i_num, entity_tag.c_str(), 1.0f, 1.0f, entity_color, p_font);
	i_num += 15;*/
	///
	///
	CA2W psz_pack_name( pEntity->GetFullName().c_str() );
	Renderer::DrawString(Canvas, x + w + 3, y + i_num, psz_pack_name.m_psz, 1.0f, 1.0f, entity_color, p_font);
	i_num += 15;

	if(pEntity->ObjectArchetype != nullptr) {
		auto outer = pEntity->ObjectArchetype;
		CA2W psz_outer(outer->GetName().c_str());
		Renderer::DrawString(Canvas, x + w + 3, y + i_num, psz_outer.m_psz, 1.0f, 1.0f, entity_color, p_font);
		i_num += 15;
	}

	
	/// Distance
	/*auto distance = SDK::Get3DDistance(SDK::g_pLocalPlayerPawn, pEntity);
	std::wstring ws_distance{ L"Distance: " };
	ws_distance += std::to_wstring(distance) + L"m";

	Renderer::DrawString(Canvas, x + w + 3, y + i_num, ws_distance.c_str(), 1.0f, 1.0f, entity_color, p_font);
	i_num += 15;*/
	///

	/// Sequence
	/*std::wstring wsPhysics{ L"Sequence: " };
	auto pPhysics = pEntity->Physics.GetValue();
	if (pPhysics == SDK::EPhysics::PHYS_Skill)
	{
		wsPhysics.append(L"Casting Skill ");
	}
	else if (pPhysics == SDK::EPhysics::PHYS_Falling)
	{
		wsPhysics.append(L"Falling ");
	}
	else if (pPhysics == SDK::EPhysics::PHYS_Swimming)
	{
		wsPhysics.append(L"Swimming ");
	}
	else if (pPhysics == SDK::EPhysics::PHYS_Flying)
	{
		wsPhysics.append(L"Flying ");
	}
	else if (pPhysics == SDK::EPhysics::PHYS_Ladder)
	{
		wsPhysics.append(L"Climbing on Ladder ");
	}
	else if (pPhysics == SDK::EPhysics::PHYS_BeHit)
	{
		wsPhysics.append(L"Got Hit ");
	}
	else if (pPhysics == SDK::EPhysics::PHYS_FLIGHT)
	{
		wsPhysics.append(L"Flight ");
	}
	else if (pEntity->IsInPain())
	{
		wsPhysics.append(L"In Pain ");
	}
	else if (pEntity->IsInvisible())
	{
		wsPhysics.append(L"Invisible ");
	}

	if (pPhysics != SDK::EPhysics::PHYS_Walking && pPhysics != SDK::EPhysics::PHYS_None
		&& pPhysics != SDK::EPhysics::PHYS_RigidBody)
	{
		Renderer::DrawString(Canvas, x + w + 3, y + i_num, wsPhysics.c_str(), 1.0f, 1.0f, entity_color, p_font);
		i_num += 15;
	}*/



	if (auto* anim_seq = pEntity->LatentSeqNode)
	{
		std::string s_anim_seq{ "AnimSeqName: " };
		s_anim_seq += anim_seq->AnimSeqName.GetName() + " ";

		CA2W psz_anim_seq(s_anim_seq.c_str());
		Renderer::DrawString(Canvas, x + w + 3, y + i_num, psz_anim_seq.m_psz, 1.0f, 1.0f, entity_color, p_font);
		i_num += 15;
	}
	///

	/// PP Outlines
	//pEntity->SetPPOutlineWidth(1.f);
	//pEntity->SetPPOutlineColor({ (float)EntityColor.R, (float)EntityColor.G, (float)EntityColor.B });
	//pEntity->SetPPOutline(true);
	///

	/// Cloak
	//pEntity->SetCloakMode(true, 255.0f, true);
	///

	/// Wallhack - SDPG_UnrealEdForeground: hides entity - SDPG_Foreground: colored entity
	//ColorModels(pEntity, entity_color, SDK::ESceneDepthPriorityGroup::SDPG_Foreground);
}