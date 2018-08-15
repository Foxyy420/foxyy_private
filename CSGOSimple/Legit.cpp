#include "Legit.hpp"
#define M_RADPI 57.295779513082f
#include <random>


//--------------------------------------------------------------------------------
float get_distance(const Vector &start, const Vector &end)
{
	float distance = sqrt((start - end).Length());

	if (distance < 1.0f)
		distance = 1.0f;

	return distance;
}
float get_fov(const QAngle &viewAngles, const QAngle &aimAngles)
{
	Vector ang, aim;
	Math::AngleVectors(viewAngles, aim);
	Math::AngleVectors(aimAngles, ang);

	return RAD2DEG(acos(aim.Dot(ang) / aim.LengthSqr()));
}
void compute_angle(const Vector &source, const Vector &destination, QAngle& angles)
{
	Vector delta = source - destination;
	angles.pitch = static_cast< float >(asin(delta.z / delta.Length()) * M_RADPI);
	angles.yaw = static_cast< float >(atan(delta.y / delta.x) * M_RADPI);
	angles.roll = 0.0f;

	if (delta.x >= 0.0f)
		angles.yaw += 180.0f;
}
QAngle compute_angle(const Vector &source, const Vector &destination)
{
	QAngle angles;

	Vector delta = source - destination;
	angles.pitch = static_cast< float >(asin(delta.z / delta.Length()) * M_RADPI);
	angles.yaw = static_cast< float >(atan(delta.y / delta.x) * M_RADPI);
	angles.roll = 0.0f;

	if (delta.x >= 0.0f)
		angles.yaw += 180.0f;

	return angles;
}

bool sanitize_angles(QAngle &angles)
{
	QAngle temp = angles;
	Math::NormalizeAngles(temp);
	Math::ClampAngles(temp);

	if (!isfinite(temp.pitch) ||
		!isfinite(temp.yaw) ||
		!isfinite(temp.roll))
		return false;

	angles = temp;

	return true;
}

#define TICK_INTERVAL			(g_GlobalVars->interval_per_tick)
#define TIME_TO_TICKS( dt )		( (int)( 0.5f + (float)(dt) / TICK_INTERVAL ) )
legitbot::legitbot()
{
	best_target = -1;

	view_angle = QAngle(0.0f, 0.0f, 0.0f);
	aim_angle = QAngle(0.0f, 0.0f, 0.0f);
	delta_angle = QAngle(0.0f, 0.0f, 0.0f);
	final_angle = QAngle(0.0f, 0.0f, 0.0f);

	hitbox_position = Vector(0.0f, 0.0f, 0.0f);

	hitbox = 0;
	aim_key = 0;
	aim_smooth = 1;
	aim_fov = 0;
	randomized_smooth = 0;
	recoil_min = 0;
	recoil_max = 0;
	randomized_angle = 0;
	
	//trigger
	enable_trigger = false;
	trigger_key = 0;
	trigger_head = false;
	trigger_chest = false;
	trigger_stomach = false;
	trigger_arms = false;
	trigger_legs = false;
	

}

float random_number_range(float min, float max)
{
	std::random_device device;
	std::mt19937 engine(device());
	std::uniform_real_distribution<> distribution(min, max);
	return static_cast< float >(distribution(engine));
}

void legitbot::on_create_move(CUserCmd *cmd, C_BasePlayer* local, C_BaseCombatWeapon* weapon)
{
	do_aimbot(local, weapon, cmd);
}

void legitbot::do_aimbot(C_BasePlayer *local, C_BaseCombatWeapon *weapon, CUserCmd *cmd)
{
	if (!local)
		return;

	if (!weapon)
		return;

	if (!g_Options.aim_LegitBotRifles && weapon->IsRifle() | !g_Options.aim_LegitBotPistols && weapon->IsPistol() | !g_Options.aim_LegitBotSnipers && weapon->IsSniper())
		return;

	if (!local->IsAlive())
		return;

	if (!weapon->HasBullets())
		return;

	weapon_settings(weapon);

	if (!GetAsyncKeyState(g_Options.aimkey))
		return;

	best_target = get_target(local, weapon, cmd, hitbox_position);

	if (best_target == -1)
		return;

	C_BasePlayer* entity = (C_BasePlayer*)g_EntityList->GetClientEntity(best_target);
	if (!entity)
		return;

	if (get_distance(local->GetEyePos(), hitbox_position) > 8192.0f)
		return;

	compute_angle(local->GetEyePos(), hitbox_position, aim_angle);
	Math::NormalizeAngles(aim_angle);

	aim_angle -= get_randomized_recoil(local);
	aim_angle += get_randomized_angles(local);

	Math::NormalizeAngles(view_angle);

	delta_angle = view_angle - aim_angle;
	Math::NormalizeAngles(delta_angle);

	float randomSmoothing = 1.0f;

	if (randomized_smooth > 1.0f)
		randomSmoothing = random_number_range(randomized_smooth / 10.0f, 1.0f);

	final_angle = view_angle - delta_angle / aim_smooth * randomSmoothing;
	Math::NormalizeAngles(final_angle);

	if (!sanitize_angles(final_angle))
		return;

	cmd->viewangles = final_angle;
	g_EngineClient->SetViewAngles(cmd->viewangles);
}

