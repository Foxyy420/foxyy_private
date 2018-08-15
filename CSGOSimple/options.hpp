#pragma once

#include <string>
#include "valve_sdk/Misc/Color.hpp"
#include <chrono>
#include <fstream>
#include <vector>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <set>
#include "skins.h"

#define OPTION(type, var, val) type var = val

extern bool aimstepInProgress;
extern bool sendPacket;
extern bool aaSide;



enum AntiAimPitch_t
{
	ANTIAIM_PITCH_NONE,

	ANTIAIM_PITCH_DOWN,
	ANTIAIM_PITCH_UP,
	ANTIAIM_PITCH_DANCE,
	ANTIAIM_PITCH_FAKEUP,
	ANTIAIM_PITCH_FAKEDOWN,
	ANTIAIM_PITCH_RANDOM
};

enum AntiAimYaw_t
{
	ANTIAIM_YAW_NONE,

	ANTIAIM_YAW_SPIN,
	ANTIAIM_YAW_STATIC_FORWARD,
	ANTIAIM_YAW_STATIC_RIGHT,
	ANTIAIM_YAW_STATIC_BACKWARDS,
	ANTIAIM_YAW_STATIC_LEFT,
	ANTIAIM_YAW_BACKWARDS,
	ANTIAIM_YAW_LEFT,
	ANTIAIM_YAW_RIGHT,
	ANTIAIM_YAW_SIDE,
	ANTIAIM_YAW_FAKE_LBY1,
	ANTIAIM_YAW_FAKE_LBY2,
	ANTIAIM_YAW_JITTER,
	ANTIAIM_YAW_BACKJITTER,
	ANTIAIM_YAW_FAKE_SIDE_LBY,
	ANTIAIM_YAW_RANDOM
};

enum Knives_t
{
	KNIFE_DEFAULT,
	KNIFE_BAYONET,
	KNIFE_FLIP,
	KNIFE_GUT,
	KNIFE_KARAMBIT,
	KNIFE_M9BAYONET,
	KNIFE_HUNTSMAN,
	KNIFE_FALCHION,
	KNIFE_BOWIE,
	KNIFE_BUTTERFLY,
	KNIFE_PUSHDAGGER
};

class Config
{
public:

	float DLight_color[3] = { 1.00f, 0.00f, 0.23f };

	SkinInfo_t skins[5035];

	bool Enable_AA = false;
	int Hitsound = 0;
	int HealthBar_Style = 0;

	bool best_resolver = false;

	struct
	{
		int Knife;
		int Ak47;
		int Arms;
		int CT_Model;
		int T_Model;
		int P90;
		int Awp;
		int M4A1;

	}Mdl;

	struct
	{
		bool Enable;
		int	 Key;
		bool Head;
		bool Arms;
		bool Chest;
		bool Stomach;
		bool Legs;

		//rifles
		bool Rifles_Enable;
		int Rifles_Key;
		bool Rifles_Head;
		bool Rifles_Arms;
		bool Rifles_Chest;
		bool Rifles_Stomach;
		bool Rifles_Legs;
		//pistols
		bool Pistols_Enable;
		int Pistols_Key;
		bool Pistols_Head;
		bool Pistols_Arms;
		bool Pistols_Chest;
		bool Pistols_Stomach;
		bool Pistols_Legs;
		//snipers
		bool Snipers_Enable;
		int Snipers_Key;
		bool Snipers_Head;
		bool Snipers_Arms;
		bool Snipers_Chest;
		bool Snipers_Stomach;
		bool Snipers_Legs;

	}Trigger;

	bool test = false;

	/*RAGE*/
	/////////////////////////////////////////////

	OPTION(bool, hvh_resolver, false);
	OPTION(int, resolvery, 0);
	OPTION(int, resolverx, 0);
	OPTION(float, aim_fov, 180.f);
	OPTION(bool, aim_enabled, false);
	OPTION(bool, steamid, false);
	OPTION(bool, aim_friendly_fire, false);
	OPTION(bool, aim_auto_shoot, false);
	OPTION(bool, aim_silent_mode, false);
	OPTION(int, silentaimtype, 0);
	OPTION(bool, auto_duck, false);
	OPTION(bool, auto_wall, false);
	OPTION(float, auto_wall_mindamange, 20.0f);
	OPTION(bool, hit_chance, false);
	OPTION(bool, multipointer, false);
	OPTION(bool, fovfix, false);
	OPTION(bool, attarget, false);
	OPTION(bool, vecvelocityprediction, false);
	OPTION(float, hit_chanceamount, false);
	OPTION(bool, auto_scope, false);
	OPTION(bool, enginepred, false);
	OPTION(bool, Remove_recoil, false);

