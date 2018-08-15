#include "valve_sdk/sdk.hpp"

#include "Config.h"
#include "json.hpp"

#include "options.hpp"

#include <fstream>
#include <experimental/filesystem> // hack
#include "hooks.hpp"
#include <iomanip>

nlohmann::json config;

void ConfigSys::CreateConfigFolder(std::string path)
{
	if (!std::experimental::filesystem::create_directory(path)) return;
}

bool ConfigSys::FileExists(std::string file)
{
	return std::experimental::filesystem::exists(file);
}

void ConfigSys::SaveConfig(const std::string path)
{
	std::ofstream output_file(path);

	if (!output_file.good())
		return;

	//ESP 1-2  (with colors)
	Save(g_Options.esp_enabled, "enable_esp");
	Save(g_Options.esp_boxtype, "esp_boxtype");  SaveColor(g_Options.color_esp_enemy_visible, "color_esp_box_enemy_visible");	SaveColor(g_Options.color_esp_enemy_occluded, "color_esp_box_enemy_occluded");
	Save(g_Options.HealthBar_Style, "esp_healthbar_type");
	Save(g_Options.esp_dropped_weapons, "esp_wepbox_type");	SaveColor(g_Options.color_esp_weapons, "color_esp_weapons");
	Save(g_Options.esp_dropped_c4, "esp_c4box_type"); SaveColor(g_Options.color_esp_c4, "color_esp_c4");
	Save(g_Options.esp_enemies_only, "esp_enemies_only");
	Save(g_Options.esp_player_names, "esp_player_names");
	Save(g_Options.esp_player_anglelines, "esp_player_anglelines");
	Save(g_Options.esp_health_text, "esp_player_health_text");
	Save(g_Options.esp_player_armour, "esp_player_armour");
	Save(g_Options.lagcomp_hitboxes, "esp_lagcomp_hitboxes");
	Save(g_Options.visuals_lagcomp_duration, "esp_lagcomp_duration");
	Save(g_Options.esp_filled_box, "esp_fill_box");	SaveColor(g_Options.color_esp_box_filled, "color_esp_box_fill");
	Save(g_Options.esp_skeleton, "esp_skeleton");	
	Save(g_Options.esp_player_weapons, "esp_player_weapon"); SaveColor(g_Options.color_esp_pl_weapons, "color_esp_pl_weapons");
	Save(g_Options.enable_dlight, "esp_dlight"); SaveArray(g_Options.DLight_color, "color_esp_dlight");
	Save(g_Options.greane_prediction, "esp_grenade_prediction");
	Save(g_Options.misc_bullettracers, "esp_bullet_trace");	SaveArray(g_Options.misc_bullettracers_color, "color_esp_bullettracer");
	Save(g_Options.esp_bullet_beams, "esp_bulletbeams"); SaveColor(g_Options.color_esp_bullet_beams, "color_esp_bullettracer");
	//Legitbot - rifles
	Save(g_Options.misc_backtrack, "backtrack");
	Save(g_Options.aim_LegitBotRifles, "legit_rifles");
	Save(g_Options.aimkey, "aim_key");
	Save(g_Options.hitbox_rifles, "legit_hitbox_rifles");
	Save(g_Options.legit_smooth_rifles, "legit_smooth_rifles");
	Save(g_Options.legit_fov_rifles, "legit_fov_rifles");
	Save(g_Options.legit_rcsmin_rifles, "legit_rcsmin_rifles");
	Save(g_Options.legit_rcsmax_rifles, "legit_rcsmax_rifles");
	//Legitbot - pistol
	Save(g_Options.aim_LegitBotPistols, "legit_pistols");
	Save(g_Options.hitbox_pistols, "legit_hitbox_pistols");
	Save(g_Options.legit_smooth_pistols, "legit_smooth_pistols");
	Save(g_Options.legit_fov_pistols, "legit_fov_pistols");
	Save(g_Options.legit_rcsmin_pistols, "legit_rcsmin_pistols");
	Save(g_Options.legit_rcsmax_pistols, "legit_rcsmax_pistols");
	//Legitbot - snipers
	Save(g_Options.aim_LegitBotSnipers, "legit_snipers");
	Save(g_Options.hitbox_snipers, "legit_hitbox_snipers");
	Save(g_Options.legit_smooth_Snipers, "legit_smooth_snipers");
	Save(g_Options.legit_fov_Snipers, "legit_fov_snipers");
	Save(g_Options.legit_rcsmin_Snipers, "legit_rcsmin_snipers");
	Save(g_Options.legit_rcsmax_Snipers, "legit_rcsmax_snipers");
	//world
	Save(g_Options.gray_world, "gray_world");
	Save(g_Options.minecraft_mode, "minecraft_mode");
	Save(g_Options.mlg_mode, "mlg_mode");
	Save(g_Options.thirdperson_enabled, "thirdperson");
	Save(g_Options.skybox, "skybox");
	//remove
	Save(g_Options.esp_no_flash, "esp_no_flash");
	Save(g_Options.esp_no_smoke, "esp_no_smoke");
	Save(g_Options.esp_no_visual_recoil, "esp_no_visual_recoil");
	//windows
	Save(g_Options.spectator_list, "spectator_list");
	Save(g_Options.Radar_window, "radar_window");
	Save(g_Options.radar_alpha, "radar_alpha");


	output_file << std::setw(4) << config << std::endl;
	output_file.close();
}

