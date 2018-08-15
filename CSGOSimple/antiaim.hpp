#pragma once
#include "MainInclude.hpp"

namespace AntiAims
{
	float Distance(Vector a, Vector b);
	bool GetBestHeadAngle(QAngle& angle);
	bool HasViableEnemy();
	void DoAntiAimY(QAngle& angle, int command_number, bool bFlip, bool& clamp);
	void DoAntiAimX(QAngle& angle, bool bFlip, bool& clamp);
	void OnCreateMove(CUserCmd* cmd);
	void FakeWalk(CUserCmd* pCmd);
	//	void LegitAA(CUserCmd *pCmd, bool& bSendPacket);
}