	/*RAGE*/
	/////////////////////////////////////////////

	/*VISUALS*/
	/////////////////////////////////////////////
	OPTION(bool, esp_player_anglelines, false);
	OPTION(bool, esp_player_money, false);
	OPTION(int, esp_boxtype, 0);
	OPTION(int, esp_world_type, 0);
	OPTION(bool, esp_filled_box, false);
	//bullet_tracers
	OPTION(bool, esp_bullet_beams, false);
	OPTION(bool, misc_bullettracers, false);
	OPTION(int, misc_bullettracers_type, 0);
	OPTION(float, esp_bullettrace_length, 1.3f);
	OPTION(float, esp_bulletbeams_length, 1.3f);
	//OPTION(bool, esp_is_flashed, false);
	OPTION(bool, grenade_pred, false);
	OPTION(bool, Recoil_crosshair, false);
	OPTION(bool, greane_prediction, false);
	OPTION(bool, esp_c4_timer, false);
	OPTION(bool, esp_enabled, false);
	OPTION(bool, esp_health_text, false);
	OPTION(bool, esp_enemies_only, false);
	OPTION(bool, esp_player_boxes, false);
	OPTION(bool, esp_player_names, false);
	OPTION(bool, esp_player_health, false);
	OPTION(bool, esp_player_armour, false);
	OPTION(bool, esp_player_weapons, false);
	OPTION(bool, esp_player_snaplines, false);
	OPTION(bool, esp_crosshair, false);
	OPTION(bool, esp_defuse_kit, false);
	OPTION(bool, esp_planted_c4, false);
	OPTION(bool, esp_remove_flash, false);
	OPTION(bool, esp_3d_cube, false);
	OPTION(bool, esp_skeleton, false);
	OPTION(bool, watermark, true);
	OPTION(int, esp_dropped_weapons, 0);
	OPTION(int, esp_dropped_c4, 0);
	OPTION(int, esp_dropped_Kits, 0);
	OPTION(bool, lagcomp_hitboxes, false);

	/*VISUALS*/
	/////////////////////////////////////////////


	/*LEGITBOT*/
	/////////////////////////////////////////////
	OPTION(bool, enable_legitbot ,false);
	//pistols
	OPTION(int, hitbox_pistols, 0);
	OPTION(bool, aim_LegitBotPistols, false);
	OPTION(float, legit_fov_pistols, 1.f);
	OPTION(float, legit_smooth_pistols, 1.f);
	OPTION(float, legit_rcsmin_pistols, 0.f);
	OPTION(float, legit_rcsmax_pistols, 0.f);
	// rifles
	OPTION(int, hitbox_rifles, 0);
	OPTION(bool, aim_LegitBotRifles, false);
	OPTION(float, legit_smooth_rifles, 1.f);
	OPTION(float, legit_fov_rifles, 1.f);
	OPTION(float, legit_rcsmin_rifles, 0.f);
	OPTION(float, legit_rcsmax_rifles, 0.f);
	//snipers
	OPTION(int, hitbox_snipers, 0);
	OPTION(bool, aim_LegitBotSnipers, false);
	OPTION(float, legit_smooth_Snipers, 1.f);
	OPTION(float, legit_fov_Snipers, 1.f);
	OPTION(float, legit_rcsmin_Snipers, 0.f);
	OPTION(float, legit_rcsmax_Snipers, 0.f);
	OPTION(float, hitchance_amount, 0.f);
	/*LEGITBOT*/
	/////////////////////////////////////////////

	/*HVH*/
	/////////////////////////////////////////////
	OPTION(float, antiaim_edge_dist, 0.0f);
	OPTION(bool, antiaim_antiresolver, false);
	OPTION(int, antiaim_pitch, ANTIAIM_PITCH_NONE);
	OPTION(int, antiaim_yaw, ANTIAIM_YAW_NONE);
	OPTION(int, antiaim_yaw_fake, ANTIAIM_YAW_NONE);
	OPTION(float, antiaim_spin_speed, 0.0f);
	OPTION(bool, antiaim_knife, false);
	OPTION(int, tankAntiaimKey, 0);
	OPTION(bool, antiuntrusted, true);
	OPTION(int, fakelag_amount, 0);
	/*HVH*/
	/////////////////////////////////////////////

