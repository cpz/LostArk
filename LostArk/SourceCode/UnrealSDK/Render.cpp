#include "Render.h"

namespace Renderer
{
	void DrawString(SDK::UCanvas* pCanvas, float X, float Y, SDK::FString String, float XScale, float YScale, SDK::FColor DesiredColor, SDK::UFont* pFont)
	{
		const auto old_cur_x = pCanvas->CurX;
		const auto old_cur_y = pCanvas->CurY;

		const auto old_color = pCanvas->DrawColor;
		
		pCanvas->CurX = X;
		pCanvas->CurY = Y;

		pCanvas->DrawColor = DesiredColor;
		pCanvas->Font = pFont;
		
		//if (SDK::g_pHUD)
		//{
		//	// top-left
		//	SDK::g_pHUD->DrawText(String, SDK::FVector2D{ X - 1, Y - 1 }, pFont, SDK::FVector2D{ XScale , YScale }, SDK::colors::black, false);
		//	// top
		//	SDK::g_pHUD->DrawText(String, SDK::FVector2D{ X, Y - 1 }, pFont, SDK::FVector2D{ XScale , YScale }, SDK::colors::black, false);
		//	// top-right
		//	SDK::g_pHUD->DrawText(String, SDK::FVector2D{ X + 1, Y - 1 }, pFont, SDK::FVector2D{ XScale , YScale }, SDK::colors::black, false);
		//	// right
		//	SDK::g_pHUD->DrawText(String, SDK::FVector2D{ X + 1, Y }, pFont, SDK::FVector2D{ XScale , YScale }, SDK::colors::black, false);
		//	// bottom-right
		//	SDK::g_pHUD->DrawText(String, SDK::FVector2D{ X + 1, Y + 1 }, pFont, SDK::FVector2D{ XScale , YScale }, SDK::colors::black, false);
		//	// bottom
		//	SDK::g_pHUD->DrawText(String, SDK::FVector2D{ X, Y + 1 }, pFont, SDK::FVector2D{ XScale , YScale }, SDK::colors::black, false);
		//	// bottom-left
		//	SDK::g_pHUD->DrawText(String, SDK::FVector2D{ X - 1, Y + 1 }, pFont, SDK::FVector2D{ XScale , YScale }, SDK::colors::black, false);
		//	// left
		//	SDK::g_pHUD->DrawText(String, SDK::FVector2D{ X - 1, Y }, pFont, SDK::FVector2D{ XScale , YScale }, SDK::colors::black, false);
		//}
		
		pCanvas->DrawText(String, false, XScale, YScale, NULL);

		pCanvas->CurX = old_cur_x;
		pCanvas->CurY = old_cur_y;

		pCanvas->DrawColor = old_color;

		pCanvas->Font = pCanvas->GetDefaultCanvasFont();
	}

	void DrawIcon(SDK::UCanvas* pCanvas, float X, float Y, float Scale, SDK::UTexture2D* Texture)
	{
		const auto cavas_texture = pCanvas->MakeIcon(Texture, 0, 0, 0, 0);
		pCanvas->DrawIcon(cavas_texture, X, Y, Scale);
	}

	void DrawCosshair(SDK::UCanvas* pCanvas)
	{
		if (!SDK::g_pViewport)
			return;

		const auto i_crosshair_size = 5;

		const auto mouse_position = SDK::g_pViewport->GetMousePosition();

		//BLACK BORDER
		{
			pCanvas->Draw2DLine(((mouse_position.X) - (i_crosshair_size + 6)), mouse_position.Y, ((mouse_position.X) + (i_crosshair_size + 6)), mouse_position.Y, SDK::colors::black);
			pCanvas->Draw2DLine(((mouse_position.X) - (i_crosshair_size + 6)), mouse_position.Y - 1, ((mouse_position.X) + (i_crosshair_size + 6)), mouse_position.Y - 1, SDK::colors::black);
			pCanvas->Draw2DLine(((mouse_position.X) - (i_crosshair_size + 6)), mouse_position.Y + 1, ((mouse_position.X) + (i_crosshair_size + 6)), mouse_position.Y + 1, SDK::colors::black);

			pCanvas->Draw2DLine(mouse_position.X, ((mouse_position.Y) - (i_crosshair_size + 6)), mouse_position.X, ((mouse_position.Y) + (i_crosshair_size + 6)), SDK::colors::black);
			pCanvas->Draw2DLine(mouse_position.X - 1, ((mouse_position.Y) - (i_crosshair_size + 6)), mouse_position.X - 1, ((mouse_position.Y) + (i_crosshair_size + 6)), SDK::colors::black);
			pCanvas->Draw2DLine(mouse_position.X + 1, ((mouse_position.Y) - (i_crosshair_size + 6)), mouse_position.X + 1, ((mouse_position.Y) + (i_crosshair_size + 6)), SDK::colors::black);
		}

		//CROSSHAIR
		pCanvas->Draw2DLine(((mouse_position.X) - (i_crosshair_size + 5)), mouse_position.Y, ((mouse_position.X) + (i_crosshair_size + 5)), mouse_position.Y, SDK::colors::green); // -
		pCanvas->Draw2DLine(mouse_position.X, ((mouse_position.Y) - (i_crosshair_size + 5)), mouse_position.X, ((mouse_position.Y) + (i_crosshair_size + 5)), SDK::colors::green); // |
	}

