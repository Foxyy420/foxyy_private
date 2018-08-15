#pragma once
#include "MainInclude.hpp"

class legitbot
	: public Singleton<legitbot>
{
public:
	legitbot();

	void weapon_settings(C_BaseCombatWeapon* weapon);

	void on_create_move(CUserCmd* cmd, C_BasePlayer* local, C_BaseCombatWeapon* weapon);
	
	void do_aimbot(C_BasePlayer *local, C_BaseCombatWeapon *weapon, CUserCmd *cmd);
	QAngle get_randomized_recoil(C_BasePlayer* local);
	QAngle get_randomized_angles(C_BasePlayer* local);

private:
	bool get_hitbox(C_BasePlayer *local, C_BasePlayer *entity, Vector &destination);
	int get_target(C_BasePlayer *local, C_BaseCombatWeapon *weapon, CUserCmd *cmd, Vector &destination);

private:
	int best_target;

	QAngle view_angle;
	QAngle aim_angle;
	QAngle delta_angle;
	QAngle final_angle;

	Vector hitbox_position;

	int aim_key;
	float aim_smooth;
	float aim_fov;
	float randomized_smooth;
	float recoil_min;
	float recoil_max;
	float randomized_angle;
	bool shoot;
	int hitbox;

	bool enable_trigger;
	int trigger_key;
	bool trigger_head;
	bool trigger_chest;
	bool trigger_stomach;
	bool trigger_arms;
	bool trigger_legs;
};

