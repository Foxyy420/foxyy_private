#include "glow.hpp"

#include "../valve_sdk/csgostructs.hpp"
#include "../options.hpp"
#include "visuals.hpp"


void Glow::RenderGlow()
{
	for (auto i = 0; i < g_GlowObjManager->m_GlowObjectDefinitions.Count(); i++)
	{
		auto &glowObject = g_GlowObjManager->m_GlowObjectDefinitions[i];
		auto entity = reinterpret_cast<C_BasePlayer*>(glowObject.m_pEntity);

		if (glowObject.IsUnused())
			continue;

		if (!entity)
			continue;

		auto class_id = entity->GetClientClass()->m_ClassID;
		Color color;

		switch (class_id)
		{
		case ClassId_CCSPlayer:
		{
			bool is_enemy = entity->m_iTeamNum() != g_LocalPlayer->m_iTeamNum();
			bool playerTeam = entity->m_iTeamNum() == 2;

			if (!g_Options.glow_players || !Visuals::ValidPlayer(entity, true))
				continue;

			if (!is_enemy && g_Options.esp_enemies_only)
				continue;

			glowObject.m_nGlowStyle = g_Options.players_glow_style;

			if (!is_enemy && g_Options.glow_enemies_only)
				continue;
			color = is_enemy ? g_Options.color_glow_enemy : g_Options.color_glow_ally;
			
			//color.SetColor(cur_color);

			break;
		}

		case ClassId_CBaseAnimating:

			if (!g_Options.glow_weapons)
				continue;

			//glowObject.m_nGlowStyle = g_Options.glow_others_style;

			color = Color::Blue;

			break;

		case ClassId_CPlantedC4:

			if (!g_Options.glow_weapons)
				continue;

			//glowObject.m_nGlowStyle = g_Options.glow_others_style;

			color = Color::Red;

			break;

		default:
		{
			if (entity->IsWeapon())
			{
				if (!g_Options.glow_weapons)
					continue;

				//glowObject.m_nGlowStyle = g_Options.glow_others_style;

				color = Color(70, 255, 70, 255);
			}
		}
		}

		glowObject.m_flRed = color.r() / 255.0f;
		glowObject.m_flGreen = color.g() / 255.0f;
		glowObject.m_flBlue = color.b() / 255.0f;
		glowObject.m_flAlpha = color.a() / 255.0f;
		glowObject.m_bRenderWhenOccluded = true;
		glowObject.m_bRenderWhenUnoccluded = false;
	}
}

void Glow::ClearGlow()
{
	for (auto i = 0; i < g_GlowObjManager->m_GlowObjectDefinitions.Count(); i++)
	{
		auto &glowObject = g_GlowObjManager->m_GlowObjectDefinitions[i];
		auto entity = reinterpret_cast<C_BasePlayer*>(glowObject.m_pEntity);

		if (glowObject.IsUnused())
			continue;

		if (!entity || entity->IsDormant())
			continue;

		glowObject.m_flAlpha = 0.0f;
	}
}
