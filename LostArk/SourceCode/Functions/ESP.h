#ifndef ESP_H__
#define ESP_H__

#include "../Include.h"

#include "../UnrealSDK/SDK.hpp"

#include "../UnrealSDK/Render.h"

class CEsp
{
public:
	CEsp(void);

	~CEsp(void);

	void        Run(SDK::UCanvas* Canvas, SDK::AEFPawn* pEntity);
private:
	void        PlayerESP(SDK::UCanvas* Canvas, SDK::AEFPawn* pEntity);

	void        ColorModels(SDK::APawn* ThePawnToColor, SDK::FColor Color, SDK::TEnumAsByte<SDK::ESceneDepthPriorityGroup> NewDepthPriorityGroup);
};

extern CEsp* g_pESP;
#endif