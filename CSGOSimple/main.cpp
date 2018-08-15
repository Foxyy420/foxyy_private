#define NOMINMAX
#include <Windows.h>

#include "valve_sdk/sdk.hpp"
#include "helpers/utils.hpp"
#include "helpers/input.hpp"

#include "hooks.hpp"
#include "menu.hpp"
#include "options.hpp"
#include "features\glow.hpp"
#include "Hitmarker.h"
#include "menu.hpp"
#include "SkinChanger.h"
#include "BulletBeams.h"


#define SEQUENCE_DEFAULT_DRAW 0
#define SEQUENCE_DEFAULT_IDLE1 1
#define SEQUENCE_DEFAULT_IDLE2 2
#define SEQUENCE_DEFAULT_LIGHT_MISS1 3
#define SEQUENCE_DEFAULT_LIGHT_MISS2 4
#define SEQUENCE_DEFAULT_HEAVY_MISS1 9
#define SEQUENCE_DEFAULT_HEAVY_HIT1 10
#define SEQUENCE_DEFAULT_HEAVY_BACKSTAB 11
#define SEQUENCE_DEFAULT_LOOKAT01 12

#define SEQUENCE_BUTTERFLY_DRAW 0
#define SEQUENCE_BUTTERFLY_DRAW2 1
#define SEQUENCE_BUTTERFLY_LOOKAT01 13
#define SEQUENCE_BUTTERFLY_LOOKAT03 15

#define SEQUENCE_FALCHION_IDLE1 1
#define SEQUENCE_FALCHION_HEAVY_MISS1 8
#define SEQUENCE_FALCHION_HEAVY_MISS1_NOFLIP 9
#define SEQUENCE_FALCHION_LOOKAT01 12
#define SEQUENCE_FALCHION_LOOKAT02 13

#define SEQUENCE_DAGGERS_IDLE1 1
#define SEQUENCE_DAGGERS_LIGHT_MISS1 2
#define SEQUENCE_DAGGERS_LIGHT_MISS5 6
#define SEQUENCE_DAGGERS_HEAVY_MISS2 11
#define SEQUENCE_DAGGERS_HEAVY_MISS1 12

#define SEQUENCE_BOWIE_IDLE1 1
#define RandomInt(nMin, nMax) (rand() % (nMax - nMin + 1) + nMin);


typedef void(*RecvVarProxyFn)(const CRecvProxyData* pData, void* pStruct, void* pOut);
RecvVarProxyFn fnSequenceProxyFn = nullptr;
RecvVarProxyFn oRecvnModelIndex;

