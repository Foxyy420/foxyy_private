#pragma once
#include "valve_sdk/csgostructs.hpp"
#include "singleton.hpp"

#include "KitParser.h"

#include <cereal/types/unordered_map.hpp>
#include <cereal/types/array.hpp>

struct StickerCfg_t
{
	int nIndex = 0;
	float flWear = FLT_MIN;
	float flScale = 1.f;
	float flRotation = 0.f;

	template<class Archive>
	void serialize(Archive & archive)
	{
		archive(cereal::make_nvp("index", nIndex),
			cereal::make_nvp("wear", flWear),
			cereal::make_nvp("scale", flScale),
			cereal::make_nvp("rotation", flRotation));
	}
};

struct WeaponItemCfg_t
{
	int nFallbackPaintKit = 0;
	int nFallbackSeed = 0;
	int nFallbackStatTrak = -1;
	float flFallbackWear = FLT_MIN;
	int iEntityQuality = 4;
	std::string szCustomName = "";
	std::array<StickerCfg_t, 4> stickers;

	template<class Archive>
	void serialize(Archive & archive)
	{
		archive(cereal::make_nvp("paintkit", nFallbackPaintKit),
			cereal::make_nvp("seed", nFallbackSeed),
			cereal::make_nvp("wear", flFallbackWear),
			cereal::make_nvp("stattrack", nFallbackStatTrak),
			cereal::make_nvp("name", szCustomName),
			cereal::make_nvp("stickers", stickers));
	}
};

struct KnifeItemCfg_t
{
	int iItemDefinitionIndex = 0;
	int nFallbackPaintKit = 0;
	int nFallbackSeed = 0;
	int nFallbackStatTrak = -1;
	float flFallbackWear = FLT_MIN;
	int iEntityQuality = 3;
	std::string szCustomName = "";

	template<class Archive>
	void serialize(Archive & archive)
	{
		archive(cereal::make_nvp("index", iItemDefinitionIndex),
			cereal::make_nvp("paintkit", nFallbackPaintKit),
			cereal::make_nvp("seed", nFallbackSeed),
			cereal::make_nvp("wear", flFallbackWear),
			cereal::make_nvp("stattrack", nFallbackStatTrak),
			cereal::make_nvp("name", szCustomName));
	}
};

struct GloveItemCfg_t
{
	int iItemDefinitionIndex = 0;
	int nFallbackPaintKit = 0;
	int nFallbackSeed = 0;
	float flFallbackWear = FLT_MIN;

	template<class Archive>
	void serialize(Archive & archive)
	{
		archive(cereal::make_nvp("index", iItemDefinitionIndex),
			cereal::make_nvp("paintkit", nFallbackPaintKit),
			cereal::make_nvp("wear", flFallbackWear));
	}
};

class Skinchanger : public Singleton<Skinchanger>
{
	void InitSkins();
	void ApplySkin(C_BaseAttributableItem* pWeapon, player_info_t pInfo);
	void ApplyModel(C_BasePlayer* pLocal, C_BaseAttributableItem* pWeapon);
	void ApplyGlove(C_BaseAttributableItem* pGlove);
	//void ApplyStickerHooks(C_BaseAttributableItem* item);
	KnifeItemCfg_t KnifeCfg;
	GloveItemCfg_t GloveCfg;
public:
	void Run();
	void Dump();
	void LoadSkins();
	const char* GetIconOverride(const std::string& original);
	std::unordered_map<int, WeaponItemCfg_t> WeaponCfg;
};