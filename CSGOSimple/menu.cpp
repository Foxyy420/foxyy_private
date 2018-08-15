#include "Menu.hpp"
#include <Windows.h>
#include <chrono>

#include "valve_sdk/csgostructs.hpp"
#include "helpers/input.hpp"
#include "options.hpp"
#include "Hitmarker.h"
#include "parser.h"
#include <algorithm>
#include <mftransform.h>
#include <cctype>

#include "imgui/imgui_internal.h"
#include "../CSGOSimple/imgui/directx9/imgui_impl_dx9.h"
#include "imgui/imgui.h"
#include "hooks.hpp"
#include "Fonts.hpp"
#include "features\visuals.hpp"
#include <ColorDlg.h>
#include "helpers/utils.hpp"
#include "imgui\imgui.h"
#include "Radar.h"
#include "XorStr.h"
#include <vector>
#include "SkinChanger.h"
#include "config.h"
#include "functions.hpp"

#include <functional>
#include <experimental/filesystem>

#define MENU_SIZE_X 540
#define MENU_SIZE_Y 890
#define TAB_COUNT 5
#define SUBTAB_COUNT 6

#include <locale>

std::vector<paint_kit> k_skins;
std::vector<std::string> skins_cc;

static ConVar* cl_mouseenable = nullptr;

void initialize()
{
	static bool once = false;

	if (!once)
	{
		InitializePaintKits();

		for (int i = 0; i < k_skins.size(); i++) {
			skins_cc.push_back(k_skins[i].name);
		}
		once = true;
	}
}

void DrawSpecialText(std::string text, std::string textBefore, bool sameLine = false, bool defaultColor = false)
{
	const auto style = &ImGui::GetStyle();
	if (defaultColor) ImGui::Text(text.c_str());
	else ImGui::TextColored(ImVec4(1.00f, 0.00f, 0.19f, 1.f), text.c_str());
	if (sameLine) ImGui::SameLine(style->WindowPadding.x + ImGui::CalcTextSize((textBefore + text).c_str()).x);
}


// =========================================================
// 
// These are the tabs on the sidebar
// 
// =========================================================

namespace ImGuiEx
{
	inline bool ColorEdit4(const char* label, Color* v, bool show_alpha = true)
	{
		auto clr = ImVec4{
			v->r() / 255.0f,
			v->g() / 255.0f,
			v->b() / 255.0f,
			v->a() / 255.0f
		};

		if (ImGui::ColorEdit4(label, &clr.x, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_AlphaBar)) {
			v->SetColor(clr.x, clr.y, clr.z, clr.w);
			return true;
		}
		return false;
	}
	inline bool ColorEdit3(const char* label, Color* v)
	{
		return ColorEdit4(label, v);
	}
}

ImFont* bestfont;
ImFont* bestee;

static bool IsKeyPressedMap(ImGuiKey key, bool repeat = true)
{
	const int key_index = GImGui->IO.KeyMap[key];
	return (key_index >= 0) ? ImGui::IsKeyPressed(key_index, repeat) : false;
}



namespace ImGui {
	static auto vector_getter = [](void* vec, int idx, const char** out_text)
	{
		auto& vector = *static_cast<std::vector<std::string>*>(vec);
		if (idx < 0 || idx >= static_cast<int>(vector.size())) { return false; }
		*out_text = vector.at(idx).c_str();
		return true;
	};

	bool Combo(const char* label, int* currIndex, std::vector<std::string>& values)
	{
		if (values.empty()) { return false; }
		return Combo(label, currIndex, vector_getter,
			static_cast<void*>(&values), values.size());
	}

	
	bool ListBox(const char* label, int* currIndex, std::vector<std::string>& values, int height_in_items = -1)
	{
		if (values.empty()) { return false; }
		return ListBox(label, currIndex, vector_getter,
			static_cast<void*>(&values), values.size(), height_in_items);
	}
	
	
	static bool ListBox(const char* label, int* current_item, std::function<const char*(int)> lambda, int items_count, int height_in_items)
	{
		return ImGui::ListBox(label, current_item, [](void* data, int idx, const char** out_text)
		{
			*out_text = (*reinterpret_cast< std::function<const char*(int)>* >(data))(idx);
			return true;
		}, &lambda, items_count, height_in_items);
	}
	
}


std::vector<std::pair<int, const char*>> weaponcomboname = {
{ WEAPON_DEAGLE, "Desert Eagle" },
{ WEAPON_ELITE, "Dual Berettas" },
{ WEAPON_FIVESEVEN, "Five-SeveN" },
{ WEAPON_GLOCK, "Glock-18" },
{ WEAPON_AK47, "AK-47" },
{ WEAPON_AUG, "AUG" },
{ WEAPON_AWP, "AWP" },
{ WEAPON_FAMAS, "FAMAS" },
{ WEAPON_G3SG1, "G3SG1" },
{ WEAPON_GALILAR, "Galil AR" },
{ WEAPON_M249, "M249" },
{ WEAPON_M4A1, "M4A4" },
{ WEAPON_MAC10, "MAC-10" },
{ WEAPON_P90, "P90" },
{ WEAPON_UMP45, "UMP-45" },
{ WEAPON_XM1014, "XM1014" },
{ WEAPON_BIZON, "PP-Bizon" },
{ WEAPON_MAG7, "MAG-7" },
{ WEAPON_NEGEV, "Negev" },
{ WEAPON_SAWEDOFF, "Sawed.Off" },
{ WEAPON_TEC9, "Tec-9" },
{ WEAPON_HKP2000, "P2000" },
{ WEAPON_MP7, "MP7" },
{ WEAPON_MP9, "MP9" },
{ WEAPON_NOVA, "Nova" },
{ WEAPON_P250, "P250" },
{ WEAPON_SCAR20, "SCAR-20" },
{ WEAPON_SG556, "SG 553" },
{ WEAPON_SSG08, "SSG 08" },
{ WEAPON_M4A1_SILENCER, "M4A1-S" },
{ WEAPON_USP_SILENCER, "USP-S" },
{ WEAPON_CZ75A, "CZ75-Auto" },
{ WEAPON_REVOLVER, "R8 Revolver" },
{ 10000, "Knife" }
};

void InitializePaintKits() {
	static std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	const auto sig_address = Utils::FindPattern("client.dll", (PBYTE)"\xE8\x00\x00\x00\x00\xFF\x76\x0C\x8D\x48\x04\xE8", "x????xxxxxxx");
	const auto item_system_offset = *reinterpret_cast<std::int32_t*>(sig_address + 1);
	const auto item_system_fn = reinterpret_cast<CCStrike15ItemSystem* (*)()>(sig_address + 5 + item_system_offset);
	const auto item_schema = reinterpret_cast<CCStrike15ItemSchema*>(std::uintptr_t(item_system_fn()) + sizeof(void*));

	const auto get_paint_kit_definition_offset = *reinterpret_cast<std::int32_t*>(sig_address + 11 + 1);
	const auto get_paint_kit_definition_fn = reinterpret_cast<CPaintKit*(__thiscall*)(CCStrike15ItemSchema*, int)>(sig_address + 11 + 5 + get_paint_kit_definition_offset);
	const auto start_element_offset = *reinterpret_cast<std::intptr_t*>(std::uintptr_t(get_paint_kit_definition_fn) + 8 + 2);
	const auto head_offset = start_element_offset - 12;
	const auto map_head = reinterpret_cast<Head_t<int, CPaintKit*>*>(std::uintptr_t(item_schema) + head_offset);

	for (auto i = 0; i <= map_head->last_element; ++i) {
		const auto paint_kit = map_head->memory[i].value;

		if (paint_kit->id == 9001)
			continue;

		const auto wide_name = g_Localize->Find(paint_kit->item_name.buffer);
		const auto name = converter.to_bytes(wide_name);

		if (paint_kit->id < 10000)
			k_skins.push_back({ paint_kit->id, name });
	}

	//std::sort(k_skins.begin(), k_skins.end());
	//k_skins.insert(k_skins.begin(), { 0, "None" });
}




