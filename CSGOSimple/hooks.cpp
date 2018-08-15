#include "menu.hpp"
#include "helpers/input.hpp"
#include "helpers/utils.hpp"
#include "features/bhop.hpp"
#include "features/chams.hpp"
#include "features/visuals.hpp"
#include "features/glow.hpp"
#include "RageBot.hpp"
#include "SkinChanger.h"
#include "BulletBeams.h"
#include "Config.h"
#include "Trigger.h"
#include "Hitmarker.h"
#include "BackTrack.h"
#include "menu.hpp"
#include "Legit.hpp"
#include "menu.hpp"
#include "GrenadePred.h"
#include "hooks.hpp"
#include "antiaim.hpp"

#include "imgui\directx9\imgui_impl_dx9.h"
#include "resolver.h"

#include <iosfwd>
#include <cstdio>
#include <ShlObj_core.h>
#include <cstring>
#include <cwchar>
#include <xstddef>
#include <crtdbg.h>
#include <random>

#define PI 3.14159265358979323846
#define min(a,b)            (((a) < (b)) ? (a) : (b))

namespace Global
{
	char my_documents_folder[MAX_PATH];
	QAngle visualAngles = QAngle(0.f, 0.f, 0.f);
	float hitmarkerAlpha;
	CUserCmd *userCMD = nullptr;
	bool bSendPacket = true;
}


namespace Hooks
{
	vfunc_hook hlclient_hook;
	vfunc_hook direct3d_hook;
	vfunc_hook vguipanel_hook;
	vfunc_hook vguisurf_hook;
	vfunc_hook mdlrender_hook;
	vfunc_hook clientmode_hook;
	vfunc_hook render_view;
	vfunc_hook findmdl_hook;
	vfunc_hook gameevents_hook;

	void Initialize()
	{
		hlclient_hook.setup(g_CHLClient, "client.dll");
		direct3d_hook.setup(g_D3DDevice9, "shaderapidx9.dll");
		vguipanel_hook.setup(g_VGuiPanel);
		vguisurf_hook.setup(g_VGuiSurface);
		mdlrender_hook.setup(g_MdlRender, "engine.dll");
		clientmode_hook.setup(g_ClientMode, "client.dll");
		render_view.setup(g_RenderView);
		findmdl_hook.setup(g_MdlCache);
		gameevents_hook.setup(g_GameEvents);

		render_view.hook_index(9, hkSceneEnd);
		direct3d_hook.hook_index(index::EndScene, hkEndScene);
		direct3d_hook.hook_index(index::Reset, hkReset);

		hlclient_hook.hook_index(index::FrameStageNotify, hkFrameStageNotify);
		hlclient_hook.hook_index(index::CreateMove, hkCreateMove_Proxy);
		gameevents_hook.hook_index(index::FireEventClientSide, hkFireEventClientSide);
		vguipanel_hook.hook_index(index::PaintTraverse, hkPaintTraverse);
		findmdl_hook.hook_index(10, HK_FindMDL);

		vguisurf_hook.hook_index(index::PlaySound, hkPlaySound);

		mdlrender_hook.hook_index(index::DrawModelExecute, hkDrawModelExecute);

		clientmode_hook.hook_index(index::DoPostScreenSpaceEffects, hkDoPostScreenEffects);
		clientmode_hook.hook_index(index::OverrideView, hkOverrideView);
		

		Visuals::CreateFonts();

		long res = SHGetFolderPathA(NULL, CSIDL_PERSONAL, NULL, SHGFP_TYPE_CURRENT, Global::my_documents_folder);
		if (res == S_OK)
		{
			std::string config_folder = std::string(Global::my_documents_folder) + "\\Foxyy\\";
			ConfigSys::Get().CreateConfigFolder(config_folder);

			std::string default_file_path = config_folder + "Foxyy.config";

			if (ConfigSys::Get().FileExists(default_file_path))
				ConfigSys::Get().LoadConfig(default_file_path);
		}

		

	}
	//--------------------------------------------------------------------------------
	void Shutdown()
	{
		hlclient_hook.unhook_all();
		direct3d_hook.unhook_all();
		vguipanel_hook.unhook_all();
		vguisurf_hook.unhook_all();
		mdlrender_hook.unhook_all();
		clientmode_hook.unhook_all();


		Visuals::DestroyFonts();
	}