void Hooked_RecvProxy_Viewmodel(CRecvProxyData *pData, void *pStruct, void *pOut)
{
	static int default_t = g_MdlInfo->GetModelIndex("models/weapons/v_knife_default_t.mdl");
	static int default_ct = g_MdlInfo->GetModelIndex("models/weapons/v_knife_default_ct.mdl");
	static int iBayonet = g_MdlInfo->GetModelIndex("models/weapons/v_knife_bayonet.mdl");
	static int iButterfly = g_MdlInfo->GetModelIndex("models/weapons/v_knife_butterfly.mdl");
	static int iFlip = g_MdlInfo->GetModelIndex("models/weapons/v_knife_flip.mdl");
	static int iGut = g_MdlInfo->GetModelIndex("models/weapons/v_knife_gut.mdl");
	static int iKarambit = g_MdlInfo->GetModelIndex("models/weapons/v_knife_karam.mdl");
	static int iM9Bayonet = g_MdlInfo->GetModelIndex("models/weapons/v_knife_m9_bay.mdl");
	static int iHuntsman = g_MdlInfo->GetModelIndex("models/weapons/v_knife_tactical.mdl");
	static int iFalchion = g_MdlInfo->GetModelIndex("models/weapons/v_knife_falchion_advanced.mdl");
	static int iDagger = g_MdlInfo->GetModelIndex("models/weapons/v_knife_push.mdl");
	static int iBowie = g_MdlInfo->GetModelIndex("models/weapons/v_knife_survival_bowie.mdl");
	static int iGunGame = g_MdlInfo->GetModelIndex("models/weapons/v_knife_gg.mdl");

	if (g_LocalPlayer)
	{
		if (g_LocalPlayer->IsAlive() && (pData->m_Value.m_Int == default_ct
			|| pData->m_Value.m_Int == default_t
			|| pData->m_Value.m_Int == iGut
			|| pData->m_Value.m_Int == iKarambit
			|| pData->m_Value.m_Int == iM9Bayonet
			|| pData->m_Value.m_Int == iHuntsman
			|| pData->m_Value.m_Int == iFalchion
			|| pData->m_Value.m_Int == iDagger
			|| pData->m_Value.m_Int == iBowie
			|| pData->m_Value.m_Int == iButterfly
			|| pData->m_Value.m_Int == iFlip
			|| pData->m_Value.m_Int == iBayonet))
		{
			if (g_Options.knifemodel == 1)
				pData->m_Value.m_Int = iBayonet;
			else if (g_Options.knifemodel == 2)
				pData->m_Value.m_Int = iFlip;
			else if (g_Options.knifemodel == 3)
				pData->m_Value.m_Int = iGut;
			else if (g_Options.knifemodel == 4)
				pData->m_Value.m_Int = iKarambit;
			else if (g_Options.knifemodel == 5)
				pData->m_Value.m_Int = iM9Bayonet;
			else if (g_Options.knifemodel == 6)
				pData->m_Value.m_Int = iHuntsman;
			else if (g_Options.knifemodel == 7)
				pData->m_Value.m_Int = iFalchion;
			else if (g_Options.knifemodel == 8)
				pData->m_Value.m_Int = iBowie;
			else if (g_Options.knifemodel == 9)
				pData->m_Value.m_Int = iButterfly;
			else if (g_Options.knifemodel == 10)
				pData->m_Value.m_Int = iDagger;


		}
	}


	oRecvnModelIndex(pData, pStruct, pOut);
}