template<size_t N>
void render_tabs(char* (&names)[N], int& activetab, float w, float h, bool sameline)
{
	bool values[N] = { false };

	values[activetab] = true;

	for (auto i = 0; i < N; ++i) {
	}
	{
	}
}

bool TextureImage = false;


void RenderSkinsTab()
{
	bool placeholder_true = true;

	auto& style = ImGui::GetStyle();
	float group_w = ImGui::GetCurrentWindow()->Size.x - style.WindowPadding.x * 1;

	ImGui::BeginChild("##skinstab", ImVec2(-1, 400), true, 0);
	{
		DrawSpecialText("Warning this skinchanger is still in BETA so things might not working properly.", "", false, false);

		ImGui::Columns(2, NULL, false);
		{
			ImGui::BeginChild("##Skins_part_1", ImVec2(0, 0), true);
			{
				ImGui::Spacing();
				ImGui::PushItemWidth(-1);
				ImGui::ListBox("##paintkit1", &selected_Weapon, weaponNamesForCombobox, ARRAYSIZE(weaponNamesForCombobox), 8);
				ImGui::PopItemWidth();
			}
			ImGui::EndChild();
		}

		ImGui::NextColumn();
		{
			ImGui::BeginChild("##Skins_part_2", ImVec2(0, 0), true);
			{
				ImGui::Spacing();
				//ImGui::Combo("##paintkit12", &g_Options.skins[weaponcomboname.at(selected_Weapon).first].weapon_skin_id, ImGui::vector_getter, static_cast<void*>(&skins_cc), skins_cc.size());
				ImGui::Combo("Skin", &g_Options.skins[weaponcomboname[selected_Weapon].first].weapon_skin_id, skins_cc);
				ImGui::InputInt("##seed", &g_Options.skins[weaponcomboname.at(selected_Weapon).first].weapon_seed, 0, 1000000000);
				ImGui::SliderFloat("##wear", &g_Options.skins[weaponcomboname.at(selected_Weapon).first].weapon_wear, 0.0f, 1.0f, "%0.6f");

				ImGui::Combo("Knifes", &g_Options.knifemodel, knife_models, ARRAYSIZE(knife_models));
				//ImGui::Combo("Knife Models", &g_Options.Mdl.Knife, knifemdl, ARRAYSIZE(knifemdl));
				
			}
			ImGui::EndChild();
		}
	}
	ImGui::EndChild();

	ImGui::PushItemWidth(-1);
	if (ImGui::Button("apply", ImVec2(-1, 40))) Skinchanger::Get().LoadSkins();

}

void RenderMiscTab()
{
	bool placeholder_true = true;


	auto& style = ImGui::GetStyle();
	float group_w = ImGui::GetCurrentWindow()->Size.x - style.WindowPadding.x * 1;

	ImGui::Columns(2, NULL, false);
	{
		ImGui::BeginChild("##misc", ImVec2(0, 0), true);
		{
			ImGui::Spacing();
			ImGui::BeginChild("##fasz", ImVec2(0, 150), true);
			{
				ImGui::Columns(2, NULL, false);
				{
					DrawSpecialText("Main Misc Features", (""), false, false);

					ImGui::Checkbox("Bunny hop##misc ", &g_Options.misc_bhop);
					//ImGui::Checkbox("Recoil Crosshair##misc ", &g_Options.Recoil_crosshair);
					ImGui::Checkbox("Auto Strafe##misc ", &g_Options.misc_autostrafe);
					ImGui::Checkbox("Auto Accept##misc ", &g_Options.auto_accept);
				}
				ImGui::NextColumn();
				{
					ImGui::Spacing();
					ImGui::Spacing();

					ImGui::Checkbox("HitMarker##misc ", &g_Options.misc_hitmarker);
					ImGui::Combo("HitSound##misc ", &g_Options.Hitsound, Hitsounds, ARRAYSIZE(Hitsounds));
				

				}
			}
			ImGui::EndChild();
			ImGui::BeginChild("##visual_misc", ImVec2(0, 170), true);
			{
				DrawSpecialText("Visuals", (""), false, false);

				ImGui::Checkbox("Enable Postprocess", &g_Options.Epost_process);
				ImGui::Checkbox("No hands", &g_Options.misc_no_hands);
				ImGui::SliderInt("ViewModel FOV", &g_Options.viewmodel_fov, 68, 150);
				ImGui::Checkbox("Show Demo Window  ", &g_Options.show_demo_window);
				ImGui::Checkbox("Lefthand", &g_Options.lefthand);


				//ImGui::Checkbox("Show Colors Window", &colors_window);
				//ImGui::SliderFloat("ambient_light_r:", &g_Options.mat_ambient_light_r, 0, 1);
				//ImGui::SliderFloat("ambient_light_g:", &g_Options.mat_ambient_light_g, 0, 1);
				//ImGui::SliderFloat("ambient_light_b:", &g_Options.mat_ambient_light_b, 0, 1);

			}
			ImGui::EndChild();

		}
		ImGui::EndChild();
	}
	ImGui::NextColumn();
	{
	
		ImGui::BeginChild("##misc2", ImVec2(0, 0), true);
		{
			ImGui::Spacing();
			ImGui::BeginChild("##chat", ImVec2(0, 150), true);
			{
				DrawSpecialText("Chat Things", (""), false, false);
			
				//ImGui::Checkbox("Animated Clantag", &g_Options.animated_clantag);

				static char textbuff[128];
				ImGui::InputText("##SetName", textbuff, 20);
				if (ImGui::Button("Set name"))
					Utils::SetName(textbuff);
				ImGui::SameLine();
				if (ImGui::Button("No name"))
					Utils::SetName("\n\xAD\xAD\xAD­­­");
				static char clanbuff[128];
				ImGui::InputText("##SetClan", clanbuff, 20);
				if (ImGui::Button("Set Clantag"))
					Utils::SetClantag(clanbuff);
			}
			ImGui::EndChild();

			static std::vector<std::string> configItems = ConfigSys::Get().GetAllConfigs();
			static int configItemCurrent = -1;

			static char fName[128] = "default";
			configItems = ConfigSys::Get().GetAllConfigs();
			ImGui::BeginChild("##config", ImVec2(0, 150), true);
			{
				if (ImGui::Button("Refresh##Config"))
					configItems = ConfigSys::Get().GetAllConfigs();

				ImGui::SameLine();
				if (ImGui::Button("Save##Config"))
				{
					if (configItems.size() > 0 && (configItemCurrent >= 0 && configItemCurrent < (int)configItems.size()))
					{
						std::string fPath = std::string(Global::my_documents_folder) + "\\Foxyy\\" + configItems[configItemCurrent] + ".config";
						ConfigSys::Get().SaveConfig(fPath);
					}
				}

				ImGui::SameLine();
				if (ImGui::Button("Remove##Config"))
				{
					if (configItems.size() > 0 && (configItemCurrent >= 0 && configItemCurrent < (int)configItems.size()))
					{
						std::string fPath = std::string(Global::my_documents_folder) + "\\Foxyy\\" + configItems[configItemCurrent] + ".config";
						std::remove(fPath.c_str());

						configItems = ConfigSys::Get().GetAllConfigs();
						configItemCurrent = -1;
					}
				}

				ImGui::PushItemWidth(138);
				{
					ImGui::InputText("", fName, 128);
				}
				ImGui::PopItemWidth();

				ImGui::SameLine();
				if (ImGui::Button("Add##Config"))
				{
					std::string fPath = std::string(Global::my_documents_folder) + "\\Foxyy\\" + fName + ".config";
					ConfigSys::Get().SaveConfig(fPath);

					configItemCurrent = -1;
				}

				ImGui::PushItemWidth(178);
				{
					if (ImGui::ListBox("", &configItemCurrent, configItems, 3))
					{
						std::string fPath = std::string(Global::my_documents_folder) + "\\Foxyy\\" + configItems[configItemCurrent] + ".config";
						ConfigSys::Get().LoadConfig(fPath);
					}
				}
				ImGui::PopItemWidth();

				/*
				ImGui::BeginChild(420, ImVec2(-1, 250), true, 0);

				for (std::vector<int>::size_type i = 0; i != configItems.size(); i++)
				{
					
					bool item_selectd = configItemCurrent == i;

					if (ImGui::Selectable(configItems[i].c_str(), item_selectd))
					{
						configItemCurrent = i;
					}
				}

				ImGui::EndChild();
				*/

			}ImGui::EndChild();
			

		}
		ImGui::EndChild();
	}

	/*
	ImGui::BeginChild("Misc", ImVec2(280, 300), true);

	DrawSpecialText("Misc", (""), true, false);
	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();


	ImGui::Checkbox("Bunny hop##misc ", &g_Options.misc_bhop);
	ImGui::Checkbox("Recoil Crosshair##misc ", &g_Options.recoil_crosshair);
	ImGui::Checkbox("Auto Strafe##misc ", &g_Options.misc_autostrafe);
	ImGui::Checkbox("Auto Accept##misc ", &g_Options.auto_accept); if (ImGui::IsItemHovered()) ImGui::SetTooltip("Accept Matches Automatically");
	ImGui::Checkbox("HitMarker##misc ", &g_Options.misc_hitmarker);
	ImGui::Combo("HitSound##misc ", &g_Options.Hitsound, Hitsounds, ARRAYSIZE(Hitsounds));
	ImGui::Checkbox("WaterMark##misc ", &g_Options.watermark);

	ImGui::EndChild();



	ImGui::SameLine();

	ImGui::BeginChild("Chat Things", ImVec2(235, 200), true);

	DrawSpecialText("Chat Things", (""), true, false);
	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	ImGui::Checkbox("Animated Clantag", &g_Options.animated_clantag);

	static char textbuff[128];
	ImGui::InputText("##inputtext", textbuff, 20);
	if (ImGui::Button("Set name"))
		Utils::SetName(textbuff);
	ImGui::SameLine();
	if (ImGui::Button("No name"))
		Utils::SetName("\n\xAD\xAD\xAD­­­");

	ImGui::Checkbox("Chatspam", &g_Options.chatspam);
	ImGui::EndChild();

	ImGui::SameLine();

	ImGui::BeginChild("Visual Misc", ImVec2(400, 250), true);
	DrawSpecialText("Visuals Misc", (""), true, false);
	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	ImGui::Checkbox("Night Sky", &g_Options.night_sky);
	ImGui::Checkbox("Enable Postprocess", &g_Options.Epost_process);
	ImGui::Checkbox("No hands", &g_Options.misc_no_hands);

	ImGui::SliderInt("viewmodel_fov:", &g_Options.viewmodel_fov, 68, 140);
    //ImGui::SliderFloat("ambient_light_r:", &g_Options.mat_ambient_light_r, 0, 1);
	//ImGui::SliderFloat("ambient_light_g:", &g_Options.mat_ambient_light_g, 0, 1);
	//ImGui::SliderFloat("ambient_light_b:", &g_Options.mat_ambient_light_b, 0, 1);
	ImGui::EndChild();

	ImGui::BeginChild("xd", ImVec2(920, 100), true);
	ImGui::Checkbox("Show Demo Window  ", &g_Options.show_demo_window);
	ImGui::SameLine();
	ImGui::Checkbox("Lefthand", &g_Options.lefthand);
	ImGui::Checkbox("Show Colors Window", &colors_window);


	ImGui::EndChild();

	*/
}