	/*GLOW*/
	/////////////////////////////////////////////
	OPTION(int, players_glow_style, 1);
	OPTION(bool, glow_enabled, false);
	OPTION(bool, glow_enemies_only, false);
	OPTION(bool, glow_players, false);
	OPTION(bool, glow_chickens, false);
	OPTION(bool, glow_c4_carrier, false);
	OPTION(bool, glow_planted_c4, false);
	OPTION(bool, glow_defuse_kits, false);
	OPTION(bool, glow_weapons, false);
	OPTION(bool, enable_dlight, false);
	/*GLOW*/
	/////////////////////////////////////////////



	/*CHAMS*/
	/////////////////////////////////////////////
	OPTION(bool, chams_player_enabled, false);
	OPTION(bool, chams_player_enemies_only, false);
	OPTION(bool, chams_player_wireframe, false);
	OPTION(bool, chams_player_flat, false);
	OPTION(bool, chams_player_ignorez, false);
	OPTION(bool, chams_player_glass, false);
	OPTION(bool, chams_weapon_materials_enabled, false);
	OPTION(int, chams_arms_materials, false);
	OPTION(int, chams_weapon_materials, false);
	OPTION(bool, chams_rainbow_wep, false);
	OPTION(bool, visual_others_steamid, false);
	OPTION(bool, visuals_grenade_pred, false);
	OPTION(bool, chams_weapon_enabled, false);
	OPTION(bool, chams_weapon_enabled_rainbow, false);
	OPTION(bool, chams_weapon_wireframe, false);
	OPTION(bool, chams_weapon_flat, false);
	OPTION(bool, chams_weapon_ignorez, false);
	OPTION(bool, chams_weapon_glass, false);
	OPTION(bool, chams_arms_materials_enabled, false);
	OPTION(bool, chams_arms_enabled, false);
	OPTION(bool, chams_arms_wireframe, false);
	OPTION(bool, chams_arms_flat, false);
	OPTION(bool, chams_arms_ignorez, false);
	OPTION(bool, chams_arms_glass, false);
	/*CHAMS*/
	/////////////////////////////////////////////



	/*COLORS*/
	/////////////////////////////////////////////
	OPTION(Color, color_esp_ally_visible, Color(0, 180, 255));
	OPTION(Color, color_esp_enemy_visible, Color(176, 0, 255));
	OPTION(Color, color_esp_ally_occluded, Color(0, 200, 255));
	OPTION(Color, color_esp_enemy_occluded, Color(254, 0, 255));
	OPTION(Color, color_esp_crosshair, Color(253, 254, 255));
	OPTION(Color, color_esp_weapons, Color(118, 1, 255));
	OPTION(Color, color_esp_defuse, Color(0, 138, 255));
	OPTION(Color, color_esp_droppedd_weapon, Color(0, 138, 255));
	OPTION(Color, color_esp_c4, Color(215, 220, 250));
	OPTION(Color, color_glow_ally, Color(0, 125, 255));
	OPTION(Color, color_glow_enemy, Color(220, 0, 255));
	OPTION(Color, color_glow_chickens, Color(0, 124, 255));
	OPTION(Color, color_glow_c4_carrier, Color(251, 251, 255));
	OPTION(Color, color_glow_planted_c4, Color(127, 0, 255));
	OPTION(Color, color_glow_defuse, Color(253, 210, 255));
	OPTION(Color, color_glow_weapons, Color(220, 118, 255));
	OPTION(Color, color_chams_player_ally_visible, Color(0, 128, 255));
	OPTION(Color, color_chams_player_ally_occluded, Color(0, 255, 255));
	OPTION(Color, color_chams_player_enemy_visible, Color(255, 0, 255));
	OPTION(Color, color_chams_player_enemy_occluded, Color(255, 128, 255));
	OPTION(Color, color_chams_arms_visible, Color(0, 128, 255));
	OPTION(Color, color_chams_weapon_visible, Color(0, 128, 255));
	OPTION(Color, color_chams_weapon_occluded, Color(0, 128, 255));
	OPTION(Color, color_chams_arms_occluded, Color(0, 128, 255));
	OPTION(Color, color_esp_box_filled, Color(0, 0, 0, 61));
	OPTION(Color, color_dlight, Color(176, 0, 255));
	OPTION(Color, color_esp_name, Color(255, 255, 255));
	OPTION(Color, color_esp_pl_weapons, Color(176, 0, 255));
	OPTION(Color, color_esp_skeleton, Color(255, 255, 255));
	OPTION(Color, color_esp_bullet_beams, Color(118, 1, 255));

