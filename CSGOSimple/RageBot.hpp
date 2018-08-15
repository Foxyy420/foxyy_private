#pragma once
#include "valve_sdk\csgostructs.hpp"
#include "helpers\math.hpp"
//=================================================

namespace Globals
{
	extern int TargetID;
}

class C_IWAimbot
{
public:
	CUserCmd *usercmd = nullptr;
	bool GetHiboxPosition(C_BasePlayer* pPlayer, Vector &vPos, int iHitBox);
	bool IsVisible(C_BasePlayer *TargetEntity, C_BasePlayer *LocalPlayer, const Vector &TargetPosition, const Vector &EyePos);
	bool TraceTargetHitpoint(C_BasePlayer* pLocalEntity, C_BaseCombatWeapon* pLocalWeapon, C_BasePlayer* pBaseEntity, Vector& vecHitPoint, CUserCmd* pUserCmd);
	void CalcAngle(Vector &vSource, Vector &vDestination, QAngle &qAngle);
	//float GetFov(Vector vLocalOrigin, Vector vPosition, Vector vForward);
	void Main(CUserCmd* pUserCmd, C_BasePlayer* pLocal, bool& bSendPacket);
	void legitaa(CUserCmd* cmd, bool& bSendPacket);
	void fixaa();
	void Fakelag(CUserCmd *pCmd, bool &bSendPacket);
	//void DoNoRecoil(CUserCmd *pCmd, Vector AimPunch);
};