void RenderLegitBotTab()
{
	auto& style = ImGui::GetStyle();
	float group_w = ImGui::GetCurrentWindow()->Size.x - style.WindowPadding.x * 1;

	bool placeholder_true = true;

	const char* weapon_tabs[] = {
		u8"\uE007",
		u8"\uE001",
		u8"\uE009"
	};

	ImGui::SameLine();
	ImGui::BeginChild("Main Tab", ImVec2(0, 0), true);
	{
		ImGui::Separator();
		ImGui::Checkbox("Master Switch  ", &g_Options.enable_legitbot); ImGui::SameLine(); ImGui::Checkbox("BackTrack", &g_Options.misc_backtrack);

		ImGui::PushFont(bestee);
		static int subtab1 = 0;
		for (int i = 0; i < IM_ARRAYSIZE(weapon_tabs); i++)
		{
			int distance = i == subtab1 ? 0 : i > subtab1 ? i - subtab1 : subtab1 - i;

			if (distance != 0) {
				ImGui::GetStyle().Colors[ImGuiCol_Text] = ImVec4(1, 1, 1, 1.f);
				ImGui::GetStyle().Colors[ImGuiCol_Button] = ImVec4(0.075, 0.075, 0.075, 1.f);
			}
			if (ImGui::Button(weapon_tabs[i], ImVec2(286, 28)))
				subtab1 = i;

			ImGui::GetStyle().Colors[ImGuiCol_Text] = ImVec4(0.90, 0.90, 0.90, 1.f);
			ImGui::GetStyle().Colors[ImGuiCol_Button] = ImVec4(0.1f, 0.1f, 0.1f, 1.f);

			if (i < IM_ARRAYSIZE(weapon_tabs) - 1)
				ImGui::SameLine();
		}

		//ImGui::SelectTabs(&subtab1, weapon_tabs, ARRAYSIZE(weapon_tabs));

		ImGui::PopFont();

		const char* Hitboxx[] =
		{
			"",
			"Head",
			"Neck",
			"Chest",
			"Stomach",
			"Multipoint"

		};

		switch (subtab1)
		{
		case 0:

			ImGui::Columns(2, NULL, false);
			{
				ImGui::BeginChild("Legit_1", ImVec2(0, 0), true);
				{
					DrawSpecialText("LegitBot", "", false, false);
					ImGui::Checkbox("Enable Aim", &g_Options.aim_LegitBotRifles);
					ImGui::Hotkey("Key##0", &g_Options.aimkey, ImVec2(0, 0));
					ImGui::Combo("Hitbox", &g_Options.hitbox_rifles, Hitboxx, ARRAYSIZE(Hitboxx));
					ImGui::SliderFloat("Smooth##0", &g_Options.legit_smooth_rifles, 1.00f, 100.00f, "%.2f");
					ImGui::SliderFloat("FOV##0", &g_Options.legit_fov_rifles, 0.00f, 30.00f, "%.2f");
					ImGui::SliderFloat("Min RCS##0", &g_Options.legit_rcsmin_rifles, 1.00f, 100.00f, "%.2f");
					ImGui::SliderFloat("Max RCS##0", &g_Options.legit_rcsmax_rifles, 1.00f, 100.00f, "%.2f");
					//ImGui::Checkbox("Test", &g_Options.test);
				}
				ImGui::EndChild();
			}
			ImGui::NextColumn();
			{
				ImGui::BeginChild("Legit_2", ImVec2(0, 0), true);
				{
					DrawSpecialText("TriggerBot", "", false, false);
					ImGui::Checkbox("Active", &g_Options.Trigger.Rifles_Enable);
					ImGui::Hotkey("Key##TriggerKey", &g_Options.Trigger.Rifles_Key, ImVec2(0, 0));
					ImGui::BeginChild(("Filter"), ImVec2(ImGui::GetWindowContentRegionWidth() * 0.2f, 21 * 5), false);
					{
						ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0, 0, 0, 0));
						ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0, 0, 0, 0));
						ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0, 0, 0, 0));
						ImGui::Selectable(("Head"), &g_Options.Trigger.Rifles_Head);
						ImGui::Selectable(("Arms"), &g_Options.Trigger.Rifles_Arms);
						ImGui::Selectable(("Chest"), &g_Options.Trigger.Rifles_Chest);
						ImGui::Selectable(("Stomach"), &g_Options.Trigger.Rifles_Stomach);
						ImGui::Selectable(("Legs"), &g_Options.Trigger.Rifles_Legs);
						ImGui::PopStyleColor();
						ImGui::PopStyleColor();
						ImGui::PopStyleColor();
					}ImGui::EndChild();

				}ImGui::EndChild();
			}
			ImGui::EndColumns();
			break;
		case 1:

			ImGui::Columns(2, NULL, false);
			{
				ImGui::BeginChild("Legit_1", ImVec2(0, 0), true);
				{
					DrawSpecialText("LegitBot", "", false, false);
					ImGui::Checkbox("Enable Aim", &g_Options.aim_LegitBotPistols);
					//ImGui::Hotkey("Key##0", &g_Options.aimkey, ImVec2(0, 0));
					ImGui::Combo("Hitbox", &g_Options.hitbox_pistols, Hitboxx, ARRAYSIZE(Hitboxx));
					ImGui::SliderFloat("Smooth##0", &g_Options.legit_smooth_pistols, 1.00f, 100.00f, "%.2f");
					ImGui::SliderFloat("FOV##0", &g_Options.legit_fov_pistols, 0.00f, 30.00f, "%.2f");
					ImGui::SliderFloat("Min RCS##0", &g_Options.legit_rcsmin_pistols, 1.00f, 100.00f, "%.2f");
					ImGui::SliderFloat("Max RCS##0", &g_Options.legit_rcsmax_pistols, 1.00f, 100.00f, "%.2f");
				}
				ImGui::EndChild();
			}
			ImGui::NextColumn();
			{
				ImGui::BeginChild("Legit_2", ImVec2(0, 0), true);
				{
					DrawSpecialText("TriggerBot", "", false, false);
					ImGui::Checkbox("Active", &g_Options.Trigger.Pistols_Enable);
					ImGui::Hotkey("Key##TriggerKey", &g_Options.Trigger.Pistols_Key, ImVec2(0, 0));
					ImGui::BeginChild(("Filter"), ImVec2(ImGui::GetWindowContentRegionWidth() * 0.2f, 21 * 5), false);
					{
						ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0, 0, 0, 0));
						ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0, 0, 0, 0));
						ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0, 0, 0, 0));
						ImGui::Selectable(("Head"), &g_Options.Trigger.Pistols_Head);
						ImGui::Selectable(("Arms"), &g_Options.Trigger.Pistols_Arms);
						ImGui::Selectable(("Chest"), &g_Options.Trigger.Pistols_Chest);
						ImGui::Selectable(("Stomach"), &g_Options.Trigger.Pistols_Stomach);
						ImGui::Selectable(("Legs"), &g_Options.Trigger.Pistols_Legs);
						ImGui::PopStyleColor();
						ImGui::PopStyleColor();
						ImGui::PopStyleColor();
					}ImGui::EndChild();

				}ImGui::EndChild();
			}
			ImGui::EndColumns();
			break;
		case 2:

			ImGui::Columns(2, NULL, false);
			{
				ImGui::BeginChild("Legit_1", ImVec2(0, 0), true);
				{
					DrawSpecialText("LegitBot", "", false, false);
					ImGui::Checkbox("Enable Aim", &g_Options.aim_LegitBotSnipers);
					//ImGui::Hotkey("Key##0", &g_Options.aimkey, ImVec2(0, 0));
					ImGui::Combo("Hitbox", &g_Options.hitbox_snipers, Hitboxx, ARRAYSIZE(Hitboxx));
					ImGui::SliderFloat("Smooth##0", &g_Options.legit_smooth_Snipers, 1.00f, 100.00f, "%.2f");
					ImGui::SliderFloat("FOV##0", &g_Options.legit_fov_Snipers, 0.00f, 30.00f, "%.2f");
					ImGui::SliderFloat("Min RCS##0", &g_Options.legit_rcsmin_Snipers, 1.00f, 100.00f, "%.2f");
					ImGui::SliderFloat("Max RCS##0", &g_Options.legit_rcsmax_Snipers, 1.00f, 100.00f, "%.2f");
				}
				ImGui::EndChild();
			}
			ImGui::NextColumn();
			{
				ImGui::BeginChild("Legit_2", ImVec2(0, 0), true);
				{
					DrawSpecialText("TriggerBot", "", false, false);
					ImGui::Checkbox("Active", &g_Options.Trigger.Snipers_Enable);
					ImGui::Hotkey("Key##TriggerKey", &g_Options.Trigger.Snipers_Key, ImVec2(0, 0));
					ImGui::BeginChild(("Filter"), ImVec2(ImGui::GetWindowContentRegionWidth() * 0.2f, 21 * 5), false);
					{
						ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0, 0, 0, 0));
						ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0, 0, 0, 0));
						ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0, 0, 0, 0));
						ImGui::Selectable(("Head"), &g_Options.Trigger.Snipers_Head);
						ImGui::Selectable(("Arms"), &g_Options.Trigger.Snipers_Arms);
						ImGui::Selectable(("Chest"), &g_Options.Trigger.Snipers_Chest);
						ImGui::Selectable(("Stomach"), &g_Options.Trigger.Snipers_Stomach);
						ImGui::Selectable(("Legs"), &g_Options.Trigger.Snipers_Legs);
						ImGui::PopStyleColor();
						ImGui::PopStyleColor();
						ImGui::PopStyleColor();
					}ImGui::EndChild();

				}ImGui::EndChild();
			}
			ImGui::EndColumns();
			break;

		}
	}
	ImGui::EndChild();

}