	float glow_player_color_t[4] = { 1.f, 0.f, 0.3f, 1.f };
	float glow_player_color_ct[4] = { 0.f, 0.2f, 1.f, 1.f };
	float glow_player_color_t_visible[4] = { 1.f, 1.f, 0.0f, 1.f };
	float glow_player_color_ct_visible[4] = { 0.f, 0.7f, 1.f, 0.85f };

	/*COLORS*/
	/////////////////////////////////////////////


	/*WORLD*/
	/////////////////////////////////////////////
	int world_type = 0;
	bool gray_world = false;
	bool mlg_mode = false;
	bool minecraft_mode = false;
	float sky_color[3] = { 0.15f, 0.15f, 0.15f };
	float nightmode_intensity = 0.30f;
	
	/*WORLD*/
	/////////////////////////////////////////////

	/*MENU*/
	/////////////////////////////////////////////
	float specl_list_alpha = 1.f;
	float radar_alpha = 1.f;

	/*MENU*/
	/////////////////////////////////////////////

	/*REMOVALS*/
	/////////////////////////////////////////////
	OPTION(bool, esp_no_flash, false);
	OPTION(bool, esp_no_smoke, false);
	OPTION(bool, esp_no_visual_recoil, false);

	/*REMOVALS*/
	/////////////////////////////////////////////



	int knifemodel;


	/*RAGE*/

	

	float MainColor[4] = { 1.00f, 0.00f, 0.23f, 1.f };
	float BulletTraceColor[3] = { 0.f, 0.f, 0.f };
	int barrelL = 0;
	bool barrel = false;

	OPTION(bool, thirdperson_enabled, false);
	OPTION(bool, chatspam, false);
	OPTION(bool, lefthand, false);


	bool enable_aim = false;
	bool spectator_list = false;

	

	OPTION(bool, grenade_esp, false);
	
	OPTION(float, visuals_lagcomp_duration, 1.f);





	


	OPTION(bool, Radar_window, false);
	OPTION(bool, legitAA, false);
	OPTION(bool, legit_enabled, false);
	OPTION(int, aimkey, 0);
	OPTION(bool, legit_rcs, false);
	

	////////////////////////////////////////


	OPTION(int, hitboxes, 1);

	

	


	//
	// MISC
	//

	OPTION(bool, misc_bhop, false);
	OPTION(bool, misc_autostrafe, false);

	OPTION(int, misc_fakewalk_key, false);
	OPTION(bool, misc_fakewalk, false);
	OPTION(bool, misc_hitmarker, false);
	OPTION(bool, g_bRCSEnabled, false);
	OPTION(bool, Epost_process, false);
	OPTION(bool, animated_clantag, false);
	OPTION(bool, misc_no_name, false);
	OPTION(bool, misc_aimware_namespam, false);
	OPTION(bool, misc_no_hands, false);
	OPTION(int, viewmodel_fov, 68);
	OPTION(float, mat_ambient_light_r, 0.0f);
	OPTION(float, mat_ambient_light_g, 0.0f);
	OPTION(float, mat_ambient_light_b, 0.0f);
	OPTION(float, transparency, 0.700);
	OPTION(float, rainbow_speed, 0.0005);



	OPTION(bool, auto_accept, true);
	OPTION(bool, open_playerlist, false);

	//------------------------------------------------------

	OPTION(bool, esp_flags_hasdefuser, false);
	OPTION(bool, esp_flags_is_flashed, false);
	OPTION(bool, esp_flags_scoped, false);
	OPTION(bool, esp_flags_bomb_carrier, false);
	OPTION(bool, esp_flags_hostage_carrier, false);
	OPTION(bool, esp_flags_rescuing, false);


	//------------------------------------------------------




	float misc_bullettracers_color[4] = { 1.f, 0.f, 0.3f, 1.f };

	OPTION(int, skybox, false);
	OPTION(bool, nightmode, false);
	
	OPTION(bool, show_demo_window, false);
	OPTION(bool, misc_backtrack, false);

	

};

extern Config g_Options;
extern bool   g_Unload;
