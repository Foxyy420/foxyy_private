#pragma once
#include "../MainInclude.hpp"

class C_BasePlayer;
class CUserCmd;
class QAngle;

namespace Misc
{
	void OnCreateMove(CUserCmd* cmd);
	void AutoStrafe(CUserCmd* cmd, QAngle oldangles);
}