IDirect3DTexture9 *tImage = nullptr;

void RenderRageBotTab()
{
	auto& style = ImGui::GetStyle();
	float group_w = ImGui::GetCurrentWindow()->Size.x - style.WindowPadding.x * 1;

	bool placeholder_true = true;

	const char* aa_tabs[] = {
		"Stand",
		"Move",
		"Edge"
	};

	static int aa_tab = 0;
	


	DrawSpecialText("Warning this ragebot is not finished so its unusable in hvh and you can get untrusted ban!", "", false, false);

	ImGui::Columns(2, NULL, false);
	{
		ImGui::BeginChild("##Visuals_part_1", ImVec2(0, 0), true);
		{
			ImGui::Checkbox(XorStr("Enabled"), &g_Options.aim_enabled);
			ImGui::Checkbox(XorStr("Auto Shoot"), &g_Options.aim_auto_shoot);//
			ImGui::Checkbox(XorStr("Silent"), &g_Options.aim_silent_mode);//
			ImGui::Checkbox(XorStr("NoRecoil"), &g_Options.Remove_recoil);//
			ImGui::Checkbox(XorStr("AutoWall"), &g_Options.auto_wall); //
			ImGui::Checkbox(XorStr("MultiPoint"), &g_Options.multipointer);//
			ImGui::SliderFloat(XorStr("FOV"), &g_Options.aim_fov, 1.f, 180.f);//
			//ImGui::SliderFloat("HItchance", &g_Options.hitchance_amount, 0.f, 100.f);
			ImGui::SliderFloat(XorStr("Min DMG"), &g_Options.auto_wall_mindamange, 1.f, 100.f);
			ImGui::Checkbox(XorStr("Hitchance"), &g_Options.hit_chance);
			ImGui::SliderFloat(XorStr("Hitchance"), &g_Options.hit_chanceamount, 0.f, 100.f);
			ImGui::Separator();
			ImGui::Text("Fakelag");
			ImGui::Separator();

		}
		ImGui::EndChild();

		ImGui::NextColumn();
		{
			ImGui::BeginChild("##Visuals_part_2", ImVec2(0, 0), true);
			{
				ImGui::Checkbox(XorStr("Engine Prediction"), &g_Options.enginepred);
				ImGui::Checkbox(XorStr("Vecvelocity prediction"), &g_Options.vecvelocityprediction);
				ImGui::Checkbox(XorStr("Resolver"), &g_Options.best_resolver);

			}
			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Checkbox(XorStr("Enable Fakewalk"), &g_Options.misc_fakewalk);
			ImGui::Combo(XorStr("Fakewalk Key"), &g_Options.misc_fakewalk_key, keyNames, ARRAYSIZE(keyNames));
			ImGui::Separator();
			ImGui::Combo("Pitch", &g_Options.antiaim_pitch, AntiaimPitchNames, ARRAYSIZE(AntiaimPitchNames));
			ImGui::Combo("Real Yaw", &g_Options.antiaim_yaw, AntiaimYawNames, ARRAYSIZE(AntiaimYawNames));
			ImGui::Combo("Fake Yaw", &g_Options.antiaim_yaw_fake, AntiaimYawNames, ARRAYSIZE(AntiaimYawNames));
			ImGui::Hotkey("Tank Key##0", &g_Options.tankAntiaimKey, ImVec2(0, 0));

			ImGui::EndChild();
		}
	}


}

