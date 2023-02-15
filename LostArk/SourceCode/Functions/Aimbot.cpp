#include "Aimbot.h"

CAimbot* g_pAimbot = new CAimbot();

CAimbot::CAimbot(void)
{
	// create
}

CAimbot::~CAimbot(void)
{
	// destroy
}

void CAimbot::Run(SDK::UCanvas* Canvas, SDK::AEFPawn* pEntity)
{
	/// Because we use GetASyncKeyState, we need to know if current active window is game window
	if ((this->m_hHwnd = FindWindowA("EFLaunchUnrealUWindowsClient", nullptr)) &&
		GetForegroundWindow() == m_hHwnd)
	{
		const auto p_target = this->FindClosestTarget(pEntity);
		if (p_target == nullptr)
			return;

		this->AimAt(Canvas, p_target);
	}
}


Vector CAimbot::PingCorrection(SDK::AEFPawn* Target)
{
	unsigned char Ping = 0;
	if (auto pInfo = SDK::g_pLocalPlayerPawn->PlayerReplicationInfo)
		Ping = pInfo->Ping;

#if DEBUG_PRINT_
	printf("Ping: %i\n", Ping);
#endif

	const Vector t_velocity{ Target->Velocity.X, Target->Velocity.Y, Target->Velocity.Z };
	auto v_ping = t_velocity * (Ping + (SDK::g_pTickDeltaTime)) * Vector(1, 1, 0);
	const Vector l_velocity{ SDK::g_pLocalPlayerPawn->Velocity.X, SDK::g_pLocalPlayerPawn->Velocity.Y, SDK::g_pLocalPlayerPawn->Velocity.Z };
	v_ping -= l_velocity * SDK::g_pTickDeltaTime;
	return v_ping;
}

SDK::AEFPawn* CAimbot::FindClosestTarget(SDK::AEFPawn* pEntity)
{
	SDK::AEFPawn* p_result = nullptr;

	double dist;

	while (pEntity)
	{
		if (!this->IsValidTarget(pEntity))
		{
			pEntity = static_cast<SDK::AEFPawn*>(pEntity->NextPawn);
			continue;
		}

		const auto dist2 = SDK::Get3DDistance(SDK::g_pLocalPlayerPawn, pEntity);

		if (!p_result || (dist2 < dist))
		{
			p_result = pEntity;
			dist = dist2;
		}

		pEntity = static_cast<SDK::AEFPawn*>(pEntity->NextPawn);
	}

	return p_result;
}

void CAimbot::OPK(SDK::AEFPawn* pTarget)
{
	pTarget->TakeHitLocation.X = (SDK::g_pLocalPlayerPawn->Location.X + 300.0f);
	pTarget->TakeHitLocation.Y = SDK::g_pLocalPlayerPawn->Location.Y;
	pTarget->TakeHitLocation.Z = SDK::g_pLocalPlayerPawn->Location.Z;
}

void CAimbot::Telekill(SDK::AEFPawn* pTarget)
{
	SDK::g_pLocalPlayerPawn->Location.X = (pTarget->Location.X + 150.0f);
	SDK::g_pLocalPlayerPawn->Location.Y = pTarget->Location.Y;
	SDK::g_pLocalPlayerPawn->Location.Z = pTarget->Location.Z;
}

SDK::FVector inline RotationToVector(SDK::FRotator Rotation) 
{
	SDK::FVector vector;
	const auto f_yaw = Rotation.Yaw * static_cast<float>(CONST_UnrRotToRad);
	const auto f_pitch = Rotation.Pitch * static_cast<float>(CONST_UnrRotToRad);
	const auto f_cos_pitch = cos(f_pitch);

	vector.X = cos(f_yaw) * f_cos_pitch;
	vector.Y = sin(f_yaw) * f_cos_pitch;
	vector.Z = sin(f_pitch);

	return vector;
}

SDK::FRotator inline VectorToRotation(const SDK::FVector& vector)
{
	SDK::FRotator rotation;

	rotation.Pitch = SDK::g_pPC->STATIC_Atan2(vector.Z, SDK::g_pPC->STATIC_Sqrt((vector.X * vector.X) + (vector.Y * vector.Y))) * CONST_RadToUnrRot;
	rotation.Yaw = SDK::g_pPC->STATIC_Atan2(vector.Y, vector.X) * CONST_RadToUnrRot;
	rotation.Roll = 0;

	return rotation;
}

