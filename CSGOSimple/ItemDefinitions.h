#pragma once

#include "valve_sdk/sdk.hpp"

#include <map>
#include <vector>

#include <cereal/archives/json.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/map.hpp>

namespace ItemDefinitions
{
	inline bool IsKnife(int i) { return (i >= WEAPON_KNIFE_BAYONET && i < GLOVE_STUDDED_BLOODHOUND) || i == WEAPON_KNIFE_T || i == WEAPON_KNIFE; }

	// Stupid MSVC requires separate constexpr constructors for any initialization

	struct Item_t
	{
		constexpr Item_t(const char* model, const char* icon = nullptr) :
			model(model),
			icon(icon)
		{}

		const char* model;
		const char* icon;
	};

	struct WeaponName_t
	{
		constexpr WeaponName_t(int definition_index, const char* name) :
			definition_index(definition_index),
			name(name)
		{}

		int definition_index = 0;
		const char* name = nullptr;
	};

	struct QualityName_t
	{
		constexpr QualityName_t(int index, const char* name) :
			index(index),
			name(name)
		{}

		int index = 0;
		const char* name = nullptr;
	};

	extern const std::map<size_t, Item_t> WeaponInfo;
	extern const std::vector<WeaponName_t> KnifeNames;
	extern const std::vector<WeaponName_t> GloveNames;
	extern const std::vector<WeaponName_t> WeaponNames;
	extern const std::vector<QualityName_t> QualityNames;
}