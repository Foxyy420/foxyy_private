#include "Skinchanger.h"

#include "KitParser.h"
#include "helpers\utils.hpp"

#include <sstream>


#include <cstdio> 
#include <cmath> 
#include <cstring> 
#include <string> 
#include <fstream> 
#include <cstdlib> 
#include "options.hpp"

#include <cereal/archives/json.hpp>
#include "ItemDefinitions.h"

static CreateClientClassFn GetWearableCreateFn()
{
	auto clazz = g_CHLClient->GetAllClasses();

	while (strcmp(clazz->m_pNetworkName, "CEconWearable"))
		clazz = clazz->m_pNext;

	return clazz->m_pCreateFn;
}

void Skinchanger::Run()
{
	// Skin config
	static auto ConfigCreated = false;
	if (!ConfigCreated)
	{
		InitSkins();
		ConfigCreated = true;
	}

	// Only run in-game
	if (!g_EngineClient->IsInGame()) return;

	// g_LocalPlayer is nulled during this FSN stage, so have to do this.
	auto* pLocal = static_cast<C_BasePlayer*>(g_EntityList->GetClientEntity(g_EngineClient->GetLocalPlayer()));
	if (!pLocal) return;

	// Populate player info struct for XUID ids
	player_info_t player_info;
	if (!g_EngineClient->GetPlayerInfo(pLocal->EntIndex(), &player_info)) return;

	// Glove stuff
	{
		auto wearables = pLocal->m_hMyWearables();

		static CBaseHandle glove_handle = 0;

		auto glove = reinterpret_cast<C_BaseAttributableItem*>(g_EntityList->GetClientEntityFromHandle(wearables[0]));

		if (!glove)
		{
			auto our_glove = reinterpret_cast<C_BaseAttributableItem*>(g_EntityList->GetClientEntityFromHandle(glove_handle));

			if (our_glove)
			{
				wearables[0] = glove_handle;
				glove = our_glove;
			}
		}

		if (!pLocal->IsAlive())
		{
			auto glove = static_cast<C_BaseAttributableItem*>(g_EntityList->GetClientEntityFromHandle(*pLocal->m_hMyWearables()));
			if (!glove)
				return;

			glove->GetClientNetworkable()->SetDestroyedOnRecreateEntities();
			glove->GetClientNetworkable()->Release();

			return;
		}

		if (!glove)
		{
			static auto create_wearable_fn = GetWearableCreateFn();

			auto entry = g_EntityList->GetHighestEntityIndex() + 1;
			auto serial = rand() % 0x1000;

			//glove = static_cast<C_BaseAttributableItem*>(create_wearable_fn(entry, serial));
			create_wearable_fn(entry, serial);
			glove = reinterpret_cast<C_BaseAttributableItem*>(g_EntityList->GetClientEntity(entry));

			{
				static auto set_abs_origin_fn = reinterpret_cast<void(__thiscall*)(void*, const Vector&)>
					(Utils::PatternScan(GetModuleHandle(L"client.dll"), "55 8B EC 83 E4 F8 51 53 56 57 8B F1"));

				static const Vector new_pos = { 10000.f, 10000.f, 10000.f };

				set_abs_origin_fn(glove, new_pos);
			}

			wearables[0] = entry | serial << 16;

			glove_handle = wearables[0];
		}

		*reinterpret_cast<int*>(uintptr_t(glove) + 0x64) = -1;
		ApplyGlove(glove);
	}

	const auto weapons = pLocal->m_hMyWeapons();
	for (size_t i = 0; weapons[i].IsValid(); i++)
	{
		auto pWeapon = static_cast<C_BaseAttributableItem*>(g_EntityList->GetClientEntityFromHandle(weapons[i]));
		if (!pWeapon) continue;

		int nWeaponIndex = pWeapon->m_nModelIndex();
		const auto nOriginalKnifeCT = g_MdlInfo->GetModelIndex("models/weapons/v_knife_default_ct.mdl");
		const auto nOriginalKnifeT = g_MdlInfo->GetModelIndex("models/weapons/v_knife_default_t.mdl");
		if (nWeaponIndex == nOriginalKnifeCT || nWeaponIndex == nOriginalKnifeT)
			pWeapon->m_nModelIndex() = g_MdlInfo->GetModelIndex(ItemDefinitions::WeaponInfo.at(KnifeCfg.iItemDefinitionIndex).model);

		//ApplyModel(pLocal, pWeapon);
		ApplySkin(pWeapon, player_info);
		//ApplyStickerHooks(pWeapon);
	}
}

void Skinchanger::Dump()
{
	try
	{
		if (CreateDirectoryA("C:\\black_panties\\dump", NULL) ||
			ERROR_ALREADY_EXISTS == GetLastError())
		{
			KitParser::Get().Dump();
		}
		else
		{
			//g_Warning("Couldn't create folder\n");
			Utils::ConsolePrint("Couldn't create folder");
		}
	}
	catch (const std::exception& ex)
	{
		//g_Warning(ex.what());
		//g_Msg("\n");
		Utils::ConsolePrint(ex.what());
	}
}

