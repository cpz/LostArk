#ifndef AIMBOT_H__
#define AIMBOT_H__

#include "../Include.h"

#include "../UnrealSDK/SDK.hpp"

#include "../UnrealSDK/Vector.h"

class CAimbot
{
public:
	CAimbot(void);

	~CAimbot(void);

	void           Run(SDK::UCanvas* Canvas, SDK::AEFPawn* pEntity);

	void           OnKeyEvent(UINT message, WPARAM wParam, LPARAM lParam);
private:
	SDK::AEFPawn*  FindClosestTarget(SDK::AEFPawn* pEntity);

	void           AimAt(SDK::UCanvas* Canvas, SDK::AEFPawn* pEntity);

	Vector         PingCorrection(SDK::AEFPawn* Target);
	bool           IsValidTarget(SDK::AEFPawn* pEntity);

	void           OPK(SDK::AEFPawn* pTarget);
	void           Telekill(SDK::AEFPawn* pTarget);

#if DEBUG_PRINT_
	void           DumpCurTargetBones(SDK::AEFPawn* pEntity);
#endif
		
private:
	HWND           m_hHwnd;
	bool           m_bKeyPressed;
};

extern CAimbot* g_pAimbot;
#endif