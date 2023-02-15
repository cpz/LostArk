#include "SDK.hpp"

namespace Renderer
{
	///
	SDK::FBox GetScreenBoundingBox(SDK::UCanvas* pCanvas, SDK::AActor* pActor);

	///
	void DrawString(SDK::UCanvas* pCanvas, float X, float Y, SDK::FString String, float XScale, float YScale, SDK::FColor DesiredColor, SDK::UFont* pFont);
	void DrawIcon(SDK::UCanvas* pCanvas, float X, float Y, float Scale, SDK::UTexture2D* Texture);
	void DrawBox(SDK::UCanvas* pCanvas, float X, float Y, float Width, float Height, SDK::FColor Color);
	void Draw3DBoundingBox(SDK::UCanvas *canvas, SDK::AEFPawn *pawn, const SDK::FColor& color);

	///
	void DrawCosshair(SDK::UCanvas* pCanvas);
	void DrawPlayerSkeleton(SDK::UCanvas* pCanvas, SDK::AEFPawn* pEntity, SDK::FColor Color);
	void LaserESP(SDK::UCanvas* pCanvas, SDK::AEFPawn* pEntity, const SDK::FColor& Color);
	void HeadCircle(SDK::UCanvas* pCanvas, SDK::AEFPawn* pEntity, int numSides, SDK::FColor Color);
}