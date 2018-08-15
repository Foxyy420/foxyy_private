#pragma once

struct SkinInfo_t {
	int weapon_skin_id;
	int weapon_seed;
	int weapon_stat_trak;
	float weapon_wear;

	SkinInfo_t(int skin = 0, int seed = 0, int stattrak = -1, float wear = 0.000000001f) {
		weapon_skin_id = skin;
		weapon_seed = seed;
		weapon_stat_trak = stattrak;
		weapon_wear = wear;
	}
};