void SetViewModelSequence2(const CRecvProxyData *pDataConst, void *pStruct, void *pOut)
{
	CRecvProxyData* pData = const_cast<CRecvProxyData*>(pDataConst);

	// Confirm that we are replacing our view model and not someone elses.
	C_BaseViewModel* pViewModel = (C_BaseViewModel*)pStruct;

	if (pViewModel) {
		IClientEntity* pOwner = g_EntityList->GetClientEntityFromHandle(pViewModel->m_hOwner());

		// Compare the owner entity of this view model to the local player entity.
		if (pOwner && pOwner->EntIndex() == g_EngineClient->GetLocalPlayer()) {
			// Get the filename of the current view model.
			const model_t* pModel = g_MdlInfo->GetModel(pViewModel->m_nModelIndex());
			const char* szModel = g_MdlInfo->GetModelName(pModel);

			// Store the current sequence.
			int m_nSequence = pData->m_Value.m_Int;

			if (!strcmp(szModel, "models/weapons/v_knife_butterfly.mdl")) {
				// Fix animations for the Butterfly Knife.
				switch (m_nSequence) {
				case SEQUENCE_DEFAULT_DRAW:
					m_nSequence = RandomInt(SEQUENCE_BUTTERFLY_DRAW, SEQUENCE_BUTTERFLY_DRAW2); break;
				case SEQUENCE_DEFAULT_LOOKAT01:
					m_nSequence = RandomInt(SEQUENCE_BUTTERFLY_LOOKAT01, SEQUENCE_BUTTERFLY_LOOKAT03); break;
				default:
					m_nSequence++;
				}
			}
			else if (!strcmp(szModel, "models/weapons/v_knife_falchion_advanced.mdl")) {
				// Fix animations for the Falchion Knife.
				switch (m_nSequence) {
				case SEQUENCE_DEFAULT_IDLE2:
					m_nSequence = SEQUENCE_FALCHION_IDLE1; break;
				case SEQUENCE_DEFAULT_HEAVY_MISS1:
					m_nSequence = RandomInt(SEQUENCE_FALCHION_HEAVY_MISS1, SEQUENCE_FALCHION_HEAVY_MISS1_NOFLIP); break;
				case SEQUENCE_DEFAULT_LOOKAT01:
					m_nSequence = RandomInt(SEQUENCE_FALCHION_LOOKAT01, SEQUENCE_FALCHION_LOOKAT02); break;
				case SEQUENCE_DEFAULT_DRAW:
				case SEQUENCE_DEFAULT_IDLE1:
					break;
				default:
					m_nSequence--;
				}
			}
			else if (!strcmp(szModel, "models/weapons/v_knife_push.mdl")) {
				// Fix animations for the Shadow Daggers.
				switch (m_nSequence) {
				case SEQUENCE_DEFAULT_IDLE2:
					m_nSequence = SEQUENCE_DAGGERS_IDLE1; break;
				case SEQUENCE_DEFAULT_LIGHT_MISS1:
				case SEQUENCE_DEFAULT_LIGHT_MISS2:
					m_nSequence = RandomInt(SEQUENCE_DAGGERS_LIGHT_MISS1, SEQUENCE_DAGGERS_LIGHT_MISS5); break;
				case SEQUENCE_DEFAULT_HEAVY_MISS1:
					m_nSequence = RandomInt(SEQUENCE_DAGGERS_HEAVY_MISS2, SEQUENCE_DAGGERS_HEAVY_MISS1); break;
				case SEQUENCE_DEFAULT_HEAVY_HIT1:
				case SEQUENCE_DEFAULT_HEAVY_BACKSTAB:
				case SEQUENCE_DEFAULT_LOOKAT01:
					m_nSequence += 3; break;
				case SEQUENCE_DEFAULT_DRAW:
				case SEQUENCE_DEFAULT_IDLE1:
					break;
				default:
					m_nSequence += 2;
				}
			}
			else if (!strcmp(szModel, "models/weapons/v_knife_survival_bowie.mdl")) {
				// Fix animations for the Bowie Knife.
				switch (m_nSequence) {
				case SEQUENCE_DEFAULT_DRAW:
				case SEQUENCE_DEFAULT_IDLE1:
					break;
				case SEQUENCE_DEFAULT_IDLE2:
					m_nSequence = SEQUENCE_BOWIE_IDLE1; break;
				default:
					m_nSequence--;
				}
			}

			// Set the fixed sequence.
			pData->m_Value.m_Int = m_nSequence;
		}
	}

	// Call original function with the modified data.
	fnSequenceProxyFn(pData, pStruct, pOut);

}

void AnimationFixHook()
{
	for (ClientClass* pClass = g_CHLClient->GetAllClasses(); pClass; pClass = pClass->m_pNext) {
		if (!strcmp(pClass->m_pNetworkName, "CBaseViewModel")) {
			// Search for the 'm_nModelIndex' property.
			RecvTable* pClassTable = pClass->m_pRecvTable;

			for (int nIndex = 0; nIndex < pClassTable->m_nProps; nIndex++) {
				RecvProp* pProp = &pClassTable->m_pProps[nIndex];

				if (!pProp || strcmp(pProp->m_pVarName, "m_nSequence"))
					continue;

				// Store the original proxy function.
				fnSequenceProxyFn = static_cast<RecvVarProxyFn>(pProp->m_ProxyFn);

				// Replace the proxy function with our sequence changer.
				pProp->m_ProxyFn = static_cast<RecvVarProxyFn>(SetViewModelSequence2);

				break;
			}

			break;
		}
	}
}