void ConfigSys::LoadConfig(const std::string path)
{
	std::ifstream input_file(path);

	if (!input_file.good())
		return;

	try
	{
		config << input_file;
	}
	catch (...)
	{
		input_file.close();
		return;
	}

	//ESP 1-2  (with colors)
	Load(g_Options.esp_enabled, "enable_esp");
	Load(g_Options.esp_boxtype, "esp_boxtype");  LoadColor(&g_Options.color_esp_enemy_visible, "color_esp_box_enemy_visible");	LoadColor(&g_Options.color_esp_enemy_occluded, "color_esp_box_enemy_occluded");
	Load(g_Options.HealthBar_Style, "esp_healthbar_type");
	Load(g_Options.esp_dropped_weapons, "esp_wepbox_type");	LoadColor(&g_Options.color_esp_weapons, "color_esp_weapons");
	Load(g_Options.esp_dropped_c4, "esp_c4box_type"); LoadColor(&g_Options.color_esp_c4, "color_esp_c4");
	Load(g_Options.esp_enemies_only, "esp_enemies_only");
	Load(g_Options.esp_player_names, "esp_player_names");
	Load(g_Options.esp_player_anglelines, "esp_player_anglelines");
	Load(g_Options.esp_health_text, "esp_player_health_text");
	Load(g_Options.esp_player_armour, "esp_player_armour");
	Load(g_Options.lagcomp_hitboxes, "esp_lagcomp_hitboxes");
	Load(g_Options.visuals_lagcomp_duration, "esp_lagcomp_duration");
	Load(g_Options.esp_filled_box, "esp_fill_box");	LoadColor(&g_Options.color_esp_box_filled, "color_esp_box_fill");
	Load(g_Options.esp_skeleton, "esp_skeleton");
	Load(g_Options.esp_player_weapons, "esp_player_weapon"); LoadColor(&g_Options.color_esp_pl_weapons, "color_esp_pl_weapons");
	Load(g_Options.enable_dlight, "esp_dlight"); LoadArray(g_Options.DLight_color, "color_esp_dlight");
	Load(g_Options.greane_prediction, "esp_grenade_prediction");
	Load(g_Options.misc_bullettracers, "esp_bullet_trace");	LoadArray(g_Options.misc_bullettracers_color, "color_esp_bullettracer");
	Load(g_Options.esp_bullet_beams, "esp_bulletbeams"); LoadColor(&g_Options.color_esp_bullet_beams, "color_esp_bullettracer");
	//Legitbot - rifles
	Load(g_Options.misc_backtrack, "backtrack");
	Load(g_Options.aim_LegitBotRifles, "legit_rifles");
	Load(g_Options.aimkey, "aim_key");
	Load(g_Options.hitbox_rifles, "legit_hitbox_rifles");
	Load(g_Options.legit_smooth_rifles, "legit_smooth_rifles");
	Load(g_Options.legit_fov_rifles, "legit_fov_rifles");
	Load(g_Options.legit_rcsmin_rifles, "legit_rcsmin_rifles");
	Load(g_Options.legit_rcsmax_rifles, "legit_rcsmax_rifles");
	//Legitbot - pistol
	Load(g_Options.aim_LegitBotPistols, "legit_pistols");
	Load(g_Options.hitbox_pistols, "legit_hitbox_pistols");
	Load(g_Options.legit_smooth_pistols, "legit_smooth_pistols");
	Load(g_Options.legit_fov_pistols, "legit_fov_pistols");
	Load(g_Options.legit_rcsmin_pistols, "legit_rcsmin_pistols");
	Load(g_Options.legit_rcsmax_pistols, "legit_rcsmax_pistols");
	//Legitbot - snipers
	Load(g_Options.aim_LegitBotSnipers, "legit_snipers");
	Load(g_Options.hitbox_snipers, "legit_hitbox_snipers");
	Load(g_Options.legit_smooth_Snipers, "legit_smooth_snipers");
	Load(g_Options.legit_fov_Snipers, "legit_fov_snipers");
	Load(g_Options.legit_rcsmin_Snipers, "legit_rcsmin_snipers");
	Load(g_Options.legit_rcsmax_Snipers, "legit_rcsmax_snipers");
	//world
	Load(g_Options.gray_world, "gray_world");
	Load(g_Options.minecraft_mode, "minecraft_mode");
	Load(g_Options.mlg_mode, "mlg_mode");
	Load(g_Options.thirdperson_enabled, "thirdperson");
	Load(g_Options.skybox, "skybox");
	//remove
	Load(g_Options.esp_no_flash, "esp_no_flash");
	Load(g_Options.esp_no_smoke, "esp_no_smoke");
	Load(g_Options.esp_no_visual_recoil, "esp_no_visual_recoil");
	//windows
	Load(g_Options.spectator_list, "spectator_list");
	Load(g_Options.Radar_window, "radar_window");
	Load(g_Options.radar_alpha, "radar_alpha");

	input_file.close();
}

