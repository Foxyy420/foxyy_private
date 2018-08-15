#include "BackTrack.h"


#define TICK_INTERVAL			(g_GlobalVars->interval_per_tick)
#define TIME_TO_TICKS( dt )		( (int)( 0.5f + (float)(dt) / TICK_INTERVAL ) )

void TimeWarp::CreateMove(CUserCmd* cmd)
{
	if (!g_Options.misc_backtrack) return;

	int bestTargetIndex = -1;
	float bestFov = FLT_MAX;

	if (!g_LocalPlayer->IsAlive())
		return;

	for (int i = 1; i < g_EngineClient->GetMaxClients(); i++)
	{
		auto pEntity = static_cast<C_BasePlayer*>(g_EntityList->GetClientEntity(i));
		if (!pEntity || !g_LocalPlayer) continue;
		if (!pEntity->IsPlayer()) continue;
		if (pEntity == g_LocalPlayer) continue;
		if (pEntity->IsDormant()) continue;
		if (!pEntity->IsAlive()) continue;
		if (pEntity->m_iTeamNum() == g_LocalPlayer->m_iTeamNum()) continue;

		float simtime = pEntity->m_flSimulationTime();
		Vector hitboxPos = pEntity->GetHitboxPos(0);

		TimeWarpData[i][cmd->command_number % (NUM_OF_TICKS + 1)] = StoredData{ simtime, hitboxPos };
		Vector ViewDir;
		Math::AngleVectors(cmd->viewangles + (g_LocalPlayer->m_aimPunchAngle() * 2.f), ViewDir);
		float FOVDistance = Math::DistancePointToLine(hitboxPos, g_LocalPlayer->GetEyePos(), ViewDir);

		if (bestFov > FOVDistance)
		{
			bestFov = FOVDistance;
			bestTargetIndex = i;
		}
	}

	float bestTargetSimTime = -1;
	if (bestTargetIndex != -1)
	{
		float tempFloat = FLT_MAX;
		Vector ViewDir;
		Math::AngleVectors(cmd->viewangles + (g_LocalPlayer->m_aimPunchAngle() * 2.f), ViewDir);
		for (int t = 0; t < NUM_OF_TICKS; ++t)
		{
			float tempFOVDistance = Math::DistancePointToLine(TimeWarpData[bestTargetIndex][t].hitboxPos, g_LocalPlayer->GetEyePos(), ViewDir);
			if (tempFloat > tempFOVDistance && TimeWarpData[bestTargetIndex][t].simtime > g_LocalPlayer->m_flSimulationTime() - 1)
			{
				if (g_LocalPlayer->CanSeePlayer(static_cast<C_BasePlayer*>(g_EntityList->GetClientEntity(bestTargetIndex)), TimeWarpData[bestTargetIndex][t].hitboxPos))
				{
					tempFloat = tempFOVDistance;
					bestTargetSimTime = TimeWarpData[bestTargetIndex][t].simtime;
				}
			}
		}

		if (bestTargetSimTime >= 0 && cmd->buttons & IN_ATTACK)
			cmd->tick_count = TIME_TO_TICKS(bestTargetSimTime);
	}
}

/*
void TimeWarp::draw_lines()
{
	player_info_t info;
	auto pLocal = static_cast<C_BasePlayer*>(g_EntityList->GetClientEntity(g_EngineClient->GetLocalPlayer()));
	for (int i = 0; i <= g_EntityList->GetHighestEntityIndex(); i++)
	{
		auto pEntity = reinterpret_cast<C_BasePlayer*>(g_EntityList->GetClientEntity(i));
		if (pEntity && pEntity != pLocal && !pEntity->IsDormant())
		{
			if (g_EngineClient->GetPlayerInfo(i, &info) && pEntity->m_iHealth() > 0)
			{
				if (g_Options.misc_backtrack)
				{
					if (g_Options.misc_backtrack) //egyenlõre jó     "Verdana", 11, 0, 1, 0, FONTFLAG_OUTLINE);
					{
						if (pLocal->m_iHealth() > 0)
						{
							for (int t = 0; t < 13; ++t)
							{
								Vector screenbacktrack[64][NUM_OF_TICKS];
								if (TimeWarpData[i][t].simtime && TimeWarpData[i][t].simtime + 1 > pLocal->m_flSimulationTime())
								{
									if (Math::WorldToScreen(TimeWarpData[i][t].hitboxPos, screenbacktrack[i][t]))
									{
										//g_VGuiSurface->DrawSetTextColor(Color::White);
										//g_VGuiSurface->DrawOutlinedRect(screenbacktrack[i][t].x, screenbacktrack[i][t].y, screenbacktrack[i][t].x + 2, screenbacktrack[i][t].y + 2);
								
										g_VGuiSurface->DrawSetTextPos(screenbacktrack[i][t].x, screenbacktrack[i][t].y);
										g_VGuiSurface->DrawSetTextFont(esp_font);
										wchar_t buf[128];
										if (MultiByteToWideChar(CP_UTF8, 0, "+", -1, buf, 128) > 0)
											g_VGuiSurface->DrawPrintText(buf, 1, FONT_DRAW_DEFAULT);

										
									}
								}
							}
						}
					}
				}
			}
		}
	}
	
}

*/