/*




*/

void RenderVisualsTab()
{
	static const char* esp_tabs[] = {
	"ESP",
	"ESP 2"
	};
	static int page = 0;
	//---------------------------------

	auto& style = ImGui::GetStyle();
	float group_w = ImGui::GetCurrentWindow()->Size.x - style.WindowPadding.x * 1;

	bool placeholder_true = true;
	

	ImGui::Columns(2, NULL, false);
	{
		

		ImGui::BeginChild("##Visuals_part_1", ImVec2(0, 0), true);
		{
			ImGui::Spacing();
			ImGui::SelectTabs(&page, esp_tabs, ARRAYSIZE(esp_tabs));

			ImGui::BeginChild("##Visuals_Main_Part", ImVec2(0, 250), true);
			{
				switch (page)
				{
				case 0:
					ImGui::Spacing();
					ImGui::Checkbox("Enable ESP", &g_Options.esp_enabled);
					ImGui::Combo("Box Type  ", &g_Options.esp_boxtype, BoxType, ARRAYSIZE(BoxType)); ImGui::SameLine(); ImGuiEx::ColorEdit4("##esp_box_enemyy", &g_Options.color_esp_enemy_visible, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_AlphaBar); ImGui::SameLine(); ImGui::SameLine(); ImGuiEx::ColorEdit4("##esp_box_eneym_occludedd", &g_Options.color_esp_enemy_occluded, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_AlphaBar);
					ImGui::Combo("Healthbar Type  ", &g_Options.HealthBar_Style, HealthBar, ARRAYSIZE(HealthBar));
					ImGui::Combo("Wep Box Type ", &g_Options.esp_dropped_weapons, weaponEspStyle, ARRAYSIZE(weaponEspStyle)); ImGui::SameLine(); ImGuiEx::ColorEdit4("##esp_box_weapons", &g_Options.color_esp_weapons, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_AlphaBar);
					ImGui::Combo("C4 Box Type  ", &g_Options.esp_dropped_c4, weaponEspStyle, ARRAYSIZE(weaponEspStyle)); ImGui::SameLine(); ImGuiEx::ColorEdit4("##esp_box_c4", &g_Options.color_esp_c4, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_AlphaBar);
					//ImGui::Combo("Kit Box Type ", &g_Options.esp_dropped_Kits, weaponEspStyle, ARRAYSIZE(weaponEspStyle)); ImGui::SameLine(); ImGuiEx::ColorEdit4("##esp_box_weapons", &g_Options.color_esp_weapons, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_AlphaBar);


					ImGui::Columns(2, NULL, false);
					{
						ImGui::Checkbox("Enemies Only", &g_Options.esp_enemies_only);
						ImGui::Checkbox("Names", &g_Options.esp_player_names);
						ImGui::Checkbox("Angle Lines     ", &g_Options.esp_player_anglelines);
						//ImGui::Checkbox("Players Money", &g_Options.esp_player_money);
						ImGui::Checkbox("Health Text", &g_Options.esp_health_text);
						//ImGui::Checkbox("Flags", &g_Options.esp_is_flashed);
						ImGui::Checkbox("Armour", &g_Options.esp_player_armour);
						ImGui::Checkbox("LagComp Hitboxes", &g_Options.lagcomp_hitboxes);
						

					}
					ImGui::NextColumn();
					{
						ImGui::Checkbox("Fill Box         ", &g_Options.esp_filled_box); ImGui::SameLine(); ImGuiEx::ColorEdit4(" ##esp_box_enemy_filledd", &g_Options.color_esp_box_filled, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreview);
						ImGui::Checkbox("Skeleton         ", &g_Options.esp_skeleton); ImGui::SameLine(); ImGuiEx::ColorEdit4(" ##skeleton", &g_Options.color_esp_skeleton, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoSidePreview);
						ImGui::Checkbox("Players Weapon   ", &g_Options.esp_player_weapons); ImGui::SameLine(); ImGuiEx::ColorEdit4("##pl_weapons", &g_Options.color_esp_pl_weapons, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_AlphaBar);
						ImGui::Checkbox("DLight           ", &g_Options.enable_dlight); ImGui::SameLine(); ImGui::ColorEdit4(" ##dlight", g_Options.DLight_color, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_AlphaBar);
						ImGui::Checkbox("Grenade Prediction", &g_Options.greane_prediction);
						ImGui::SliderFloat("Duration", &g_Options.visuals_lagcomp_duration, 0.5, 5);
					}
					ImGui::Spacing();
					break;
				case 1:

					ImGui::Columns(2, NULL, false);
					{
						ImGui::Spacing();
						ImGui::Checkbox("Snaplines", &g_Options.esp_player_snaplines);
						ImGui::Checkbox("Bullet Trace     ", &g_Options.misc_bullettracers); ImGui::SameLine(); ImGui::ColorEdit4(" ##bullettrace", g_Options.misc_bullettracers_color, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_AlphaBar);
						ImGui::Checkbox("Bullet Beams     ", &g_Options.esp_bullet_beams); ImGui::SameLine(); ImGuiEx::ColorEdit4(" ##esp_bullet_beams", &g_Options.color_esp_bullet_beams, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_AlphaBar);
					}
					ImGui::NextColumn();
					{
						ImGui::Spacing();
						ImGui::Checkbox("Recoil Crosshair  ", &g_Options.Recoil_crosshair);
						ImGui::SliderFloat("Duration##bullettrace", &g_Options.esp_bullettrace_length, 0.5, 3);
						ImGui::SliderFloat("Duration##bulletbeam", &g_Options.esp_bulletbeams_length, 0.5, 3);
						ImGui::Separator();
						ImGui::Checkbox("Enabled", &g_Options.glow_enabled);
						ImGui::Checkbox("Team check", &g_Options.glow_enemies_only);
						ImGui::Checkbox("Players", &g_Options.glow_players);
						ImGui::Checkbox("Weapons", &g_Options.glow_weapons);
					}

					

					/*
					ImGui::Columns(2, NULL, false);
					{

						ImGui::Checkbox("Enable Glow", &g_Options.glow_enabled);
						ImGui::Combo("Glow Stlye", &g_Options.players_glow_style, GlowStlye, ARRAYSIZE(GlowStlye));
						ImGui::Checkbox("Enemies Only     ", &g_Options.glow_enemies_only);  ImGui::SameLine(); ImGuiEx::ColorEdit4("##glow color ally", &g_Options.color_glow_ally, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_AlphaBar);
						ImGui::Checkbox("Players          ", &g_Options.glow_players); ImGui::SameLine(); ImGuiEx::ColorEdit4("##glow color enemy", &g_Options.color_glow_enemy, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_AlphaBar);
						ImGui::Checkbox("Chickens         ", &g_Options.glow_chickens); ImGui::SameLine(); ImGuiEx::ColorEdit4("##glow color chickens", &g_Options.color_glow_chickens, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_AlphaBar);
					}
					ImGui::NextColumn();
					{

						ImGui::Checkbox("C4	Carrier		  ", &g_Options.glow_c4_carrier); ImGui::SameLine(); ImGuiEx::ColorEdit4("##glow color c4 carrier", &g_Options.color_glow_c4_carrier, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_AlphaBar);
						ImGui::Checkbox("Planted C4       ", &g_Options.glow_planted_c4); ImGui::SameLine(); ImGuiEx::ColorEdit4("##glow color planted c4", &g_Options.color_glow_planted_c4, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_AlphaBar);
						ImGui::Checkbox("Defuse Kits      ", &g_Options.glow_defuse_kits); ImGui::SameLine(); ImGuiEx::ColorEdit4("##glow color defuse kits", &g_Options.color_glow_defuse, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_AlphaBar);
						ImGui::Checkbox("Weapons          ", &g_Options.glow_weapons); ImGui::SameLine(); ImGuiEx::ColorEdit4("##glow color weapons", &g_Options.color_glow_weapons, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_AlphaBar);
					}
					*/

					break;
				}


			}ImGui::EndChild();
			
			/*
			ImGui::BeginChild("Flags", ImVec2(0, 130), true);
			{
				DrawSpecialText("Flags", (""), true, false);
				ImGui::Spacing();
				ImGui::Selectable(" Has Defuser", &g_Options.esp_flags_hasdefuser);
				ImGui::Selectable(" Is Flashed", &g_Options.esp_flags_is_flashed);
				ImGui::Selectable(" Is Scoped", &g_Options.esp_flags_scoped);
				ImGui::Selectable(" Bomb Carrier", &g_Options.esp_flags_bomb_carrier);
				ImGui::Selectable(" Hostage Carrier", &g_Options.esp_flags_hostage_carrier);
				ImGui::Selectable(" Is Rescuing", &g_Options.esp_flags_rescuing);

			}
			ImGui::EndChild();
			*/

			ImGui::BeginChild("##world", ImVec2(0, 170), true);
			ImGui::Spacing();
			ImGui::Columns(2, NULL, false);
			{
				DrawSpecialText("World Things", (""), true, false);
				ImGui::Spacing();
				ImGui::Checkbox("Gray World", &g_Options.gray_world);
				ImGui::Checkbox("Minekraft Mode", &g_Options.minecraft_mode);
				ImGui::Checkbox("MLG Mode", &g_Options.mlg_mode);
			//	
				ImGui::Checkbox("NightMode   ", &g_Options.nightmode); ImGui::SameLine(); ImGui::ColorEdit4("##xddd", g_Options.sky_color, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoAlpha);
				ImGui::SliderFloat("Intesity", &g_Options.nightmode_intensity, 0.05, 0.45);
				ImGui::Checkbox("ThirdPerson", &g_Options.thirdperson_enabled); if (ImGui::IsItemHovered()) ImGui::SetTooltip("Use the up and down arrows");
			}
			ImGui::NextColumn();
			{
				DrawSpecialText("Remove", (""), false, false);
				ImGui::Checkbox("Remove Flash", &g_Options.esp_no_flash);
				ImGui::Checkbox("Remove Smoke", &g_Options.esp_no_smoke);
				ImGui::Checkbox("No Visual Recoil", &g_Options.esp_no_visual_recoil);
			}


			ImGui::EndChild();

		}
		ImGui::EndChild();



	}
	ImGui::NextColumn();
	{
		ImGui::BeginChild("##Visuals_part_2", ImVec2(0, 0), true);
		{
			ImGui::Spacing();
			ImGui::BeginChild("##csams", ImVec2(0, 220), true);
			DrawSpecialText("Chams", (""), true, false); if (ImGui::IsItemHovered()) ImGui::SetTooltip("Chams will be updated soon");
			ImGui::Spacing();
			

			if (ImGui::TreeNode("Players"))
			{


				ImGui::Spacing();
				ImGui::Spacing();

				ImGui::Checkbox("Chams               ", &g_Options.chams_player_enabled); ImGui::SameLine(); ImGuiEx::ColorEdit4("##chamms", &g_Options.color_chams_player_enemy_visible, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreview); ImGui::SameLine(); ImGuiEx::ColorEdit4("##chams_enemy", &g_Options.color_chams_player_enemy_occluded, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreview);
				ImGui::Checkbox("Team Check          ", &g_Options.chams_player_enemies_only); ImGui::SameLine(); ImGuiEx::ColorEdit4("##chamms_enemyyy", &g_Options.color_chams_player_ally_visible, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreview); ImGui::SameLine(); ImGuiEx::ColorEdit4("##chams_enemyyyyy   ", &g_Options.color_chams_player_ally_occluded, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreview);
				ImGui::Checkbox("Wireframe           ", &g_Options.chams_player_wireframe);
				ImGui::Checkbox("Flat                ", &g_Options.chams_player_flat);
				ImGui::Checkbox("Ignore-Z            ", &g_Options.chams_player_ignorez);
				ImGui::Checkbox("Glass               ", &g_Options.chams_player_glass);
				ImGui::TreePop();
			}

			



			if (ImGui::TreeNode("Arms"))
			{
				ImGui::Spacing();
				ImGui::Spacing();

				ImGui::Checkbox(XorStr("Enable Chams   "), &g_Options.chams_arms_enabled); ImGui::SameLine(); ImGuiEx::ColorEdit4("##csams_arms", &g_Options.color_chams_arms_visible, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_AlphaBar);
				ImGui::Checkbox(XorStr("Wireframe      "), &g_Options.chams_arms_wireframe);
				ImGui::Checkbox(XorStr("Flat           "), &g_Options.chams_arms_flat);
				ImGui::Checkbox(XorStr("Ignore-Z       "), &g_Options.chams_arms_ignorez); ImGui::SameLine(); ImGuiEx::ColorEdit4("##csams_arms_occluded", &g_Options.color_chams_arms_occluded, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_AlphaBar);
				ImGui::Checkbox(XorStr("Glass          "), &g_Options.chams_arms_glass);
				ImGui::TreePop();

			}

			if (ImGui::TreeNode("Weapons"))
			{
				ImGui::Spacing();
				ImGui::Spacing();

				ImGui::Checkbox(XorStr("Enable Chams   "), &g_Options.chams_weapon_enabled); ImGui::SameLine(); ImGuiEx::ColorEdit4("##weapon_chahhms", &g_Options.color_chams_weapon_visible, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_AlphaBar);
				ImGui::Checkbox(XorStr("Wireframe      "), &g_Options.chams_weapon_wireframe);
				ImGui::Checkbox(XorStr("Flat           "), &g_Options.chams_weapon_flat);
				ImGui::Checkbox(XorStr("Glass          "), &g_Options.chams_weapon_glass);
				ImGui::TreePop();
			}

			ImGui::Spacing();
			ImGui::Spacing();
			ImGui::Spacing();


			ImGui::EndChild();

			ImGui::BeginChild(XorStr("##word things"), ImVec2(0, 100), true);


			ImGui::Spacing();
			DrawSpecialText(XorStr("SkyBox"), XorStr(""), true, false);
			ImGui::Spacing();

			ImGui::Combo(XorStr("Skyboxes"), &g_Options.skybox, Skynames, ARRAYSIZE(Skynames));

			ImGui::EndChild();

			ImGui::BeginChild(XorStr("##Windows"), ImVec2(0, 115), true);
			ImGui::Spacing();
			DrawSpecialText(XorStr("Misc Visuals"), XorStr(""), false, false);
			ImGui::Checkbox(XorStr("Spectator List"), &g_Options.spectator_list);
			ImGui::Checkbox(XorStr("Radar Window"), &g_Options.Radar_window);
			ImGui::SliderFloat(XorStr("Radar Alpha"), &g_Options.radar_alpha,0.250,1);


			ImGui::EndChild();


		}
		ImGui::EndChild();
	}
}

