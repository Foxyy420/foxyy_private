#include "menu.hpp"
#include "hooks.hpp"
#include "options.hpp"

static Vector RotatePoint(Vector EntityPos, Vector LocalPlayerPos, int posX, int posY, int sizeX, int sizeY, float angle, float zoom, bool* viewCheck)
{
	float r_1, r_2;
	float x_1, y_1;

	r_1 = -(EntityPos.y - LocalPlayerPos.y);
	r_2 = EntityPos.x - LocalPlayerPos.x;
	float Yaw = angle - 90.0f;

	float yawToRadian = Yaw * (float)(M_PI / 180.0F);
	x_1 = (float)(r_2 * (float)cos((double)(yawToRadian)) - r_1 * sin((double)(yawToRadian))) / 20;
	y_1 = (float)(r_2 * (float)sin((double)(yawToRadian)) + r_1 * cos((double)(yawToRadian))) / 20;

	*viewCheck = y_1 < 0;

	x_1 *= zoom;
	y_1 *= zoom;

	int sizX = sizeX / 2;
	int sizY = sizeY / 2;

	x_1 += sizX;
	y_1 += sizY;

	if (x_1 < 5)
		x_1 = 5;

	if (x_1 > sizeX - 5)
		x_1 = sizeX - 5;

	if (y_1 < 5)
		y_1 = 5;

	if (y_1 > sizeY - 5)
		y_1 = sizeY - 5;


	x_1 += posX;
	y_1 += posY;


	return Vector(x_1, y_1, 0);
}


void DrawRadar()
{
	ImGuiStyle& style = ImGui::GetStyle();
	ImVec2 oldPadding = style.WindowPadding;
	float oldAlpha = style.Colors[ImGuiCol_WindowBg].w;
	style.WindowPadding = ImVec2(0, 0);
	style.Colors[ImGuiCol_WindowBg].w = 0.7f;
	style.Colors[ImGuiCol_TitleBg] = ImColor(21, 21, 21, 255);
	style.Colors[ImGuiCol_TitleBgCollapsed] = ImColor(21, 21, 21, 255);
	style.Colors[ImGuiCol_TitleBgActive] = ImColor(21, 21, 21, 255);
	style.Colors[ImGuiCol_CloseButton] = ImColor(0, 0, 0, 0);
	style.Colors[ImGuiCol_CloseButtonHovered] = ImColor(0, 0, 0, 0);
	style.Colors[ImGuiCol_CloseButtonActive] = ImColor(0, 0, 0, 0);
	style.Alpha = 1.f;
	ImGui::SetNextWindowSize(ImVec2(300, 300));

	if (ImGui::Begin(("Radar"), &g_Options.Radar_window, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize))
	{
		ImDrawList* draw_list = ImGui::GetWindowDrawList();

		ImVec2 winpos = ImGui::GetWindowPos();
		ImVec2 winsize = ImGui::GetWindowSize();

		draw_list->AddLine(ImVec2(winpos.x + winsize.x * 0.5f, winpos.y), ImVec2(winpos.x + winsize.x * 0.5f, winpos.y + winsize.y), ImColor(70, 70, 70, 255), 1.f);
		draw_list->AddLine(ImVec2(winpos.x, winpos.y + winsize.y * 0.5f), ImVec2(winpos.x + winsize.x, winpos.y + winsize.y * 0.5f), ImColor(70, 70, 70, 255), 1.f);

		draw_list->AddLine(ImVec2(winpos.x + winsize.x * 0.5f, winpos.y + winsize.y * 0.5f), ImVec2(winpos.x, winpos.y), ImColor(90, 90, 90, 255), 1.f);
		draw_list->AddLine(ImVec2(winpos.x + winsize.x * 0.5f, winpos.y + winsize.y * 0.5f), ImVec2(winpos.x + winsize.x, winpos.y), ImColor(90, 90, 90, 255), 1.f);

		IClientEntity *pLocal = g_EntityList->GetClientEntity(g_EngineClient->GetLocalPlayer());


		if (g_EngineClient->IsInGame() && g_EngineClient->IsConnected())
		{

			Vector LocalPos = g_LocalPlayer->GetEyePos();
			QAngle ang;
			g_EngineClient->GetViewAngles(ang);

			for (int i = 0; i < g_EngineClient->GetMaxClients(); i++) {
				C_BasePlayer *pBaseEntity = (C_BasePlayer*)g_EntityList->GetClientEntity(i);

				bool enemy_only;

				if (!pBaseEntity)
					continue;
				//if (pBaseEntity->IsDormant())
					//continue;
				if (!pBaseEntity->m_iHealth() > 0)
					continue;
			
				if (g_LocalPlayer->m_iTeamNum() == pBaseEntity->m_iTeamNum())
					continue;


				bool viewCheck = false;
				Vector EntityPos = RotatePoint(pBaseEntity->GetRenderOrigin(), LocalPos, winpos.x, winpos.y, winsize.x, winsize.y, ang.yaw, 1.5f, &viewCheck);


				ImU32 clr = ImGui::GetColorU32(ImVec4(255, 0, 0, 255));

				int s = 3;

				draw_list->AddCircleFilled(ImVec2(EntityPos.x, EntityPos.y), s, clr);

			}

		}
	}
	ImGui::End();
	style.WindowPadding = oldPadding;
	style.Colors[ImGuiCol_WindowBg].w = oldAlpha;

}

