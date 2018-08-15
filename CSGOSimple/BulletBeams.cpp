
#include "MainInclude.hpp"
#include "BulletBeams.h"


void BulletBeamsEvent::FireGameEvent(IGameEvent *event)
{
	if (!g_LocalPlayer || !event)
		return;

	if (g_Options.esp_bullet_beams)
	{
		if (g_EngineClient->GetPlayerForUserID(event->GetInt("userid")) == g_EngineClient->GetLocalPlayer() && g_LocalPlayer && g_LocalPlayer->IsAlive())
		{
			float x = event->GetFloat("x"), y = event->GetFloat("y"), z = event->GetFloat("z");
			bulletImpactInfo.push_back({ g_GlobalVars->curtime, Vector(x, y, z) });
		}
	}

}

int BulletBeamsEvent::GetEventDebugID(void)
{
	return EVENT_DEBUG_ID_INIT;
}

void BulletBeamsEvent::RegisterSelf()
{
	g_GameEvents->AddListener(this, "bullet_impact", false);
}

void BulletBeamsEvent::UnregisterSelf()
{
	g_GameEvents->RemoveListener(this);
}

void BulletBeamsEvent::Paint(void)
{
	if (!g_Options.esp_bullet_beams)
		return;

	if (!g_EngineClient->IsInGame() || !g_LocalPlayer || !g_LocalPlayer->IsAlive())
	{
		bulletImpactInfo.clear();
		return;
	}

	std::vector<BulletImpactInfo> &impacts = bulletImpactInfo;

	if (impacts.empty())
		return;

	Color current_color(g_Options.color_esp_bullet_beams);

	for (size_t i = 0; i < impacts.size(); i++)
	{
		auto current_impact = impacts.at(i);

		BeamInfo_t beamInfo;

		beamInfo.m_nType = TE_BEAMPOINTS;
		beamInfo.m_pszModelName = "sprites/physbeam.vmt";
		beamInfo.m_nModelIndex = -1;
		beamInfo.m_flHaloScale = 0.0f;
		beamInfo.m_flLife = g_Options.esp_bulletbeams_length;
		beamInfo.m_flWidth = 2.f;
		beamInfo.m_flEndWidth = 2.f;
		beamInfo.m_flFadeLength = 0.5f;
		beamInfo.m_flAmplitude = 2.0f;
		beamInfo.m_flBrightness = 255.f;
		beamInfo.m_flSpeed = 0.2f;
		beamInfo.m_nStartFrame = 0;
		beamInfo.m_flFrameRate = 0.f;
		beamInfo.m_flRed = current_color.r();
		beamInfo.m_flGreen = current_color.g();
		beamInfo.m_flBlue = current_color.b();
		beamInfo.m_nSegments = 2;
		beamInfo.m_bRenderable = true;
		beamInfo.m_nFlags = FBEAM_ONLYNOISEONCE | FBEAM_NOTILE | FBEAM_HALOBEAM;

		beamInfo.m_vecStart = g_LocalPlayer->GetEyePos();
		beamInfo.m_vecEnd = current_impact.m_vecHitPos;

		auto beam = g_RenderBeams->CreateBeamPoints(beamInfo);
		if (beam)
			g_RenderBeams->DrawBeam(beam);

		//g_DebugOverlay->AddBoxOverlay(current_impact.m_vecHitPos, Vector(-3, -3, -3), Vector(3, 3, 3), QAngle(0, 0, 0), current_color.r(), current_color.g(), current_color.b(), current_color.a(), 0.8f);

		impacts.erase(impacts.begin() + i);
	}
}