	SDK::FBox GetScreenBoundingBox(SDK::UCanvas* pCanvas, SDK::AActor* pActor)
	{
		SDK::FBox out_box{}, components_bounding_box{};
		SDK::FVector bounding_box_coordinates[8];
		pActor->GetComponentsBoundingBox(&components_bounding_box);

		bounding_box_coordinates[0].X = components_bounding_box.Min.X;
		bounding_box_coordinates[0].Y = components_bounding_box.Min.Y;
		bounding_box_coordinates[0].Z = components_bounding_box.Min.Z;
		bounding_box_coordinates[0] = pCanvas->Project(bounding_box_coordinates[0]);

		bounding_box_coordinates[1].X = components_bounding_box.Max.X;
		bounding_box_coordinates[1].Y = components_bounding_box.Min.Y;
		bounding_box_coordinates[1].Z = components_bounding_box.Min.Z;
		bounding_box_coordinates[1] = pCanvas->Project(bounding_box_coordinates[1]);

		bounding_box_coordinates[2].X = components_bounding_box.Min.X;
		bounding_box_coordinates[2].Y = components_bounding_box.Max.Y;
		bounding_box_coordinates[2].Z = components_bounding_box.Min.Z;
		bounding_box_coordinates[2] = pCanvas->Project(bounding_box_coordinates[2]);

		bounding_box_coordinates[3].X = components_bounding_box.Max.X;
		bounding_box_coordinates[3].Y = components_bounding_box.Max.Y;
		bounding_box_coordinates[3].Z = components_bounding_box.Min.Z;
		bounding_box_coordinates[3] = pCanvas->Project(bounding_box_coordinates[3]);

		bounding_box_coordinates[4].X = components_bounding_box.Min.X;
		bounding_box_coordinates[4].Y = components_bounding_box.Min.Y;
		bounding_box_coordinates[4].Z = components_bounding_box.Max.Z;
		bounding_box_coordinates[4] = pCanvas->Project(bounding_box_coordinates[4]);

		bounding_box_coordinates[5].X = components_bounding_box.Max.X;
		bounding_box_coordinates[5].Y = components_bounding_box.Min.Y;
		bounding_box_coordinates[5].Z = components_bounding_box.Max.Z;
		bounding_box_coordinates[5] = pCanvas->Project(bounding_box_coordinates[5]);

		bounding_box_coordinates[6].X = components_bounding_box.Min.X;
		bounding_box_coordinates[6].Y = components_bounding_box.Max.Y;
		bounding_box_coordinates[6].Z = components_bounding_box.Max.Z;
		bounding_box_coordinates[6] = pCanvas->Project(bounding_box_coordinates[6]);

		bounding_box_coordinates[7].X = components_bounding_box.Max.X;
		bounding_box_coordinates[7].Y = components_bounding_box.Max.Y;
		bounding_box_coordinates[7].Z = components_bounding_box.Max.Z;
		bounding_box_coordinates[7] = pCanvas->Project(bounding_box_coordinates[7]);

		out_box.Min.X = pCanvas->ClipX;
		out_box.Min.Y = pCanvas->ClipY;
		out_box.Max.X = 0;
		out_box.Max.Y = 0;

		for (auto& bounding_box_coordinate : bounding_box_coordinates) {
			if (out_box.Min.X > bounding_box_coordinate.X)
				out_box.Min.X = bounding_box_coordinate.X;

			if (out_box.Min.Y > bounding_box_coordinate.Y)
				out_box.Min.Y = bounding_box_coordinate.Y;

			if (out_box.Max.X < bounding_box_coordinate.X)
				out_box.Max.X = bounding_box_coordinate.X;

			if (out_box.Max.Y < bounding_box_coordinate.Y)
				out_box.Max.Y = bounding_box_coordinate.Y;
		}

		return out_box;
	}