float inline GetViewAngle(SDK::UCanvas* pCanvas, SDK::FVector pawnLocation)
{
	if (SDK::g_pPC == nullptr)
		return 0.0f;

	SDK::FVector view_location{};
	SDK::FRotator view_rotation{};
	
	SDK::g_pPC->GetPlayerViewPoint(&view_location, &view_rotation);

	const SDK::FVector delta = {
		pawnLocation.X - view_location.X,
		pawnLocation.Y - view_location.Y,
		pawnLocation.Z - view_location.Z
	};

	const auto f_delta_size = pCanvas->STATIC_VSize(delta);

	const SDK::FVector div = {
		delta.X / f_delta_size,
		delta.Y / f_delta_size,
		delta.Z / f_delta_size
	};

	const auto view_rot_to_vec = RotationToVector(view_rotation);

	const auto f_angle = pCanvas->STATIC_Dot_VectorVector(div, view_rot_to_vec);
	
	return f_angle;
}

void CAimbot::AimAt(SDK::UCanvas* Canvas, SDK::AEFPawn* pEntity)
{
	if (pEntity && pEntity->Mesh && GetAsyncKeyState(VK_XBUTTON1) & 0x8000)
	{
		const auto v_bone_location = pEntity->Mesh->GetBoneLocation("Bip001-Head", 0);
		
		if (pEntity->STATIC_IsZero(v_bone_location))
			return;

		const auto project = Canvas->Project(v_bone_location);
		if (!project.X || !project.Y)
			return;

		if (!SDK::IsVisibleOnScreen(Canvas, project))
			return;

		SDK::g_pViewport->SetMouse(project.X, project.Y);

		const auto aim_rotation = VectorToRotation(v_bone_location);

		SDK::g_pPC->RelativeRotation = aim_rotation;
		SDK::g_pPC->Rotation = aim_rotation;
		SDK::g_pPC->SetRotation ( aim_rotation );
		SDK::g_pPC->ClientSetRotation ( aim_rotation, false );

		SDK::g_pLocalPlayerPawn->RelativeRotation = aim_rotation;
		SDK::g_pLocalPlayerPawn->Rotation = aim_rotation;
		SDK::g_pLocalPlayerPawn->ClientSetRotation ( aim_rotation );
		SDK::g_pLocalPlayerPawn->SetRotation ( aim_rotation );
		SDK::g_pLocalPlayerPawn->SetDesiredRotation ( aim_rotation, false, false, 0.0f, false );

		SDK::g_pPC->StartFire(0);
		SDK::g_pPC->StopFire(0);
	}
}


bool CAimbot::IsValidTarget(SDK::AEFPawn* pEntity)
{
	if (SDK::g_pPC == nullptr)
		return false;

	if (SDK::g_pPC->IsSpectating())
		return false;

	if (SDK::g_pLocalPlayerPawn->bPlayedDeath || SDK::g_pLocalPlayerPawn->bDeleteMe ||
		SDK::g_pLocalPlayerPawn->bPendingDelete || SDK::g_pLocalPlayerPawn->bHidden ||
		SDK::IsLocalVehicle())
		return false;

	if (pEntity == SDK::g_pLocalPlayerPawn)
		return false;

	if (pEntity->InGodMode())
		return false;

	if (pEntity->bPlayedDeath || pEntity->bDeleteMe ||
		pEntity->bPendingDelete || pEntity->bHidden)
		return false;

	if (!SDK::IsVisible(pEntity))
		return false;

	if (pEntity->Physics.GetValue() == SDK::EPhysics::PHYS_RigidBody)
		return false;

	if (IsPlayer(pEntity) && (pEntity->GetTeamNum() == 255))
		return false;

	if (IsPlayer(pEntity) && !(pEntity->IsSameTeam(SDK::g_pLocalPlayerPawn)))
		return true;

	if (IsMonster(pEntity))
		return true;

	return false;
}

void CAimbot::OnKeyEvent(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == WM_LBUTTONDOWN || message == WM_LBUTTONDBLCLK)
		m_bKeyPressed = true;
	else if (message == WM_LBUTTONUP)
		m_bKeyPressed = false;
}


#if DEBUG_PRINT_
#include "../Utils/Log.h"

void CAimbot::DumpCurTargetBones(SDK::AEFPawn* pEntity)
{
	SDK::TArray< SDK::FName > BoneName;
	pEntity->Mesh->GetBoneNames(&BoneName);
	char cur_bone[256];
	
	for (auto i = 0; i < BoneName.Num(); ++i)
	{
		sprintf_s(cur_bone, "{cur bone} (%s) <%i>", BoneName.GetByIndex(i).GetName().c_str(), i);
		Log->Write(cur_bone, LogType::kDebug);
	}
}
#endif