std::vector<std::string> ConfigSys::GetAllConfigs()
{
	namespace fs = std::experimental::filesystem;

	std::string fPath = std::string(Global::my_documents_folder) + "\\Foxyy\\";

	std::vector<ConfigFile> config_files = GetAllConfigsInFolder(fPath, ".config");
	std::vector<std::string> config_file_names;

	for (auto config = config_files.begin(); config != config_files.end(); config++)
		config_file_names.emplace_back(config->GetName());

	std::sort(config_file_names.begin(), config_file_names.end());

	return config_file_names;
}

std::vector<ConfigFile> ConfigSys::GetAllConfigsInFolder(const std::string path, const std::string ext)
{
	namespace fs = std::experimental::filesystem;

	std::vector<ConfigFile> config_files;

	if (fs::exists(path) && fs::is_directory(path))
	{
		for (auto it = fs::recursive_directory_iterator(path); it != fs::recursive_directory_iterator(); it++)
		{
			if (fs::is_regular_file(*it) && it->path().extension() == ext)
			{
				std::string fPath = path + it->path().filename().string();

				std::string tmp_f_name = it->path().filename().string();
				size_t pos = tmp_f_name.find(".");
				std::string fName = (std::string::npos == pos) ? tmp_f_name : tmp_f_name.substr(0, pos);

				ConfigFile new_config(fName, fPath);
				config_files.emplace_back(new_config);
			}
		}
	}
	return config_files;
}

template<typename T>
void ConfigSys::Load(T &value, std::string str)
{
	if (config[str].empty())
		return;

	value = config[str].get<T>();
}

void ConfigSys::LoadArray(float_t value[4], std::string str)
{
	if (config[str]["0"].empty() || config[str]["1"].empty() || config[str]["2"].empty() || config[str]["3"].empty())
		return;

	value[0] = config[str]["0"].get<float_t>();
	value[1] = config[str]["1"].get<float_t>();
	value[2] = config[str]["2"].get<float_t>();
	value[3] = config[str]["3"].get<float_t>();
}

void ConfigSys::LoadArray(bool value[14], std::string str)
{
	if (config[str]["0"].empty() || config[str]["1"].empty() || config[str]["2"].empty() || config[str]["3"].empty()
		|| config[str]["4"].empty() || config[str]["5"].empty() || config[str]["6"].empty() || config[str]["7"].empty()
		|| config[str]["8"].empty() || config[str]["9"].empty() || config[str]["10"].empty() || config[str]["11"].empty()
		|| config[str]["12"].empty() || config[str]["13"].empty())
		return;

	value[0] = config[str]["0"].get<bool>();
	value[1] = config[str]["1"].get<bool>();
	value[2] = config[str]["2"].get<bool>();
	value[3] = config[str]["3"].get<bool>();
	value[4] = config[str]["4"].get<bool>();
	value[5] = config[str]["5"].get<bool>();
	value[6] = config[str]["6"].get<bool>();
	value[7] = config[str]["7"].get<bool>();
	value[8] = config[str]["8"].get<bool>();
	value[9] = config[str]["9"].get<bool>();
	value[10] = config[str]["10"].get<bool>();
	value[11] = config[str]["11"].get<bool>();
	value[12] = config[str]["12"].get<bool>();
	value[13] = config[str]["13"].get<bool>();
}

template<typename T>
void ConfigSys::Save(T &value, std::string str)
{
	config[str] = value;
}

void ConfigSys::SaveArray(float_t value[4], std::string str)
{
	config[str]["0"] = value[0];
	config[str]["1"] = value[1];
	config[str]["2"] = value[2];
	config[str]["3"] = value[3];
}

void ConfigSys::SaveArray(bool value[14], std::string str)
{
	config[str]["0"] = value[0];
	config[str]["1"] = value[1];
	config[str]["2"] = value[2];
	config[str]["3"] = value[3];
	config[str]["4"] = value[4];
	config[str]["5"] = value[5];
	config[str]["6"] = value[6];
	config[str]["7"] = value[7];
	config[str]["8"] = value[8];
	config[str]["9"] = value[9];
	config[str]["10"] = value[10];
	config[str]["11"] = value[11];
	config[str]["12"] = value[12];
	config[str]["13"] = value[13];
}


void ConfigSys::SaveColor(Color col, std::string str)
{
	config[str]["0"] = col[0];
	config[str]["1"] = col[1];
	config[str]["2"] = col[2];
	config[str]["3"] = col[3];
}


void ConfigSys::LoadColor(Color* col, std::string str)
{
	if (config[str]["0"].empty() || config[str]["1"].empty(), config[str]["2"].empty(), config[str]["3"].empty())
		return;

	int red = config[str]["0"].get<int>();
	int green = config[str]["1"].get<int>();
	int blue = config[str]["2"].get<int>();
	int alpha = config[str]["3"].get<int>();

	col->SetColor(red, green, blue, alpha);
}
 