	void __stdcall hkOverrideView(CViewSetup* vsView)
	{
		static auto ofunc = clientmode_hook.get_original<OverrideView>(index::OverrideView);

		if (g_EngineClient->IsInGame() && vsView) {
			CCSGrenadeHint::Get().View();
		}

		ofunc(g_ClientMode, vsView);
	}


	//--------------------------------------------------------------------------------
	long __stdcall hkEndScene(IDirect3DDevice9* device)
	{
		auto oEndScene = direct3d_hook.get_original<EndScene>(index::EndScene);

	    Menu::Get().Render();

		static auto viewmodel_fov = g_CVar->FindVar("viewmodel_fov");
		static auto mat_ambient_light_r = g_CVar->FindVar("mat_ambient_light_r");
		static auto mat_ambient_light_g = g_CVar->FindVar("mat_ambient_light_g");
		static auto mat_ambient_light_b = g_CVar->FindVar("mat_ambient_light_b");

		static auto crosshair = g_CVar->FindVar("cl_crosshair_recoil");
		crosshair->m_nFlags &= FCVAR_CHEAT;

			
			static auto mat_drawgray = g_CVar->FindVar("mat_drawgray");
			mat_drawgray->m_nFlags &= FCVAR_CHEAT;
			if (g_Options.gray_world)
			{
				mat_drawgray->SetValue("1");
			}
			else
			{
				mat_drawgray->SetValue("0");
			}

			static auto minecraft = g_CVar->FindVar("mat_showlowresimage");
			minecraft->m_nFlags &= FCVAR_CHEAT;
			if (g_Options.minecraft_mode)
			{ minecraft->SetValue("1"); }
			else
			{ minecraft->SetValue("0"); }
			//-------------------------------------------------
			static auto mlg = g_CVar->FindVar("mat_showmiplevels");
			mlg->m_nFlags &= FCVAR_CHEAT;
			if (g_Options.mlg_mode)
			{ mlg->SetValue("1"); }
			else
			{ mlg->SetValue("0"); }
			//------------------------------------------------
			static auto lefthand = g_CVar->FindVar("cl_righthand");
			lefthand->m_nFlags &= FCVAR_CHEAT;
			if (g_Options.lefthand)
			{
				lefthand->SetValue("0");
			}
			else
			{
				lefthand->SetValue("1");
			}

	




		static bool NoFlashReset = false;

		if (g_Options.esp_no_flash && !NoFlashReset)
		{
			IMaterial* flash = g_MatSystem->FindMaterial(
				"effects\\flashbang", TEXTURE_GROUP_CLIENT_EFFECTS);

			IMaterial* flashWhite = g_MatSystem->FindMaterial("effects\\flashbang_white",
				TEXTURE_GROUP_CLIENT_EFFECTS);

			if (flash && flashWhite)
			{
				flash->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, true);
				flashWhite->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, true);

				NoFlashReset = true;
			}
		}
		else if (!g_Options.esp_no_flash && NoFlashReset)
		{
			IMaterial* flash = g_MatSystem->FindMaterial(
				"effects\\flashbang", TEXTURE_GROUP_CLIENT_EFFECTS);

			IMaterial* flashWhite = g_MatSystem->FindMaterial("effects\\flashbang_white",
				TEXTURE_GROUP_CLIENT_EFFECTS);

			if (flash && flashWhite)
			{
				flash->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, false);
				flashWhite->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, false);

				NoFlashReset = false;
			}
		}

		static auto xd = g_CVar->FindVar("mat_postprocess_enable");
		xd->m_nFlags &= FCVAR_CHEAT;


		if (g_Options.Epost_process)
		{
			xd->SetValue("1");
		}

		else
		{
			xd->SetValue("0");
		}

		static auto sv_skyname = g_CVar->FindVar("sv_skyname");
		sv_skyname->m_nFlags &= FCVAR_CHEAT;


		
		viewmodel_fov->m_fnChangeCallbacks.m_Size = 0;
		viewmodel_fov->SetValue(g_Options.viewmodel_fov);
		mat_ambient_light_r->SetValue(g_Options.mat_ambient_light_r);
		mat_ambient_light_g->SetValue(g_Options.mat_ambient_light_g);
		mat_ambient_light_b->SetValue(g_Options.mat_ambient_light_b);

		
		
		if (InputSys::Get().IsKeyDown(VK_TAB))
		Utils::RankRevealAll();

		return oEndScene(device);
	}

	/*
	bool PrecacheModel(const char* szModelName)
	{
		INetworkStringTable* m_pModelPrecacheTable = g_ClientStringTableContainer->FindTable("modelprecache");

		if (m_pModelPrecacheTable)
		{
			g_MdlInfo->FindOrLoadModel(szModelName);
			int idx = m_pModelPrecacheTable->AddString(false, szModelName);
			if (idx == INVALID_STRING_INDEX)
				return false;
		}
		return true;
	}
	*/

	//--------------------------------------------------------------------------------
	long __stdcall hkReset(IDirect3DDevice9* device, D3DPRESENT_PARAMETERS* pPresentationParameters)
	{
		auto oReset = direct3d_hook.get_original<Reset>(index::Reset);

		Visuals::DestroyFonts();
		Menu::Get().OnDeviceLost();

		auto hr = oReset(device, pPresentationParameters);

		if (hr >= 0) {
			Menu::Get().OnDeviceReset();
			Visuals::CreateFonts();
		}
		return hr;
	}

	static inline float DegreesToRadians(float Angle)
	{
		return Angle * PI / 180.0f;
	}



	CMoveData bMoveData[0x200];
	void Prediction(CUserCmd* pCmd, C_BasePlayer* LocalPlayer)
	{
		if (g_MoveHelper && LocalPlayer->IsAlive())
		{
			float curtime = g_GlobalVars->curtime;
			float frametime = g_GlobalVars->frametime;
			int iFlags = LocalPlayer->m_fFlags();

			g_GlobalVars->curtime = (float)LocalPlayer->m_nTickBase() * g_GlobalVars->interval_per_tick;
			g_GlobalVars->frametime = g_GlobalVars->interval_per_tick;

			g_MoveHelper->SetHost(LocalPlayer);

			g_Prediction->SetupMove(LocalPlayer, pCmd, nullptr, bMoveData);
			g_GameMovement->ProcessMovement(LocalPlayer, bMoveData);
			g_Prediction->FinishMove(LocalPlayer, pCmd, bMoveData);

			g_MoveHelper->SetHost(0);

			g_GlobalVars->curtime = curtime;
			g_GlobalVars->frametime = frametime;
			LocalPlayer->m_fFlags() = iFlags;
		}
	}


	void MovementFix(CUserCmd* pCmd)
	{

		QAngle ClientViewAngles;
		g_EngineClient->GetViewAngles(ClientViewAngles);

		QAngle CmdAngle = pCmd->viewangles;
		Math::NormalizeAngles(pCmd->viewangles);


		Vector vMove = Vector(pCmd->forwardmove, pCmd->sidemove, 0.0f);
		float flSpeed = vMove.Length();
		Vector qMove = vMove.Angle();
		float flYaw = DegreesToRadians((CmdAngle.yaw - ClientViewAngles.yaw) + qMove.y);
		if (CmdAngle.pitch >= 90.0f || CmdAngle.pitch <= -90.0f) pCmd->forwardmove = -cos(flYaw) * flSpeed;
		else pCmd->forwardmove = cos(flYaw) * flSpeed;
		pCmd->sidemove = sin(flYaw) * flSpeed;

	}


	float RandomFloat(float min, float max)
	{
		float r = (float)rand() / (float)RAND_MAX;
		return min + r * (max - min);
	}


	//--------------------------------------------------------------------------------
	void __stdcall hkCreateMove(int sequence_number, float input_sample_frametime, bool active, bool& bSendPacket)
	{
		int firedShots = g_LocalPlayer->m_iShotsFired();
		QAngle oldPunch;

		auto oCreateMove = hlclient_hook.get_original<CreateMove>(index::CreateMove);

		oCreateMove(g_CHLClient, sequence_number, input_sample_frametime, active);

		auto cmd = g_Input->GetUserCmd(sequence_number);
		auto verified = g_Input->GetVerifiedCmd(sequence_number);

		if (!cmd || !cmd->command_number)
			return;
	
	
		C_BasePlayer *local = g_LocalPlayer;
		C_BaseCombatWeapon* weapon = g_LocalPlayer->m_hActiveWeapon();

		

		legitbot().do_aimbot(g_LocalPlayer, weapon, cmd);
		triggerbot::Triggerbot(cmd);
		CCSGrenadeHint::Get().Tick(cmd->buttons);

		if (g_Options.misc_bhop) {
			Misc::OnCreateMove(cmd);
		}

		if (g_Options.misc_autostrafe)
		{
			Misc::AutoStrafe(cmd, cmd->viewangles);
		}

		if (g_Options.misc_fakewalk && GetAsyncKeyState(g_Options.misc_fakewalk_key))
		{
			static int choked = 0;
			choked = choked > 7 ? 0 : choked + 1;
			cmd->forwardmove = choked < 2 || choked > 5 ? 0 : cmd->forwardmove;
			cmd->sidemove = choked < 2 || choked > 5 ? 0 : cmd->sidemove;
			bSendPacket = choked < 1;
		}

		QAngle qOldView = cmd->viewangles;
		bool bBulletTime = true;
		if (cmd->buttons & IN_ATTACK && bBulletTime)
		{
			cmd->viewangles = qOldView;
		}
		//QAngle aimAngle = Math::CalcAngle(g_LocalPlayer->GetEyePos(), aim_point) - (g_Options.rage_norecoil ? g_LocalPlayer->m_aimPunchAngle() * 2.f : QAngle(0, 0, 0));

		auto pLocal2 = (C_BasePlayer*)g_EntityList->GetClientEntity(g_EngineClient->GetLocalPlayer());

		if (g_Options.aim_enabled)
		{
			if (g_Options.enginepred)
			{
				Prediction(cmd, pLocal2);
				
				//lagfix.Record();
			}

			C_IWAimbot autoaim;
			autoaim.Main(cmd, (C_BasePlayer*)g_EntityList->GetClientEntity(g_EngineClient->GetLocalPlayer())->GetBaseEntity(), bSendPacket);
			
		}

		if (g_Options.Remove_recoil)
		{
			static QAngle oldangle = { 0,0,0 };

			static auto m_weapon_recoil_scale = g_CVar->FindVar("weapon_recoil_scale");
			cmd->viewangles -= g_LocalPlayer->m_aimPunchAngle()* m_weapon_recoil_scale->GetFloat();
		}
	
		TimeWarp::Get().CreateMove(cmd);
		MovementFix(cmd);

		static bool sidePress = false;
		if (InputSys::Get().IsKeyDown(g_Options.tankAntiaimKey) && !sidePress)
		{
			aaSide = !aaSide;
			sidePress = true;
		}
		else if (!InputSys::Get().IsKeyDown(g_Options.tankAntiaimKey) && sidePress)
		{
			sidePress = false;
		}

		if (g_LocalPlayer->IsAlive() && g_EngineClient->IsInGame()) AntiAims::OnCreateMove(cmd);

		if (g_Options.antiuntrusted)
		{
			Math::NormalizeAngles(cmd->viewangles);
			Math::ClampAngles(cmd->viewangles);
		}


		verified->m_cmd = *cmd;
		verified->m_crc = cmd->GetChecksum();
	}


	//--------------------------------------------------------------------------------
	__declspec(naked) void __stdcall hkCreateMove_Proxy(int sequence_number, float input_sample_frametime, bool active)
	{
		__asm
		{
			push ebp
			mov  ebp, esp
			push ebx
			lea  ecx, [esp]
			push ecx
			push dword ptr[active]
			push dword ptr[input_sample_frametime]
			push dword ptr[sequence_number]
			call Hooks::hkCreateMove
			pop  ebx
			pop  ebp
			retn 0Ch
		}
	}

	//--------------------------------------------------------------------------------
	void __stdcall hkPaintTraverse(vgui::VPANEL panel, bool forceRepaint, bool allowForce)
	{
		static auto panelId = vgui::VPANEL{ 0 };
		static auto oPaintTraverse = vguipanel_hook.get_original<PaintTraverse>(index::PaintTraverse);

		oPaintTraverse(g_VGuiPanel, panel, forceRepaint, allowForce);

		if (!panelId) {
			const auto panelName = g_VGuiPanel->GetName(panel);
			if (!strcmp(panelName, "FocusOverlayPanel")) {
				panelId = panel;
			}
		}
		else if (panelId == panel) {

			if (g_EngineClient->IsInGame() && g_LocalPlayer)
			{
				QAngle vecAngles;
				g_EngineClient->GetViewAngles(vecAngles);

				static bool spoofed = false;
				if (!spoofed) {

					ConVar* sv_cheats = g_CVar->FindVar("sv_cheats");
					//    SpoofedConvar* sv_cheats_spoofed = new SpoofedConvar(sv_cheats);
					//    sv_cheats_spoofed->SetInt(1);
					sv_cheats->SetValue(1);
					spoofed = !spoofed;
				}

				if (g_Options.Recoil_crosshair)
					Visuals::RecoilCrosshair();

				if (g_Options.thirdperson_enabled)
				{
					if (GetAsyncKeyState(VK_UP))
					{
						if (!g_Input->m_fCameraInThirdPerson)
						{
							g_Input->m_fCameraInThirdPerson = true;
							g_Input->m_vecCameraOffset = Vector(vecAngles.pitch, vecAngles.yaw, 150.f);
						}
					}
					if (GetAsyncKeyState(VK_DOWN))
					{
						g_Input->m_fCameraInThirdPerson = false;
						g_Input->m_vecCameraOffset = Vector(vecAngles.pitch, vecAngles.yaw, 0);
					}
				}
				else
				{
					g_Input->m_fCameraInThirdPerson = false;
					g_Input->m_vecCameraOffset = Vector(vecAngles.pitch, vecAngles.yaw, 0);
				}
			}

			if (g_EngineClient->IsInGame() && g_EngineClient->IsConnected() && g_LocalPlayer)
			{

				if (g_Options.esp_enabled) {
					for (auto i = 1; i <= g_EntityList->GetHighestEntityIndex(); ++i) {
						auto entity = C_BasePlayer::GetPlayerByIndex(i);

						if (!entity)
							continue;

						int index;

						if (i < 65 && Visuals::ValidPlayer(entity)) {
					
							if (Visuals::Player::Begin(entity)) {

								Visuals::Player::RenderBox((C_BaseEntity*) entity);
								if (g_Options.esp_player_snaplines) Visuals::Player::RenderSnapline();
								if (g_Options.esp_player_names)     Visuals::Player::RenderName();
								if (g_Options.esp_player_health)    Visuals::Player::RenderHealth();
								if (g_Options.esp_player_armour)    Visuals::Player::RenderArmour();
								if (g_Options.esp_skeleton)			Visuals::Player::Skeleton();
								if (g_Options.esp_player_weapons)	Visuals::Player::RenderWeapon();
								if (g_Options.esp_filled_box)		Visuals::Player::RenderFill();
								if (g_Options.enable_dlight)		Visuals::DLight(entity);
								
							
								if (g_Options.esp_player_anglelines) Visuals::AngleLines();

								switch (g_Options.HealthBar_Style)
								{
								case 0:
									break;
								case 1:
									Visuals::Player::RenderHealth();
									break;
								case 2:
									Visuals::EdgyHealthBar();
									break;
								}
				
							


							}
						}
						
						else if (g_Options.esp_dropped_weapons && entity->IsWeapon()) {
							Visuals::Misc::RenderWeapon((C_BaseCombatWeapon*)entity);
						}
						else if (g_Options.esp_dropped_Kits && entity->IsDefuseKit()) {
							Visuals::Misc::RenderDefuseKit(entity);
						}
						else if (entity->IsPlantedC4()) {
							if (g_Options.esp_dropped_c4)
								Visuals::Misc::RenderPlantedC4(entity);
						}
					}
				}
				if (g_Options.esp_crosshair)
					Visuals::Misc::RenderCrosshair();

				if (g_Options.misc_hitmarker)
				{
					HitMarkerEvent::Get().Paint();
				}

				CCSGrenadeHint::Get().Paint();

				//Visuals::NightMode();

			}

			if (g_Options.watermark)
			{
				Visuals::Player::RenderWaterMark();
			}

		}
	}
	//--------------------------------------------------------------------------------
	void __stdcall hkPlaySound(const char* name)
	{
		static auto oPlaySound = vguisurf_hook.get_original<PlaySound>(index::PlaySound);

		if (!g_Options.auto_accept) return;


		oPlaySound(g_VGuiSurface, name);

		// Auto Accept
		if (strstr(name, "UI/competitive_accept_beep.wav")) {
			static auto fnAccept =
				(void(*)())Utils::PatternScan(GetModuleHandleA("client.dll"), "55 8B EC 83 E4 F8 83 EC 08 56 8B 35 ? ? ? ? 57 83 BE");

			fnAccept();

			//This will flash the CSGO window on the taskbar
			//so we know a game was found (you cant hear the beep sometimes cause it auto-accepts too fast)
			FLASHWINFO fi;
			fi.cbSize = sizeof(FLASHWINFO);
			fi.hwnd = InputSys::Get().GetMainWindow();
			fi.dwFlags = FLASHW_ALL | FLASHW_TIMERNOFG;
			fi.uCount = 0;
			fi.dwTimeout = 0;
			FlashWindowEx(&fi);
		}
	}

	void __fastcall hkSceneEnd(void* thisptr, void* edx)
	{
		auto ofunc = render_view.get_original<SceneEndFn>(9);


		ofunc(thisptr);
	}

	//--------------------------------------------------------------------------------
	int __stdcall hkDoPostScreenEffects(int a1)
	{
		auto oDoPostScreenEffects = clientmode_hook.get_original<DoPostScreenEffects>(index::DoPostScreenSpaceEffects);
		BulletBeamsEvent::Get().Paint();

		if (g_Options.glow_enabled)
		{
			Glow::RenderGlow();
		}
		return oDoPostScreenEffects(g_ClientMode, a1);
	}

	bool Die = false;

	void ChangeSkins(C_BasePlayer* local) {
		if (!local || Die)
			return;


		auto weapons = local->m_hMyWeapons();
		for (auto i = 0; weapons[i]; i++) {
			C_BaseAttributableItem* weapon = (C_BaseAttributableItem*)g_EntityList->GetClientEntityFromHandle(weapons[i]);
			if (!weapon)
				continue;

			int definition_index = weapon->m_Item().m_iItemDefinitionIndex();
			if (definition_index == 0)
				continue;


			weapon->m_nFallbackPaintKit() = k_skins[g_Options.skins[definition_index].weapon_skin_id].id;
			//	weapon->m_nFallbackPaintKit() = g_Options.skins[definition_index].weapon_skin_id;
			weapon->m_nFallbackSeed() = g_Options.skins[definition_index].weapon_seed;
			weapon->m_nFallbackStatTrak() = -1;
			weapon->m_flFallbackWear() = g_Options.skins[definition_index].weapon_wear;

			weapon->m_Item().m_iItemIDHigh() = -1;
			weapon->m_Item().m_iAccountID() = local->GetPlayerInfo().xuid_low;

		}

	}

	inline bool ApplyCustomModel(C_BaseAttributableItem* pWeapon, const char* vMdl)
	{
		auto pLocal = (C_BasePlayer*)g_EntityList->GetClientEntity(g_EngineClient->GetLocalPlayer());

		// Get the view model of this weapon.
		C_BaseViewModel* pViewModel = pLocal->m_hViewModel().Get();

		if (!pViewModel)
			return false;

		// Get the weapon belonging to this view model.
		auto hViewModelWeapon = pViewModel->m_hWeapon();
		C_BaseAttributableItem* pViewModelWeapon = (C_BaseAttributableItem*)g_EntityList->GetClientEntityFromHandle(hViewModelWeapon);

		if (pViewModelWeapon != pWeapon)
			return false;

		// Check if an override exists for this view model.
		int nViewModelIndex = pViewModel->m_nModelIndex();

		// Set the replacement model.
		pViewModel->m_nModelIndex() = g_MdlInfo->GetModelIndex(vMdl);

		return true;
	}

	MDLHandle_t __fastcall HK_FindMDL(void* ecx, void* edx, char* FilePath)
	{
		
		/*
		
		if (strstr(FilePath, "knife_default_ct.mdl") || strstr(FilePath, "knife_default_t.mdl"))
		{
			sprintf(FilePath, "models/weapons/Dzucht/crowbar/crowbar.mdl");		
		}
		*/
		/*
		if (strstr(FilePath, "ak47.mdl"))
		{
			sprintf(FilePath, "models/weapons/catgun/v_catgun.mdl");
		}
		*/
		

		return findmdl_hook.get_original<findMdl>(10)(ecx, FilePath);
	}


	//--------------------------------------------------------------------------------
	void __stdcall hkFrameStageNotify(ClientFrameStage_t stage)
	{
		static auto ofunc = hlclient_hook.get_original<FrameStageNotify>(index::FrameStageNotify);
		ofunc(g_CHLClient, stage);

		C_BasePlayer* me = C_BasePlayer::GetPlayerByIndex(g_EngineClient->GetLocalPlayer());
		auto pLocal = (C_BasePlayer*)g_EntityList->GetClientEntity(g_EngineClient->GetLocalPlayer());

		QAngle aim_punch_old;
		QAngle view_punch_old;
	
		QAngle *aim_punch = nullptr;
		QAngle *view_punch = nullptr;

		Resolver::OnFrameStageNotify(stage);

		
		switch (stage)
		{
		case FRAME_UNDEFINED:
			break;
		case FRAME_START:
			
			break;
		case FRAME_NET_UPDATE_START:
			break;
		case FRAME_NET_UPDATE_POSTDATAUPDATE_START:
			ChangeSkins(me);
			//Skinchanger::Get().Run();
			break;
		case FRAME_NET_UPDATE_POSTDATAUPDATE_END:
			break;
		case FRAME_NET_UPDATE_END:
			break;
		case FRAME_RENDER_START:

			if (g_Options.esp_no_smoke)
				*(int*)Offsets::smokeCount = 0;

			break;
		case FRAME_RENDER_END:
			break;
		}
		

	if (g_EngineClient->IsInGame() && g_EngineClient->IsConnected() && stage == FRAME_NET_UPDATE_POSTDATAUPDATE_START)
	{

	auto hWeapons = pLocal->m_hMyWeapons();
	// skin changer
	if (hWeapons)
	{
		static char* KnifeCT = "models/weapons/v_knife_ct.mdl";
		static char* KnifeT = "models/weapons/v_knife_t.mdl";
		static char* Bayonet = "models/weapons/v_knife_bayonet.mdl";
		static char* Butterfly = "models/weapons/v_knife_butterfly.mdl";
		static char* Flip = "models/weapons/v_knife_flip.mdl";
		static char* Gut = "models/weapons/v_knife_gut.mdl";
		static char* Karambit = "models/weapons/v_knife_karam.mdl";
		static char* M9Bayonet = "models/weapons/v_knife_m9_bay.mdl";
		static char* Huntsman = "models/weapons/v_knife_tactical.mdl";
		static char* Falchion = "models/weapons/v_knife_falchion_advanced.mdl";
		static char* Dagger = "models/weapons/v_knife_push.mdl";
		static char* Bowie = "models/weapons/v_knife_survival_bowie.mdl";

		// go through all weapons
		for (int i = 0; hWeapons[i]; i++)
		{
			// Get the weapon entity from the handle.
			auto pWeapon = (C_BaseAttributableItem*)g_EntityList->GetClientEntityFromHandle(hWeapons[i]);
			int pWeaponType = ((C_BaseCombatWeapon*)g_EntityList->GetClientEntityFromHandle(hWeapons[i]))->GetCSWeaponData()->iWeaponType;

			if (!pWeapon)
				continue;

			// Knife changer if holding a knife
			if (pWeaponType == WEAPONTYPE_KNIFE)
			{
				// model indexes
				switch (g_Options.knifemodel)
				{
				case KNIFE_DEFAULT:
					if (pLocal->m_iTeamNum() == 0) pWeapon->m_nModelIndex() = g_MdlInfo->GetModelIndex(KnifeT);
					if (pLocal->m_iTeamNum() == 1) pWeapon->m_nModelIndex() = g_MdlInfo->GetModelIndex(KnifeCT);
					break;
				case KNIFE_BAYONET:
					pWeapon->m_nModelIndex() = g_MdlInfo->GetModelIndex(Bayonet);
					break;
				case KNIFE_FLIP:
					pWeapon->m_nModelIndex() = g_MdlInfo->GetModelIndex(Flip);
					break;
				case KNIFE_GUT:
					pWeapon->m_nModelIndex() = g_MdlInfo->GetModelIndex(Gut);
					break;
				case KNIFE_KARAMBIT:
					pWeapon->m_nModelIndex() = g_MdlInfo->GetModelIndex(Karambit);
					break;
				case KNIFE_M9BAYONET:
					pWeapon->m_nModelIndex() = g_MdlInfo->GetModelIndex(M9Bayonet);
					break;
				case KNIFE_HUNTSMAN:
					pWeapon->m_nModelIndex() = g_MdlInfo->GetModelIndex(Huntsman);
					break;
				case KNIFE_FALCHION:
					pWeapon->m_nModelIndex() = g_MdlInfo->GetModelIndex(Falchion);
					break;
				case KNIFE_BOWIE:
					pWeapon->m_nModelIndex() = g_MdlInfo->GetModelIndex(Bowie);
					break;
				case KNIFE_BUTTERFLY:
					pWeapon->m_nModelIndex() = g_MdlInfo->GetModelIndex(Butterfly);
					break;
				case KNIFE_PUSHDAGGER:
					pWeapon->m_nModelIndex() = g_MdlInfo->GetModelIndex(Dagger);
					break;
				}

				// changine knife model
				switch (g_Options.knifemodel)
				{
				case KNIFE_DEFAULT:
					if (pLocal->m_iTeamNum() == 0) ApplyCustomModel(pWeapon, KnifeT);
					if (pLocal->m_iTeamNum() == 1) ApplyCustomModel(pWeapon, KnifeCT);
					break;
				case KNIFE_BAYONET:
					ApplyCustomModel(pWeapon, Bayonet);
					break;
				case KNIFE_FLIP:
					ApplyCustomModel(pWeapon, Flip);
					break;
				case KNIFE_GUT:
					ApplyCustomModel(pWeapon, Gut);
					break;
				case KNIFE_KARAMBIT:
					ApplyCustomModel(pWeapon, Karambit);
					break;
				case KNIFE_M9BAYONET:
					ApplyCustomModel(pWeapon, M9Bayonet);
					break;
				case KNIFE_HUNTSMAN:
					ApplyCustomModel(pWeapon, Huntsman);
					break;
				case KNIFE_FALCHION:
					ApplyCustomModel(pWeapon, Falchion);
					break;
				case KNIFE_BOWIE:
					ApplyCustomModel(pWeapon, Bowie);
					break;
				case KNIFE_BUTTERFLY:
					ApplyCustomModel(pWeapon, Butterfly);
					break;
				case KNIFE_PUSHDAGGER:
					ApplyCustomModel(pWeapon, Dagger);
					break;
				}

			}
		}
	}



	}
	}

	bool __fastcall hkFireEventClientSide(IGameEventManager2* thisptr, void* edx, IGameEvent* pEvent)
	{
		auto oFunc = gameevents_hook.get_original<FireEventClientSide>(index::FireEventClientSide);

		// No events? just call the original.
		if (!pEvent)
			return oFunc(thisptr, pEvent);

		Resolver::FireGameEvent(pEvent);


		return oFunc(thisptr, pEvent);
	}


	//--------------------------------------------------------------------------------
	void __stdcall hkDrawModelExecute(IMatRenderContext* ctx, const DrawModelState_t& state, const ModelRenderInfo_t& pInfo, matrix3x4_t* pCustomBoneToWorld)
	{
		static auto ofunc = mdlrender_hook.get_original<DrawModelExecute>(index::DrawModelExecute);

		Chams::Get().OnDrawModelExecute(ctx, state, pInfo, pCustomBoneToWorld);
		
		ofunc(g_MdlRender, ctx, state, pInfo, pCustomBoneToWorld);

		g_MdlRender->ForcedMaterialOverride(nullptr);
	}
	}


		
	
