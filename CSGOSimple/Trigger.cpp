#include "Trigger.h"

namespace triggerbot {

	void Triggerbot(CUserCmd* pCmd) {

		if (g_LocalPlayer->IsAlive() && g_LocalPlayer->IsAlive() && !(g_LocalPlayer->m_lifeState() & LIFE_DYING))
		{
			auto pWeapon = g_LocalPlayer->m_hActiveWeapon();

			if (pWeapon)
			{
				static bool enable = false;
				static int	key = 0;
				static bool head = false;
				static bool arms = false;
				static bool chest = false;
				static bool stomach = false;
				static bool legs = false;

				if (pWeapon->IsPistol()) enable = g_Options.Trigger.Pistols_Enable;
				else if (pWeapon->IsRifle()) enable = g_Options.Trigger.Rifles_Enable;
				else if (pWeapon->IsSniper()) enable = g_Options.Trigger.Snipers_Enable;
				
				if (pWeapon->IsPistol()) key = g_Options.Trigger.Pistols_Key;
				else if (pWeapon->IsRifle()) key = g_Options.Trigger.Rifles_Key;
				else if (pWeapon->IsSniper()) key = g_Options.Trigger.Snipers_Key;

				if (pWeapon->IsPistol()) head = g_Options.Trigger.Pistols_Head;
				else if (pWeapon->IsRifle()) head = g_Options.Trigger.Rifles_Head;
				else if (pWeapon->IsSniper()) head = g_Options.Trigger.Snipers_Head;

				if (pWeapon->IsPistol()) arms = g_Options.Trigger.Pistols_Arms;
				else if (pWeapon->IsRifle()) arms = g_Options.Trigger.Rifles_Arms;
				else if (pWeapon->IsSniper()) arms = g_Options.Trigger.Snipers_Arms;

				if (pWeapon->IsPistol()) chest = g_Options.Trigger.Pistols_Chest;
				else if (pWeapon->IsRifle()) chest = g_Options.Trigger.Rifles_Chest;
				else if (pWeapon->IsSniper()) chest = g_Options.Trigger.Snipers_Chest;

				if (pWeapon->IsPistol()) stomach = g_Options.Trigger.Pistols_Stomach;
				else if (pWeapon->IsRifle()) stomach = g_Options.Trigger.Rifles_Stomach;
				else if (pWeapon->IsSniper()) stomach = g_Options.Trigger.Snipers_Stomach;

				if (pWeapon->IsPistol()) legs = g_Options.Trigger.Pistols_Legs;
				else if (pWeapon->IsRifle()) legs = g_Options.Trigger.Rifles_Legs;
				else if (pWeapon->IsSniper()) legs = g_Options.Trigger.Snipers_Legs;

				Vector src, dst, forward;
				trace_t tr;
				Ray_t ray;
				CTraceFilter filter;

				QAngle viewangle = pCmd->viewangles;

				viewangle += g_LocalPlayer->m_aimPunchAngle() * 2.f;

				Math::AngleVectors2(viewangle, forward);

				forward *= g_LocalPlayer->m_hActiveWeapon()->GetCSWeaponData()->flRange;
				filter.pSkip = g_LocalPlayer;
				src = g_LocalPlayer->GetEyePos();
				dst = src + forward;
				ray.Init(src, dst);

				g_EngineTrace->TraceRay(ray, 0x46004003, &filter, &tr);

				if (!tr.hit_entity)
					return;

				int hitgroup = tr.hitgroup;
				bool didHit = false;
				if ((head && tr.hitgroup == 1)
					|| (chest && tr.hitgroup == 2)
					|| (stomach && tr.hitgroup == 3)
					|| (arms && (tr.hitgroup == 4 || tr.hitgroup == 5))
					|| (legs && (tr.hitgroup == 6 || tr.hitgroup == 7)))
				{
					didHit = true;
				}


				if (didHit && (tr.hit_entity->GetBaseEntity()->m_iTeamNum() != g_LocalPlayer->m_iTeamNum()))
				{
					if (key > 0 && GetAsyncKeyState(key) & 0x8000 && enable)
					{
						pCmd->buttons |= IN_ATTACK;
					}
				}

			}
		}
	}
}


