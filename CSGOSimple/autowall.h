#pragma once
#include "valve_sdk\csgostructs.hpp"
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



struct FireBulletData
{
	FireBulletData(const Vector &eye_pos) : src(eye_pos)
	{
	}

	Vector						src;
	trace_t						enter_trace;
	Vector						direction;
	CTraceFilter				filter;
	float						trace_length;
	float						trace_length_remaining;
	float						current_damage;
	int							penetrate_count;
};
class C_AutoWall
{
public:
	bool IsBreakableEntity(C_BasePlayer* pBaseEntity);
	bool HandleBulletPenetration(C_BaseCombatWeapon *wpn_data, FireBulletData& data);
	bool PenetrateWall(C_BasePlayer* pBaseEntity, C_BaseCombatWeapon* pWeapon, const Vector& vecPoint, float& flDamage);
	bool SimulateFireBullet(C_BasePlayer* pBaseEntity, C_BaseCombatWeapon* pWeapon, FireBulletData& BulletData);
	bool TraceDidHitWorld(trace_t* pTrace);
	bool TraceDidHitNonWorldEntity(trace_t* pTrace);
	bool TraceToExit(Vector& vecEnd, trace_t* pEnterTrace, Vector vecStart, Vector vecDir, trace_t* pExitTrace);
	void UTIL_TraceLine(const Vector& vecStart, const Vector& vecEnd, unsigned int nMask, C_BasePlayer* pCSIgnore, trace_t* pTrace);
	void ScaleDamage(int iHitgroup, C_BasePlayer* pBaseEntity, float flWeaponArmorRatio, float& flDamage);
};