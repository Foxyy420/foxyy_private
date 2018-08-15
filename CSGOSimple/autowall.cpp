#include "autowall.h"
#include "RageBot.hpp"
#include "helpers\math.hpp"

#define CHAR_TEXT_WOOD 87
#define CHAT_TEXT_GLASS 2
#define CHAT_TEXT_METAL 60

#define	HITGROUP_GENERIC	0
#define	HITGROUP_HEAD		1
#define	HITGROUP_CHEST		2
#define	HITGROUP_STOMACH	3
#define HITGROUP_LEFTARM	4	
#define HITGROUP_RIGHTARM	5
#define HITGROUP_LEFTLEG	6
#define HITGROUP_RIGHTLEG	7
#define HITGROUP_GEAR		10			// alerts NPC, but doesn't do damage or bleed (1/100th damage)



bool C_AutoWall::TraceDidHitWorld(trace_t* pTrace)
{
	return(pTrace->hit_entity == g_EntityList->GetClientEntity(0));
}
bool C_AutoWall::TraceDidHitNonWorldEntity(trace_t* pTrace)
{
	if (!pTrace)
		return false;

	if (pTrace->hit_entity == NULL)
		return false;

	if (TraceDidHitWorld(pTrace))
		return false;

	return true;
}

float VectorLength(Vector v)
{
	return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

bool C_AutoWall::HandleBulletPenetration(C_BaseCombatWeapon *wpn_data, FireBulletData& data)
{
	CCSWeaponInfo* pWeaponInfo = wpn_data->GetCSWeaponData();
	surfacedata_t *enter_surface_data = g_PhysSurface->GetSurfaceData(data.enter_trace.surface.surfaceProps);
	int enter_material = enter_surface_data->game.material;
	float enter_surf_penetration_mod = enter_surface_data->game.flPenetrationModifier;
	data.trace_length += data.enter_trace.fraction * data.trace_length_remaining;
	data.current_damage *= pow(pWeaponInfo->flRangeModifier, (data.trace_length * 0.002));
	data.current_damage *= (float)pow(pWeaponInfo->flRangeModifier, (data.trace_length * 0.002f));
	if ((data.trace_length > 3000.f) || (enter_surf_penetration_mod < 0.1f))data.penetrate_count = 0;
	if (data.penetrate_count <= 0)return false;
	Vector dummy;
	trace_t trace_exit;
	if (!TraceToExit(dummy, &data.enter_trace, data.enter_trace.endpos, data.direction, &trace_exit)) return false;
	surfacedata_t *exit_surface_data = g_PhysSurface->GetSurfaceData(trace_exit.surface.surfaceProps);
	int exit_material = exit_surface_data->game.material;
	float exit_surf_penetration_mod = exit_surface_data->game.flPenetrationModifier;
	float final_damage_modifier = 0.16f;
	float combined_penetration_modifier = 0.0f;
	if (((data.enter_trace.contents & CONTENTS_GRATE) != 0) || (enter_material == 89) || (enter_material == 71)) { combined_penetration_modifier = 3.0f; final_damage_modifier = 0.05f; }
	else { combined_penetration_modifier = (enter_surf_penetration_mod + exit_surf_penetration_mod) * 0.5f; }
	if (enter_material == exit_material)
	{
		if (exit_material == 87 || exit_material == 85)combined_penetration_modifier = 3.0f;
		else if (exit_material == 76)combined_penetration_modifier = 2.0f;
	}
	float v34 = fmaxf(0.f, 1.0f / combined_penetration_modifier);
	float v35 = (data.current_damage * final_damage_modifier) + v34 * 3.0f * fmaxf(0.0f, (3.0f / pWeaponInfo->flRangeModifier) * 1.25f);
	float thickness = VectorLength(trace_exit.endpos - data.enter_trace.endpos);
	thickness *= thickness;
	thickness *= v34;
	thickness /= 24.0f;
	float lost_damage = fmaxf(0.0f, v35 + thickness);
	if (lost_damage > data.current_damage)return false;
	if (lost_damage >= 0.0f)data.current_damage -= lost_damage;
	if (data.current_damage < 1.0f) return false;
	data.src = trace_exit.endpos;
	data.penetrate_count--;

	return true;
}

bool C_AutoWall::PenetrateWall(C_BasePlayer* pBaseEntity, C_BaseCombatWeapon* pWeapon, const Vector& vecPoint, float& flDamage)
{
	if (!pBaseEntity || !pWeapon)
		return false;
	FireBulletData BulletData(pBaseEntity->GetEyePos());
	BulletData.filter.pSkip = pBaseEntity;
	QAngle qAngles;
	//	MessageBox(NULL, "Kalkuluje end¿ela", "Maciej pFadddWare", NULL);
	//C_IWAimbot Aimbot;
	//Aimbot.CalcAngle(BulletData.src, const_cast<Vector&>(vecPoint), qAngles);

	Vector vDelta = BulletData.src - vecPoint;

	float fHyp = (vDelta.x * vDelta.x) + (vDelta.y * vDelta.y);

	float fRoot;

	//MessageBox(NULL, "FastSQRT", "Maciej pFadddWare", NULL);
	__asm
	{
		sqrtss xmm0, fHyp
		movss fRoot, xmm0
	}
	//	MessageBox(NULL, "RAD2DEG", "Maciej pFadddWare", NULL);

	qAngles.pitch = RAD2DEG(atan(vDelta.z / fRoot));
	//	MessageBox(NULL, "RAD2DEG2", "Maciej pFadddWare", NULL);
	qAngles.yaw = RAD2DEG(atan(vDelta.y / vDelta.x));
	//MessageBox(NULL, "RAD2DEG3", "Maciej pFadddWare", NULL);

	//	MessageBox(NULL, "delta", "Maciej pFadddWare", NULL);
	if (vDelta.x >= 0.0f)
		qAngles.yaw += 180.0f;
	//MessageBox(NULL, "normalization", "Maciej pFadddWare", NULL);
	//Math::NormalizeAngles(qAngles);


	//MessageBox(NULL, "Kasia ma ciasnom cipeczke wiedziales ?", "Maciej pFadddWare", NULL);
	Math::AngleVectors(qAngles, BulletData.direction);
	//MessageBox(NULL, "Kasia ma ciasnom cipeczke wiedziales ?", "Maciej pFadddWare", NULL);
	Math::NormalizeAnglesvec(BulletData.direction);
	//MessageBox(NULL, "Kasia ma ciasnom cipeczke wiedziales ?", "Maciej pFadddWare", NULL);
	if (!SimulateFireBullet(pBaseEntity, pWeapon, BulletData))
	{
		return false;
	}

	flDamage = BulletData.current_damage;

	return true;
}
float GetHitgroupDamageMult(int iHitgroup)
{
	switch (iHitgroup)
	{
	case HITGROUP_GENERIC:
		return 1.f;
	case HITGROUP_HEAD:
		return 4.f;
	case HITGROUP_CHEST:
		return 1.f;
	case HITGROUP_STOMACH:
		return 1.5f;
	case HITGROUP_LEFTARM:
		return 1.f;
	case HITGROUP_RIGHTARM:
		return 1.f;
	case HITGROUP_LEFTLEG:
		return 0.75f;
	case HITGROUP_RIGHTLEG:
		return 0.75f;
	case HITGROUP_GEAR:
		return 1.f;
	default:
		break;
	}

	return 1.f;
}
void C_AutoWall::ScaleDamage(int iHitgroup, C_BasePlayer* pBaseEntity, float flWeaponArmorRatio, float& flDamage)
{
	if (!pBaseEntity)
		return;

	flDamage *= GetHitgroupDamageMult(iHitgroup);

	if (pBaseEntity->m_ArmorValue() > 0)
	{
		if (iHitgroup == HITGROUP_HEAD)
		{
			if (pBaseEntity->m_bHasHelmet())
				flDamage *= (flWeaponArmorRatio * 1.5f);
		}
		else flDamage *= (flWeaponArmorRatio * 0.5f);
	}
}
int GetPlayerModifiedDamage(const float &constdamage, bool isHeadshot, C_BaseCombatWeapon *Weapon, C_BasePlayer *targetEntity)
{
	float damage = constdamage;

	int armor = targetEntity->m_ArmorValue();
	if ((armor > 0) && (!isHeadshot || (isHeadshot)))
	{
		float weaponArmorRatio = (.5f * Weapon->GetCSWeaponData()->flArmorRatio);

		float newdamage = (weaponArmorRatio * damage);
		float armordamage = ((damage - newdamage) * .5f);

		if (armordamage <= (float)armor)
		{
			armordamage = armordamage;
		}
		else
		{
			newdamage = (damage + ((float)armor * -2.f));
			armordamage = (int)armor;
		}

		damage = newdamage;
	}
	else
	{
		damage = damage;
	}

	return (int)damage;
}
//=================================================
float GetHitgroupModifiedDamage(float dmg, int hitgroup)
{
	static float hitgroupModifiers[] = { 1.f, 4.f, 1.f, 1.25f, 1.f, 1.f, .75f, .75f };
	return (dmg * hitgroupModifiers[hitgroup]);
}
void C_AutoWall::UTIL_TraceLine(const Vector& vecStart, const Vector& vecEnd, unsigned int nMask, C_BasePlayer* pCSIgnore, trace_t* pTrace)
{
	Ray_t ray;
	ray.Init(vecStart, vecEnd);

	CTraceFilter filter;
	filter.pSkip = pCSIgnore;
	g_EngineTrace->TraceRay(ray, nMask, &filter, pTrace);
}

bool C_AutoWall::SimulateFireBullet(C_BasePlayer* pBaseEntity, C_BaseCombatWeapon* pWeapon, FireBulletData& BulletData)
{
	if (!pBaseEntity || !pWeapon)
		return false;

	BulletData.penetrate_count = 4;
	BulletData.trace_length = 0.0f;

	CCSWeaponInfo* pWeaponInfo = pWeapon->GetCSWeaponData();

	BulletData.current_damage = (float)pWeaponInfo->iDamage;
	while ((BulletData.penetrate_count > 0) && (BulletData.current_damage >= 1.0f))
	{
		BulletData.trace_length_remaining = pWeaponInfo->flRange - BulletData.trace_length;
		auto vecEnd = BulletData.src + BulletData.direction * BulletData.trace_length_remaining;
		UTIL_TraceLine(BulletData.src, vecEnd, 0x4600400B, pBaseEntity, &BulletData.enter_trace);


		if (BulletData.enter_trace.fraction == 1.0f)
			break;

		if ((BulletData.enter_trace.hitgroup <= 7) && (BulletData.enter_trace.hitgroup > 0) && pBaseEntity->m_iTeamNum() != BulletData.enter_trace.hit_entity->GetBaseEntity()->m_iTeamNum())
		{
			BulletData.trace_length += BulletData.enter_trace.fraction * BulletData.trace_length_remaining;
			BulletData.current_damage *= (float)pow(pWeaponInfo->flRangeModifier, BulletData.trace_length * 0.002);
			//	MessageBox(NULL, "Kasia ma ciasnom cipeczke wiedziales ?", "Macije pFadddWare", NULL);
			ScaleDamage(BulletData.enter_trace.hitgroup, (C_BasePlayer*)(BulletData.enter_trace.hit_entity)->GetBaseEntity(), pWeaponInfo->flArmorRatio, BulletData.current_damage);
			GetPlayerModifiedDamage(GetHitgroupModifiedDamage(BulletData.current_damage, BulletData.enter_trace.hitgroup), (BulletData.enter_trace.hitgroup == 1), pWeapon, pBaseEntity); //Comment here
			return true;
		}

		if (!HandleBulletPenetration(pWeapon, BulletData))
			break;
	}

	return false;
}


bool C_AutoWall::TraceToExit(Vector& vecEnd, trace_t* pEnterTrace, Vector vecStart, Vector vecDir, trace_t* pExitTrace)
{
	auto flDistance = 0.0f;
	while (flDistance <= 90.0f)
	{
		flDistance += 4.0f;
		vecEnd = vecStart + vecDir * flDistance;

		auto iPointContents = g_EngineTrace->GetPointContents(vecEnd, MASK_SHOT_HULL | CONTENTS_HITBOX, NULL);
		if (iPointContents & MASK_SHOT_HULL && (!(iPointContents & CONTENTS_HITBOX)))
			continue;

		auto vecNewEnd = vecEnd - (vecDir * 4.0f);
		UTIL_TraceLine(vecEnd, vecNewEnd, 0x4600400B, 0, pExitTrace);

		if (!pExitTrace->hit_entity)
		{
			return false;
		}

		if (pExitTrace->startsolid && pExitTrace->surface.flags & SURF_HITBOX)
		{
			UTIL_TraceLine(vecEnd, vecStart, 0x600400B, (C_BasePlayer*)pExitTrace->hit_entity, pExitTrace);
			if ((pExitTrace->fraction < 1.0f || pExitTrace->allsolid) && !pExitTrace->startsolid)
			{
				vecEnd = pExitTrace->endpos;
				return true;
			}
			continue;
		}

		if (!(pExitTrace->fraction < 1.0 || pExitTrace->allsolid || pExitTrace->startsolid) || pExitTrace->startsolid)
		{
			if (pExitTrace->hit_entity)
			{
				if (TraceDidHitNonWorldEntity(pEnterTrace) /*Comment here*//*&& pEnterTrace->m_pEnt->IsBreakableEntity()*/)
				{
					return true;
				}
			}
			continue;
		}

		if (((pExitTrace->surface.flags >> 7) & 1) && !((pEnterTrace->surface.flags >> 7) & 1))
			continue;


		if (pExitTrace->plane.normal.Dot(vecDir) <= 1.0f)
		{
			auto flFraction = pExitTrace->fraction * 4.0f;
			vecEnd = vecEnd - (vecDir * flFraction);
			return true;
		}
	}
	return false;
}