	void DrawBox(SDK::UCanvas* pCanvas, float X, float Y, float Width, float Height, SDK::FColor Color)
	{
		const auto old_x = pCanvas->CurX;
		const auto old_y = pCanvas->CurY;

		const auto old_color = pCanvas->DrawColor;

		pCanvas->CurX = X;
		pCanvas->CurY = Y;

		pCanvas->DrawColor = Color;

		pCanvas->DrawBox(Width, Height);

		pCanvas->CurX = old_x;
		pCanvas->CurY = old_y;

		pCanvas->DrawColor = old_color;
	}

	void Draw3DBoundingBox(SDK::UCanvas *canvas, SDK::AEFPawn *pawn, const SDK::FColor& color)
	{
		SDK::FVector min{}, max{}, vec1{}, vec2{}, vec3{}, vec4{}, vec5{}, vec6{}, vec7{}, vec8{};

		SDK::FBox box{};
		
		pawn->GetComponentsBoundingBox(&box);

		if (box.IsValid)
		{
			min = box.Min;
			max = box.Max;

			vec3 = min;
			vec3.X = max.X;
			vec4 = min;
			vec4.Y = max.Y;
			vec5 = min;
			vec5.Z = max.Z;
			vec6 = max;
			vec6.X = min.X;
			vec7 = max;
			vec7.Y = min.Y;
			vec8 = max;
			vec8.Z = min.Z;
			vec1 = canvas->Project(min);
			vec2 = canvas->Project(max);
			vec3 = canvas->Project(vec3);
			vec4 = canvas->Project(vec4);
			vec5 = canvas->Project(vec5);
			vec6 = canvas->Project(vec6);
			vec7 = canvas->Project(vec7);
			vec8 = canvas->Project(vec8);
			canvas->Draw2DLine(vec1.X, vec1.Y, vec5.X, vec5.Y, color);
			canvas->Draw2DLine(vec2.X, vec2.Y, vec8.X, vec8.Y, color);
			canvas->Draw2DLine(vec3.X, vec3.Y, vec7.X, vec7.Y, color);
			canvas->Draw2DLine(vec4.X, vec4.Y, vec6.X, vec6.Y, color);
			canvas->Draw2DLine(vec1.X, vec1.Y, vec3.X, vec3.Y, color);
			canvas->Draw2DLine(vec1.X, vec1.Y, vec4.X, vec4.Y, color);
			canvas->Draw2DLine(vec8.X, vec8.Y, vec3.X, vec3.Y, color);
			canvas->Draw2DLine(vec8.X, vec8.Y, vec4.X, vec4.Y, color);
			canvas->Draw2DLine(vec2.X, vec2.Y, vec6.X, vec6.Y, color);
			canvas->Draw2DLine(vec2.X, vec2.Y, vec7.X, vec7.Y, color);
			canvas->Draw2DLine(vec5.X, vec5.Y, vec6.X, vec6.Y, color);
			canvas->Draw2DLine(vec5.X, vec5.Y, vec7.X, vec7.Y, color);
		}
	}

	void DrawPlayerSkeleton(SDK::UCanvas* pCanvas, SDK::AEFPawn* pEntity, SDK::FColor Color)
	{
		if (SDK::IsPet(pEntity) || SDK::IsVehicle(pEntity) || SDK::IsSummonNpc(pEntity))
			return;

		std::vector<std::vector<std::string>> bones_vector =
		{
		{ "Bip001-Head", "Bip001-Neck", "Bip001-Pelvis" },
		{ "Bip001-L-UpperArm", "Bip001-L-Clavicle", "Bip001-L-Forearm", "Bip001-L-Hand" },
		{ "bip001-l-thigh", "bip001-l-calf", "bip001-l-foot", "Bip001-R-UpperArm" },
		{ "Bip001-R-Clavicle", "Bip001-R-Forearm", "Bip001-R-Hand", "bip001-r-thigh" },
		{ "bip001-r-calf", "bip001-r-foot" }
		};

		for (auto p_bone_vector : bones_vector)
		{
			SDK::FVector fv_prev{}, fv_cur{};
			fv_prev = pCanvas->Project(pEntity->Mesh->GetBoneLocation(p_bone_vector.at(0).c_str(), 0));

			for (size_t bone = 1; bone < p_bone_vector.size(); bone++)
			{
				fv_cur = pCanvas->Project(pEntity->Mesh->GetBoneLocation(p_bone_vector.at(bone).c_str(), 0));
				if (!fv_cur.X || !fv_cur.Y || !fv_cur.Z)
					break;

				pCanvas->Draw2DLine(fv_prev.X, fv_prev.Y, fv_cur.X, fv_cur.Y, Color);

				fv_prev = fv_cur;
			}
		}
	}

