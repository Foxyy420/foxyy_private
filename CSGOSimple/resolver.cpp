#include "resolver.h"
#include "options.hpp"
#include "helpers\math.hpp"

namespace Resolver
{
	std::vector<int64_t> Players = {};

	std::vector<std::pair<C_BasePlayer*, QAngle>> player_data;

	int ResolverStage[64];

	float Bolbilize(float Yaw)
	{
		if (Yaw > 180)
			Yaw -= (round(Yaw / 360) * 360.f);
		else if (Yaw < -180)
			Yaw += (round(Yaw / 360) * -360.f);

		return Yaw;
	}

	Vector CalcAngle69(Vector dst, Vector src)
	{
		Vector angles;

		double delta[3] = { (src.x - dst.x), (src.y - dst.y), (src.z - dst.z) };
		double hyp = sqrt(delta[0] * delta[0] + delta[1] * delta[1]);
		angles.x = (float)(atan(delta[2] / hyp) * 180.0 / 3.14159265);
		angles.y = (float)(atanf(delta[1] / delta[0]) * 57.295779513082f);
		angles.z = 0.0f;

		if (delta[0] >= 0.0)
		{
			angles.y += 180.0f;
		}

		return angles;
	}

	void OnFrameStageNotify(ClientFrameStage_t stage)
	{
		// FORCE LBY RESOLVER

		/*
		if (!g_EngineClient->IsInGame())
		return;

		C_BasePlayer* localplayer = (C_BasePlayer*)g_EntityList->GetClientEntity(g_EngineClient->GetLocalPlayer());

		if (!localplayer)
		return;

		if (stage == FRAME_NET_UPDATE_POSTDATAUPDATE_START)
		{
		for (int i = 1; i < g_EngineClient->GetMaxClients(); ++i)
		{
		C_BasePlayer* player = (C_BasePlayer*)g_EntityList->GetClientEntity(i);

		if (!player
		|| player == localplayer
		|| player->IsDormant()
		|| !player->IsAlive()
		|| player->m_bGunGameImmunity()
		|| player->m_iTeamNum() == localplayer->m_iTeamNum())
		continue;

		player_info_t entityInformation;
		g_EngineClient->GetPlayerInfo(i, &entityInformation);

		if (g_Options.resolver && std::find(Players.begin(), Players.end(), entityInformation.xuid_high) == Players.end())
		continue;

		player_data.push_back(std::pair<C_BasePlayer*, QAngle>(player, player->m_angEyeAngles()));

		player->m_angEyeAngles().yaw = player->m_flLowerBodyYawTarget();
		}
		}
		else if (stage == FRAME_RENDER_END)
		{
		for (unsigned long i = 0; i < player_data.size(); i++)
		{
		std::pair<C_BasePlayer*, QAngle> player_aa_data = player_data[i];
		player_aa_data.first->m_angEyeAngles() = player_aa_data.second;
		}

		player_data.clear();
		}
		*/

		
		if (g_EngineClient->IsInGame() && g_EngineClient->IsConnected() && g_Options.best_resolver)
		{
			auto pLocal = (C_BasePlayer*)g_EntityList->GetClientEntity(g_EngineClient->GetLocalPlayer());

			for (int i = 1; i < g_EntityList->GetHighestEntityIndex(); i++)
			{
				auto pEnt = (C_BasePlayer*)g_EntityList->GetClientEntity(i);

				if (pEnt)
				{
					if (pEnt->IsAlive() && pEnt->IsPlayer() && g_LocalPlayer->IsAlive())
					{
						static float oldlby[65];
						static float lastlby[65];
						static float standtime[65];
						static float lbystandtime[65];
						static float last_moving_time[65];
						bool lby_updated;
						float bodyeyedelta = pEnt->m_angEyeAngles().yaw - pEnt->m_flLowerBodyYawTarget();
						const bool is_moving = pEnt->m_vecVelocity().Length2D() > 0.1;
						const bool could_be_slowmo = pEnt->m_vecVelocity().Length2D() < 36;
						const bool is_crouching = pEnt->m_fFlags() & FL_DUCKING;
						static float last_moving_lby[64];
						standtime[pEnt->EntIndex()] = g_GlobalVars->curtime - last_moving_time[pEnt->EntIndex()];
						lbystandtime[pEnt->EntIndex()] = g_GlobalVars->curtime - last_moving_time[pEnt->EntIndex()];

						if (lbystandtime[pEnt->EntIndex()] >= 1.1)
						{
							lbystandtime[pEnt->EntIndex()] -= 1.1;
						}
						if (pEnt->m_flLowerBodyYawTarget() != oldlby[pEnt->EntIndex()])
						{
							lby_updated = true;
							oldlby[pEnt->EntIndex()] = pEnt->m_flLowerBodyYawTarget();
						}
						else
						{
							lby_updated = false;
						}

						if (lby_updated)
						{
							pEnt->m_angEyeAngles().yaw = pEnt->m_flLowerBodyYawTarget();
						}
						else if (is_moving && !could_be_slowmo)
						{
							ResolverStage[pEnt->EntIndex()] = 1;
							last_moving_lby[pEnt->EntIndex()] = pEnt->m_flLowerBodyYawTarget();
							lastlby[pEnt->EntIndex()] = last_moving_lby[pEnt->EntIndex()];
							last_moving_time[pEnt->EntIndex()] = g_GlobalVars->curtime;
							pEnt->m_angEyeAngles().yaw = pEnt->m_flLowerBodyYawTarget();
						}
						else if (lbystandtime[pEnt->EntIndex()] > 1.05 && lbystandtime[pEnt->EntIndex()] < 1.125)
						{
							ResolverStage[pEnt->EntIndex()] = 4;
							pEnt->m_angEyeAngles().yaw = pEnt->m_flLowerBodyYawTarget();
						}
						else if (is_moving && !is_crouching)
						{
							ResolverStage[pEnt->EntIndex()] = 2;
							pEnt->m_angEyeAngles().yaw = last_moving_lby[pEnt->EntIndex()];
						}
						else
						{
							if ((standtime[pEnt->EntIndex()]) > 3)
							{
								ResolverStage[pEnt->EntIndex()] = 3;
								switch (g_LocalPlayer->m_iShotsFired() % 3)
								{
								case 0: pEnt->m_angEyeAngles().yaw = Bolbilize(CalcAngle69(pEnt->m_vecOrigin(), pLocal->m_vecOrigin()).y + 70); break;
								case 1: pEnt->m_angEyeAngles().yaw = Bolbilize(CalcAngle69(pEnt->m_vecOrigin(), pLocal->m_vecOrigin()).y + 180); break;
								case 2: pEnt->m_angEyeAngles().yaw = Bolbilize(CalcAngle69(pEnt->m_vecOrigin(), pLocal->m_vecOrigin()).y - 70); break;
								}
							}
							else
							{
								ResolverStage[pEnt->EntIndex()] = 2;
								pEnt->m_angEyeAngles().yaw = last_moving_lby[pEnt->EntIndex()];
							}
						}
					}
				}
			}
		}
		

		/*		
		if (g_EngineClient->IsInGame() && g_EngineClient->IsConnected() && g_Options.resolver)
		{
			for (int i = 1; i < g_EntityList->GetHighestEntityIndex(); i++)
			{
				auto pEntity = (C_BasePlayer*)g_EntityList->GetClientEntity(i);

				if (pEntity)
				{
					if (pEntity->IsAlive() && pEntity->IsPlayer())
					{
						bool InFakewalk = pEntity->IsInFakewalk();
						bool IsBreakingLBY = pEntity->IsBreakingLBY();
						bool Moving = pEntity->IsMoving();
					}
				}
			}
		}
		*/
	}

	void FireGameEvent(IGameEvent* event)
	{
		if (!event)
			return;

		if (strcmp(event->GetName(), "player_connect_full") != 0 && strcmp(event->GetName(), "cs_game_disconnected") != 0)
			return;

		if (event->GetInt("userid") && g_EngineClient->GetPlayerForUserID(event->GetInt("userid")) != g_EngineClient->GetLocalPlayer())
			return;

		Players.clear();
	}
}