void legitbot::weapon_settings(C_BaseCombatWeapon* weapon)
{
	if (!weapon)
		return;


	if (weapon->IsPistol())
	{

		hitbox = g_Options.hitbox_pistols;
		aim_key = g_Options.aimkey;
		aim_smooth = g_Options.legit_smooth_pistols;
		aim_fov = g_Options.legit_fov_pistols;
		randomized_smooth = 1;
		recoil_min = g_Options.legit_rcsmin_pistols;
		recoil_max = g_Options.legit_rcsmax_pistols;
		randomized_angle = 1;
	}
	else if (weapon->IsRifle())
	{
		hitbox = g_Options.hitbox_rifles;
		aim_key = g_Options.aimkey;
		aim_smooth = g_Options.legit_smooth_rifles;
		aim_fov = g_Options.legit_fov_rifles;
		randomized_smooth = 1;
		recoil_min = g_Options.legit_rcsmin_rifles;
		recoil_max = g_Options.legit_rcsmax_rifles;
		randomized_angle = 1;
	}
	else if (weapon->IsSniper())
	{
		hitbox = g_Options.hitbox_snipers;
		aim_key = g_Options.aimkey;
		aim_smooth = g_Options.legit_smooth_Snipers;
		aim_fov = g_Options.legit_fov_Snipers;
		randomized_smooth = 1;
		recoil_min = g_Options.legit_rcsmin_Snipers;
		recoil_max = g_Options.legit_rcsmax_Snipers;
		randomized_angle = 1;
	}

}

QAngle legitbot::get_randomized_recoil(C_BasePlayer *local)
{
	QAngle compensatedAngles = (local->m_aimPunchAngle() * 2.0f) * (random_number_range(recoil_min, recoil_max) / 100.0f);
	Math::NormalizeAngles(compensatedAngles);

	return (local->m_iShotsFired() > 1 ? compensatedAngles : QAngle(0.0f, 0.0f, 0.0f));
}

QAngle legitbot::get_randomized_angles(C_BasePlayer *local)
{
	QAngle randomizedValue = QAngle(0.0f, 0.0f, 0.0f);

	float randomRate = random_number_range(-randomized_angle, randomized_angle);
	float randomDeviation = random_number_range(-randomized_angle, randomized_angle);

	switch (rand() % 2)
	{
	case 0:
		randomizedValue.pitch = (randomRate * cos(randomDeviation));
		randomizedValue.yaw = (randomRate * cos(randomDeviation));
		randomizedValue.roll = (randomRate * cos(randomDeviation));
		break;
	case 1:
		randomizedValue.pitch = (randomRate * sin(randomDeviation));
		randomizedValue.yaw = (randomRate * sin(randomDeviation));
		randomizedValue.roll = (randomRate * sin(randomDeviation));
		break;
	}

	Math::NormalizeAngles(randomizedValue);

	return (local->m_iShotsFired() > 1 ? randomizedValue : QAngle(0.0f, 0.0f, 0.0f));
}

bool legitbot::get_hitbox(C_BasePlayer *local, C_BasePlayer *entity, Vector &destination)
{
	int bestHitbox = -1;
	float best_fov = aim_fov;

	int hitboxspots = hitbox;

	switch (hitbox)
	{
	case 0:
		break;
	case 1:
		hitboxspots = HITBOX_HEAD;
		break;
	case 2:
		hitboxspots = HITBOX_NECK;
		break;
	case 3:
		hitboxspots = HITBOX_CHEST;
		break;
	case 4:
		hitboxspots = HITBOX_STOMACH;
		break;
	case 5:
		hitboxspots = HITBOX_NECK, HITBOX_HEAD, HITBOX_CHEST, HITBOX_STOMACH;
		break;
	}

	std::vector<int> hitboxes = { hitboxspots };
	for (auto hitbox : hitboxes)
	{
		Vector temp;
		if (!entity->GetHitboxPos(hitbox, temp))
			continue;

		float fov = get_fov(view_angle, compute_angle(local->GetEyePos(), temp));
		if (fov < best_fov)
		{
			best_fov = fov;
			bestHitbox = hitbox;
		}
	}

	if (bestHitbox != -1)
	{
		if (!entity->GetHitboxPos(bestHitbox, destination))
			return true;
	}

	return false;
}



int legitbot::get_target(C_BasePlayer *local, C_BaseCombatWeapon *weapon, CUserCmd *cmd, Vector &destination)
{
	int best_target = -1;
	float best_fov = FLT_MAX;

	g_EngineClient->GetViewAngles(view_angle);

	for (int i = 1; i <= g_GlobalVars->maxClients; i++)
	{
		C_BasePlayer *entity = (C_BasePlayer*)g_EntityList->GetClientEntity(i);
		if (!entity
			|| entity == local
			|| entity->IsDormant()
			|| entity->m_lifeState() != LIFE_ALIVE
			|| entity->m_bGunGameImmunity()
			|| entity->GetClientClass()->m_ClassID != ClassId_CCSPlayer
			|| !(entity->m_fFlags() & FL_ONGROUND))
			continue;

		if (true)
		{
			if (entity->m_iTeamNum() == local->m_iTeamNum())
				continue;
		}

		Vector hitbox;
		if (get_hitbox(local, entity, hitbox))
			continue;

		float fov = get_fov(view_angle + (local->m_aimPunchAngle() * 2.0f), compute_angle(local->GetEyePos(), hitbox));
		if (fov < best_fov && fov < aim_fov)
		{
			if (local->CanSeePlayer(entity, hitbox))
			{
				best_fov = fov;
				destination = hitbox;
				best_target = i;
			}
		}
	}

	return best_target;
}