void Skinchanger::LoadSkins()
{
	InitSkins();

	try
	{
		if (CreateDirectoryA("C:\\black_panties", NULL) ||
			ERROR_ALREADY_EXISTS == GetLastError())
		{
			std::ifstream is("C:\\black_panties\\skins.json");
			if (is.good())
			{
				cereal::JSONInputArchive iarchive(is);

				iarchive(cereal::make_nvp("weapons", WeaponCfg),
					cereal::make_nvp("knife", KnifeCfg),
					cereal::make_nvp("gloves", GloveCfg));
			}
			else
			{
				//g_Warning("skins.json not found, creating...\n");

				std::ofstream os("C:\\black_panties\\skins.json");
				cereal::JSONOutputArchive oarchive(os, cereal::JSONOutputArchive::Options::Options(3));

				oarchive(cereal::make_nvp("weapons", WeaponCfg),
					cereal::make_nvp("knife", KnifeCfg),
					cereal::make_nvp("gloves", GloveCfg));
			}
		}
		else
		{
			//g_Warning("Couldn't create folder\n");
			Utils::ConsolePrint("Couldn't create folder");
		}
	}
	catch (const std::exception& ex)
	{
		//g_Warning(ex.what());
		//g_Msg("\n");
		Utils::ConsolePrint(ex.what());
	}

	g_ClientState->ForceFullUpdate();
}

const char* Skinchanger::GetIconOverride(const std::string & original)
{
	if (original == "knife_default_ct" || original == "knife_t") return ItemDefinitions::WeaponInfo.at(KnifeCfg.iItemDefinitionIndex).icon;
	return nullptr;
}

void Skinchanger::ApplySkin(C_BaseAttributableItem* pWeapon, player_info_t pInfo)
{
	auto& nWeaponIndex = pWeapon->m_Item().m_iItemDefinitionIndex();
	if (nWeaponIndex == WEAPON_KNIFE || nWeaponIndex == WEAPON_KNIFE_T || nWeaponIndex == KnifeCfg.iItemDefinitionIndex)
	{
		pWeapon->m_Item().m_iItemDefinitionIndex() = KnifeCfg.iItemDefinitionIndex;
		pWeapon->m_nFallbackPaintKit() = KnifeCfg.nFallbackPaintKit;
		pWeapon->m_Item().m_iEntityQuality() = KnifeCfg.iEntityQuality;
		pWeapon->m_nFallbackSeed() = KnifeCfg.nFallbackSeed;
		pWeapon->m_nFallbackStatTrak() = KnifeCfg.nFallbackStatTrak;
		pWeapon->m_flFallbackWear() = std::max(0.01f, KnifeCfg.flFallbackWear);
		//trcpy(&pWeapon->m_Item().m_szCustomName(), KnifeCfg.szCustomName.c_str());

		pWeapon->m_Item().m_iAccountID() = pInfo.xuid_low;

		pWeapon->m_Item().m_iItemIDHigh() = -1;
	}
	else if (WeaponCfg.find(nWeaponIndex) == WeaponCfg.end()) return;
	else
	{
		const auto cfg = WeaponCfg[nWeaponIndex];

		pWeapon->m_nFallbackPaintKit() = cfg.nFallbackPaintKit;
		pWeapon->m_Item().m_iEntityQuality() = cfg.iEntityQuality;
		pWeapon->m_nFallbackSeed() = cfg.nFallbackSeed;
		pWeapon->m_nFallbackStatTrak() = cfg.nFallbackStatTrak;
		pWeapon->m_flFallbackWear() = std::max(0.01f, cfg.flFallbackWear);
		//strcpy(&pWeapon->m_Item().m_szCustomName(), cfg.szCustomName.c_str());

		pWeapon->m_Item().m_iAccountID() = pInfo.xuid_low;

		pWeapon->m_Item().m_iItemIDHigh() = -1;
	}
}

void Skinchanger::ApplyModel(C_BasePlayer* pLocal, C_BaseAttributableItem* pWeapon)
{
	/*
	auto pViewModel = pLocal->m_hViewModel();
	if (!pViewModel) return;

	auto hViewModelWeapon = pViewModel->m_hWeapon();
	auto pViewModelWeapon = static_cast<C_BaseAttributableItem*>(g_EntityList->GetClientEntityFromHandle(hViewModelWeapon));

	if (pViewModelWeapon != pWeapon) return;

	int nViewModelIndex = pViewModel->m_nModelIndex();

	const auto nOriginalKnifeCT = g_MdlInfo->GetModelIndex("models/weapons/v_knife_default_ct.mdl");
	const auto nOriginalKnifeT = g_MdlInfo->GetModelIndex("models/weapons/v_knife_default_t.mdl");

	int knife;

	if (nViewModelIndex == nOriginalKnifeCT || nViewModelIndex == nOriginalKnifeT)
	pViewModel->m_nModelIndex() = g_MdlInfo->GetModelIndex(ItemDefinitions::WeaponInfo.at(knife).model);
	*/
}


void Skinchanger::ApplyGlove(C_BaseAttributableItem* pGlove)
{
	pGlove->m_Item().m_iItemDefinitionIndex() = GloveCfg.iItemDefinitionIndex;

	pGlove->m_nFallbackPaintKit() = GloveCfg.nFallbackPaintKit;
	pGlove->m_nFallbackSeed() = GloveCfg.nFallbackSeed;
	pGlove->m_flFallbackWear() = std::max(0.01f, GloveCfg.flFallbackWear);

	pGlove->m_Item().m_iItemIDHigh() = -1;

	pGlove->m_nModelIndex() = g_MdlInfo->GetModelIndex(ItemDefinitions::WeaponInfo.at(GloveCfg.iItemDefinitionIndex).model);
	pGlove->PreDataUpdate(0);
}

void Skinchanger::InitSkins()
{
	



	GloveCfg.iItemDefinitionIndex = GLOVE_MOTORCYCLE;
	GloveCfg.nFallbackPaintKit = 10026;
}