void RenderColors() {

	auto& style = ImGui::GetStyle();
	float group_w = ImGui::GetCurrentWindow()->Size.x - style.WindowPadding.x * 1;

	bool placeholder_true = true;

	DrawSpecialText("Informations", (""), true, false);

	ImGui::Spacing();
}



void Menu::Initialize()
{
	_visible = true;

	ImGui_ImplDX9_Init(InputSys::Get().GetMainWindow(), g_D3DDevice9);

	cl_mouseenable = g_CVar->FindVar("cl_mouseenable");

	CreateStyle();
}

void Menu::Shutdown()
{
	ImGui_ImplDX9_Shutdown();
	cl_mouseenable->SetValue(true);
}

void Menu::OnDeviceLost()
{
	ImGui_ImplDX9_InvalidateDeviceObjects();
}

void Menu::OnDeviceReset()
{
	ImGui_ImplDX9_CreateDeviceObjects();
}

/*____________________________________________________________________________________________*/




void RainbowMeme(int x, int y, int width, int height, float flSpeed)
{
	ImDrawList* window_draw_list = ImGui::GetWindowDrawList();

	static float flRainbow;

	flRainbow += flSpeed;

	if (flRainbow > 1.f) flRainbow = 0.f;

	for (int i = 0; i < width; i++)
	{
		float hue = (1.f / float(width)) * i;

		hue -= flRainbow;

		if (hue < 0.f) hue += 1.f;

		Color color = Color::FromHSB(hue, 1.f, 1.f);

		window_draw_list->AddRectFilled(ImVec2(x + i, y), ImVec2(width, height), ImColor(color.r(), color.g(), color.b(), color.a()));
	}
}