	void LaserESP(SDK::UCanvas* pCanvas, SDK::AEFPawn* pEntity, const SDK::FColor& Color)
	{
		if (SDK::IsPet(pEntity) || SDK::IsVehicle(pEntity) || SDK::IsSummonNpc(pEntity))
			return;

		if (pEntity != nullptr && pCanvas != nullptr)
		{
			const double yaw = pEntity->Rotation.Yaw / 182;
			const double pitch = pEntity->RemoteViewPitch / 182;

			const auto radiants_yaw = ((yaw * CONST_Pi) / 180);
			const double radiants_pitch = ((pitch * CONST_Pi) / 180);
			const double hyp = 250;
			const double x = (hyp * cos(radiants_yaw));
			const double y = (hyp * sin(radiants_yaw));
			const double z = (hyp * sin(radiants_pitch));
			const float pl_x = pEntity->Location.X + x;
			const float pl_z = pEntity->Location.Y + y;
			const float pl_y = pEntity->GetPawnViewLocation().Z + z;

			SDK::FVector laser{};
			laser.X = pl_x;
			laser.Z = pl_y;
			laser.Y = pl_z;
			laser = pCanvas->Project(laser);

			SDK::FVector head_loc{};
			head_loc.X = pEntity->Location.X;
			head_loc.Y = pEntity->Location.Y;
			head_loc.Z = pEntity->Mesh->GetBoneLocation("Bip001-Head", 0).Z;
			head_loc = pCanvas->Project(head_loc);

			pCanvas->Draw2DLine(head_loc.X, head_loc.Y, laser.X, laser.Y, Color);
		}
	}

	void HeadCircle(SDK::UCanvas* pCanvas, SDK::AEFPawn* pEntity, int numSides, SDK::FColor Color)
	{
		if (SDK::IsPet(pEntity) || SDK::IsVehicle(pEntity) || SDK::IsSummonNpc(pEntity))
			return;

		if (!pEntity->Mesh)
			return;

		const auto world_head = pEntity->Mesh->GetBoneLocation("Bip001-Head", 0);

		auto head = world_head;
		head = pCanvas->Project(head);

		auto head_up = world_head;
		head_up.Z += 14;
		head_up = pCanvas->Project(head_up);

		auto head_left = world_head;
		head_left.X += 14;
		head_left = pCanvas->Project(head_left);

		auto head_right = world_head;
		head_right.Y += 14;
		head_right = pCanvas->Project(head_right);

		auto neck = pEntity->Mesh->GetBoneLocation("Bip001-Neck", 0);
		neck = pCanvas->Project(neck);

		pCanvas->Draw2DLine(neck.X, neck.Y, head.X, head.Y, Color);

		const int x = head.X;
		const int y = head.Y;

		auto radius = 0;

		int up = (head_up.Y - head.Y);
		int left = (x - head_left.X);
		int right = (x - head_right.X);

		if (up < 0) up = -up;
		if (left < 0) left = -left;
		if (right < 0) right = -right;

		if (up >= left && up >= right) radius = up;
		else if (left >= up && left >= right) radius = left;
		else if (right >= up && right >= up) radius = right;

		if (radius < 0) radius = -radius;

		const float step = CONST_Pi * 2.0 / numSides;
		const auto count = 0;
		SDK::FVector2D v[128];
		for (float a = 0; a < CONST_Pi * 2.0; a += step) {
			const auto x1 = radius * cos(a) + x;
			const auto y1 = radius * sin(a) + y;
			const auto x2 = radius * cos(a + step) + x;
			const auto y2 = radius * sin(a + step) + y;
			v[count].X = x1;
			v[count].Y = y1;
			v[count + 1].X = x2;
			v[count + 1].Y = y2;
			pCanvas->Draw2DLine(v[count].X, v[count].Y, x2, y2, Color);
		}
	}

		void DrawCircle(SDK::UCanvas* pCanvas, int X, int Y, int radius, int numSides, SDK::FColor Color)
		{
			const float step = CONST_Pi * 2.0 / numSides;
			const auto count = 0;
			SDK::FVector2D v[128];
			for (float a = 0; a < CONST_Pi * 2.0; a += step) {
				const auto x1 = radius * cos(a) + X;
				const auto y1 = radius * sin(a) + Y;
				const auto x2 = radius * cos(a + step) + X;
				const auto y2 = radius * sin(a + step) + Y;
				v[count].X = x1;
				v[count].Y = y1;
				v[count + 1].X = x2;
				v[count + 1].Y = y2;
				pCanvas->Draw2DLine(X, Y, x2, y2, Color); // Points from Centre to ends of circle
				pCanvas->Draw2DLine(v[count].X, v[count].Y, x2, y2, Color);// Circle Around
			}
		}
}