void AnimationFixUnhook()
{
	for (ClientClass* pClass = g_CHLClient->GetAllClasses(); pClass; pClass = pClass->m_pNext) {
		if (!strcmp(pClass->m_pNetworkName, "CBaseViewModel")) {
			// Search for the 'm_nModelIndex' property.
			RecvTable* pClassTable = pClass->m_pRecvTable;

			for (int nIndex = 0; nIndex < pClassTable->m_nProps; nIndex++) {
				RecvProp* pProp = &pClassTable->m_pProps[nIndex];

				if (!pProp || strcmp(pProp->m_pVarName, "m_nSequence"))
					continue;

				// Replace the proxy function with our sequence changer.
				pProp->m_ProxyFn = fnSequenceProxyFn;

				break;
			}

			break;
		}
	}
}


DWORD WINAPI OnDllAttach(LPVOID base)
{
	// 
	// Wait at most 10s for the main game modules to be loaded.
	// 
	if (Utils::WaitForModules(10000, { L"client.dll", L"engine.dll", L"shaderapidx9.dll" }) == WAIT_TIMEOUT) {
		// One or more modules were not loaded in time
		return FALSE;
	}


	Utils::AttachConsole();
	SetConsoleTitle(L"Cool cheat");
	system("COLOR 0A");


	try {
		Utils::ConsolePrint("Initializing...\n");

		Interfaces::Initialize();

		HitMarkerEvent::Get().RegisterSelf();
		BulletImpactEvent::Get().RegisterSelf();
		BulletBeamsEvent::Get().RegisterSelf();
		NetvarSys::Get().Initialize();
		InputSys::Get().Initialize();
		Menu::Get().Initialize();
		AnimationFixHook();

		Hooks::Initialize();

		g_ClientState->ForceFullUpdate();
		Skinchanger::Get().Dump();
		initialize();

	



		// Register some hotkeys.
		// - Note:  The function that is called when the hotkey is pressed
		//          is called from the WndProc thread, not this thread.
		// 

		// Panic button
		InputSys::Get().RegisterHotkey(VK_DELETE, []() {
			g_Unload = true;
		});

		// Menu Toggle
		InputSys::Get().RegisterHotkey(VK_INSERT, []() {
			Menu::Get().Toggle();
		});





		Utils::ConsolePrint("Finished.\n");
		Utils::ConsolePrint("Built on: %s %s\n", __DATE__, __TIME__);

		while (!g_Unload)
			Sleep(1000);

		g_CVar->FindVar("crosshair")->SetValue(true);

		FreeLibraryAndExitThread(static_cast<HMODULE>(base), 1);

	}
	catch (const std::exception& ex) {
		Utils::ConsolePrint("An error occured during initialization:\n");
		Utils::ConsolePrint("%s\n", ex.what());
		Utils::ConsolePrint("Press any key to exit.\n");
		Utils::ConsoleReadKey();
		Utils::DetachConsole();

		FreeLibraryAndExitThread(static_cast<HMODULE>(base), 1);
	}

	// unreachable
	//return TRUE;
}

BOOL WINAPI OnDllDetach()
{

	Utils::DetachConsole();

	HitMarkerEvent::Get().UnregisterSelf();
	BulletImpactEvent::Get().UnregisterSelf();
	BulletBeamsEvent::Get().UnregisterSelf();
	Hooks::Shutdown();
	AnimationFixUnhook();
	
	Menu::Get().Shutdown();
	return TRUE;
}

BOOL WINAPI DllMain(
	_In_      HINSTANCE hinstDll,
	_In_      DWORD     fdwReason,
	_In_opt_  LPVOID    lpvReserved
)
{
	switch (fdwReason) {
	case DLL_PROCESS_ATTACH:
		DisableThreadLibraryCalls(hinstDll);
		CreateThread(nullptr, 0, OnDllAttach, hinstDll, 0, nullptr);
		return TRUE;
	case DLL_PROCESS_DETACH:
		if (lpvReserved == nullptr)
			return OnDllDetach();
		return TRUE;
	default:
		return TRUE;
	}
}