void SpecListStyle()
{
	ImVec4* colorss = ImGui::GetStyle().Colors;
	colorss[ImGuiCol_WindowBg].w = g_Options.specl_list_alpha;
	colorss[ImGuiCol_TitleBg] = ImColor(21, 21, 21, 255);
	colorss[ImGuiCol_TitleBgCollapsed] = ImColor(21, 21, 21, 255);
	colorss[ImGuiCol_TitleBgActive] = ImColor(21, 21, 21, 255);
	colorss[ImGuiCol_CloseButton] = ImColor(0, 0, 0, 0);
	colorss[ImGuiCol_CloseButtonHovered] = ImColor(0, 0, 0, 0);
	colorss[ImGuiCol_CloseButtonActive] = ImColor(0, 0, 0, 0);

}

static void SquareConstraint(ImGuiSizeConstraintCallbackData *data)
{
	data->DesiredSize = ImVec2(max(data->DesiredSize.x, data->DesiredSize.y), max(data->DesiredSize.x, data->DesiredSize.y));
}

void Menu::Render()
{

	ImVec2 mainWindowPos;

	ImGui_ImplDX9_NewFrame();



	ImGui::SetNextWindowSize(ImVec2(300, 150));

	ImGui::GetIO().MouseDrawCursor = _visible;

	ImGuiStyle &style = ImGui::GetStyle();
	if (_visible)
	{
		if (style.Alpha > 1.f)
			style.Alpha = 1.f;
		else if (style.Alpha != 1.f)
			style.Alpha += 0.01f;
	}
	else
	{
		if (style.Alpha < 0.f)
			style.Alpha = 0.f;
		else if (style.Alpha != 0.f)
			style.Alpha -= 0.02f;
	}



	int pX, pY;
	int y, h;
	g_EngineClient->GetScreenSize(y, h);


	char nameChar[64];



	ImGui::SetNextWindowSize(ImVec2(890, 540));
	if (_visible)
	{
		if (ImGui::Begin(u8"cool cheat name", &_visible, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar)) {


			static const char* sidebar_tabs[] = {
				ICON_FA_CROSSHAIRS " LegitBot" ,
				ICON_FA_FROWN_O " RageBot",
				ICON_FA_EYE  " Visuals",
				ICON_FA_FOLDER_OPEN " Misc",
				ICON_FA_PAINT_BRUSH " Skins"
			};



			static float flRainbow;
			float flSpeed = 0.0008f;

			ImVec2 curPos = ImGui::GetCursorPos();
			ImVec2 curWindowPos = ImGui::GetWindowPos();
			curPos.x += curWindowPos.x;
			curPos.y += curWindowPos.y;

			int size;
			int y;



			g_EngineClient->GetScreenSize(y, size);
			RainbowMeme(curPos.x - 10, curPos.y + 32, ImGui::GetWindowSize().x + size, curPos.y + 34, flSpeed);


			if (g_Options.show_demo_window)
			{
				ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiCond_FirstUseEver); // Normally user code doesn't need/want to call this because positions are saved in .ini file anyway. Here we just want to make the demo initial state a bit more friendly!
				ImGui::ShowDemoWindow(&g_Options.show_demo_window);
			}



			ImGui::PushFont(bestfont);

			static int page = 0;
			static int pina = 0;
			ImGui::SelectTabs(&page, sidebar_tabs, ARRAYSIZE(sidebar_tabs));

			ImGui::PopFont();

			ImGui::Spacing();


			
			ImGui::GetStyle().ItemSpacing = ImVec2(0, 8);
			ImGui::GetStyle().FrameBorderSize = 1.f;
			ImGui::GetStyle().WindowPadding = ImVec2(8, 2);
			ImGui::GetStyle().WindowBorderSize = 2.0f;
			ImGui::GetStyle().WindowTitleAlign = ImVec2(0.5f, 0.5f);
			ImGui::GetStyle().ScrollbarSize = 8.0f;
			// // 


			static int subtab1;



			switch (page)
			{
			case 0:
				RenderLegitBotTab();
				//ImGui::Checkbox("Spectator List", &g_Options.spectator_list);
				break;
			case 1:
				RenderRageBotTab();
				break;
			case 2:
				RenderVisualsTab();
				break;
			case 3:
				RenderMiscTab();
				break;
			case 4:
				
				RenderSkinsTab();
				break;
			}



			//-------------
			/*
			if (page == 2)
			{
			ImGui::Button("", ImVec2(185, 4));
			ImGui::GetStyle().Colors[ImGuiCol_Button] = ImVec4(1.00f, 0.00f, 0.27f, 1.00f);
			ImGui::SameLine();
			}
			else
			{
			ImGui::Button("", ImVec2(185, 4));
			ImGui::GetStyle().Colors[ImGuiCol_Button] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
			ImGui::SameLine();
			}
			*/
			ImGui::End();
		}
	}

	if (g_Options.spectator_list)
	{
		int cnt = 0;
		for (int i = 1; i <= g_EntityList->GetHighestEntityIndex(); i++)
		{
	
			C_BasePlayer *player = C_BasePlayer::GetPlayerByIndex(i);

			if (!player || player == nullptr)
				continue;

			player_info_t player_info;
			if (player != g_LocalPlayer)
			{
				if (g_EngineClient->GetPlayerInfo(i, &player_info) && !player->IsAlive() && !player->IsDormant())
				{
					auto observer_target = player->m_hObserverTarget();
					if (!observer_target)
						continue;

					auto target = observer_target.Get();
					if (!target)
						continue;

					SpecListStyle();

					player_info_t player_info2;
					if (g_EngineClient->GetPlayerInfo(target->EntIndex(), &player_info2))
					{
						char player_name[255] = { 0 };
						sprintf_s(player_name, "%s -> %s", player_info.szName, player_info2.szName);
						

						ImGui::SetNextWindowSize(ImVec2(300, 160));
						ImGui::Begin("Spectator List", &g_Options.spectator_list, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
						{
							ImVec2 siz = ImGui::CalcTextSize(player_name);
							

							if (target->EntIndex() == g_LocalPlayer->EntIndex())
							{
								ImGui::TextColored(ImVec4(1.00f, 0.00f, 0.23f, 1.f), player_name);
							}
							else
							{
								ImGui::Text(player_name);
							}

						}ImGui::End();

					}
						int w, h;
						++cnt;
					}
				}
			else
			{

				ImGui::SetNextWindowSize(ImVec2(300, 160));
				ImGui::Begin("Spectator List", &g_Options.spectator_list, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
				{


				}ImGui::End();

			}

			}
		}

	if (g_Options.Radar_window)
	{	
		ImGuiStyle& style = ImGui::GetStyle();
		ImVec2 oldPadding = style.WindowPadding;
		float oldAlpha = style.Colors[ImGuiCol_WindowBg].w;
		style.WindowPadding = ImVec2(0, 0);
		style.Colors[ImGuiCol_WindowBg].w = g_Options.radar_alpha;
		style.Colors[ImGuiCol_TitleBg] = ImColor(21, 21, 21, 255);
		style.Colors[ImGuiCol_TitleBgCollapsed] = ImColor(21, 21, 21, 255);
		style.Colors[ImGuiCol_TitleBgActive] = ImColor(21, 21, 21, 255);
		style.Colors[ImGuiCol_CloseButton] = ImColor(0, 0, 0, 0);
		style.Colors[ImGuiCol_CloseButtonHovered] = ImColor(0, 0, 0, 0);
		style.Colors[ImGuiCol_CloseButtonActive] = ImColor(0, 0, 0, 0);
		style.Alpha = 1.f;
		ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiSetCond_FirstUseEver);
		ImGui::SetNextWindowSizeConstraints(ImVec2(0, 0), ImVec2(FLT_MAX, FLT_MAX), SquareConstraint);
		if (ImGui::Begin(("Radar"), &g_Options.Radar_window, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar))
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
	ImGui::Render();
	}




void Menu::Show()
{
	_visible = true;
	cl_mouseenable->SetValue(false);
}

void Menu::Hide()
{
	_visible = false;
	cl_mouseenable->SetValue(true);
}

void Menu::Toggle()
{
	_visible = !_visible;
	cl_mouseenable->SetValue(!_visible);
}



void Menu::CreateStyle()
{

	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->AddFontDefault();

	// merge in icons from Font Awesome
	static const ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
	ImFontConfig icons_config; icons_config.MergeMode = true; icons_config.PixelSnapH = true;
	ImFontConfig iconsCfg; iconsCfg.MergeMode = true; iconsCfg.PixelSnapH = true; iconsCfg.FontDataOwnedByAtlas = false;
	bestfont = io.Fonts->AddFontFromMemoryTTF(fontAwesomeRawData, sizeof fontAwesomeRawData, 15.5f, &iconsCfg, icons_ranges);


	ImFontConfig config;

	config.PixelSnapH = true;
	config.OversampleH = 1;
	config.OversampleV = 1;
	config.GlyphExtraSpacing.x = 1.0f;
	static const ImWchar icon_ranges[] = { 0xE000, 0xF000, 0 };
	bestee = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\csgo_icons.ttf", 24.0f, &config, icon_ranges);


	ImVec4* colors = ImGui::GetStyle().Colors;
	colors[ImGuiCol_Text] = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);
	colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
	colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
	colors[ImGuiCol_ChildBg] = ImVec4(0.07f, 0.07f, 0.07f, 0.40f);
	colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
	colors[ImGuiCol_Border] = ImVec4(0.00f, 0.00f, 0.00f, 0.71f);
	colors[ImGuiCol_BorderShadow] = ImVec4(0.06f, 0.06f, 0.06f, 0.01f);
	colors[ImGuiCol_FrameBg] = ImVec4(0.10f, 0.10f, 0.10f, 0.71f);
	colors[ImGuiCol_FrameBgHovered] = ImVec4(0.19f, 0.19f, 0.19f, 0.40f);
	colors[ImGuiCol_FrameBgActive] = ImVec4(0.20f, 0.20f, 0.20f, 0.67f);
	colors[ImGuiCol_TitleBg] = ImVec4(0.07f, 0.07f, 0.07f, 0.48f);
	colors[ImGuiCol_TitleBgActive] = ImVec4(0.07f, 0.07f, 0.07f, 0.48f);
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.07f, 0.07f, 0.07f, 0.48f);
	colors[ImGuiCol_MenuBarBg] = ImVec4(0.10f, 0.10f, 0.10f, 0.66f);
	colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.00f);
	colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.17f, 0.17f, 0.17f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.37f, 0.00f, 0.12f, 1.00f);
	colors[ImGuiCol_CheckMark] = ImVec4(0.44f, 0.00f, 0.13f, 1.00f);
	colors[ImGuiCol_SliderGrab] = ImVec4(0.29f, 0.29f, 0.29f, 1.00f);
	colors[ImGuiCol_SliderGrabActive] = ImVec4(1.00f, 0.00f, 0.27f, 1.00f);
	colors[ImGuiCol_Button] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
	colors[ImGuiCol_ButtonHovered] = ImVec4(1.00f, 0.00f, 0.23f, 1.00f);
	colors[ImGuiCol_ButtonActive] = ImVec4(1.00f, 0.00f, 0.23f, 1.00f);
	colors[ImGuiCol_Header] = ImVec4(1.00f, 0.00f, 0.30f, 1.00f);
	colors[ImGuiCol_HeaderHovered] = ImVec4(1.00f, 0.00f, 0.30f, 0.80f);
	colors[ImGuiCol_HeaderActive] = ImVec4(1.00f, 0.00f, 0.33f, 1.00f);
	colors[ImGuiCol_Separator] = ImVec4(0.10f, 0.10f, 0.10f, 0.90f);
	colors[ImGuiCol_SeparatorHovered] = ImVec4(1.00f, 0.00f, 0.33f, 0.78f);
	colors[ImGuiCol_SeparatorActive] = ImVec4(1.00f, 0.00f, 0.36f, 1.00f);
	colors[ImGuiCol_ResizeGrip] = ImVec4(0.26f, 0.59f, 0.98f, 0.25f);
	colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
	colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
	colors[ImGuiCol_CloseButton] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_CloseButtonHovered] = ImVec4(1.00f, 1.00f, 1.00f, 0.00f);
	colors[ImGuiCol_CloseButtonActive] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
	colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
	colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
	colors[ImGuiCol_TextSelectedBg] = ImVec4(1.00f, 0.00f, 0.30f, 0.35f);
	colors[ImGuiCol_ModalWindowDarkening] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
	colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);

	_style.WindowBorderSize = 1.5f;
	_style.FrameBorderSize = 1.5f;
	_style.WindowPadding = ImVec2(8, 8);      // Padding within a window
	_style.WindowMinSize = ImVec2(32, 32);    // Minimum window size
	_style.WindowRounding = 9.0f;             // Radius of window corners rounding. Set to 0.0f to have rectangular windows
	_style.WindowTitleAlign = ImVec2(5.f, 0.5f);// Alignment for title bar text
	_style.FramePadding = ImVec2(4, 3);      // Padding within a framed rectangle (used by most widgets)
	_style.FrameRounding = 0.0f;             // Radius of frame corners rounding. Set to 0.0f to have rectangular frames (used by most widgets).
	_style.ItemSpacing = ImVec2(0, 4);      // Horizontal and vertical spacing between widgets/lines
	_style.ItemInnerSpacing = ImVec2(4, 4);      // Horizontal and vertical spacing between within elements of a composed widget (e.g. a slider and its label)
	_style.TouchExtraPadding = ImVec2(0, 0);      // Expand reactive bounding box for touch-based system where touch position is not accurate enough. Unfortunately we don't sort widgets so priority on overlap will always be given to the first widget. So don't grow this too much!
	_style.IndentSpacing = 21.0f;            // Horizontal spacing when e.g. entering a tree node. Generally == (FontSize + FramePadding.x*2).
	_style.ColumnsMinSpacing = 6.0f;             // Minimum horizontal spacing between two columns
	_style.ScrollbarSize = 8.0f;            // Width of the vertical scrollbar, Height of the horizontal scrollbar
	_style.ScrollbarRounding = 9.0f;             // Radius of grab corners rounding for scrollbar
	_style.GrabMinSize = 10.0f;            // Minimum width/height of a grab box for slider/scrollbar
	_style.GrabRounding = 0.0f;             // Radius of grabs corners rounding. Set to 0.0f to have rectangular slider grabs.
	_style.ButtonTextAlign = ImVec2(0.5f, 0.5f);// Alignment of button text when button is larger than text.
	_style.DisplayWindowPadding = ImVec2(22, 22);    // Window positions are clamped to be visible within the display area by at least this amount. Only covers regular windows.
	_style.DisplaySafeAreaPadding = ImVec2(4, 4);      // If you cannot see the edge of your screen (e.g. on a TV) increase the safe area padding. Covers popups/tooltips as well regular windows.
	_style.AntiAliasedLines = true;             // Enable anti-aliasing on lines/borders. Disable if you are really short on CPU/GPU.
	_style.CurveTessellationTol = 1.25f;            // Tessellation tolerance. Decrease for highly tessellated curves (higher quality, more polygons), increase to reduce quality.
}



