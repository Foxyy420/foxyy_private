#pragma once

const char* BoxType[] =
{
	"Off",
	"Full",
	"Corners Only",
	"3D Box"
};

const char* GlowStlye[] =
{
	"Outline outer", "Cover", "Outline inner"
};

const char* weaponEspStyle[] =
{
	"Off", "Full", "Corners Only", "3D Box"
};

enum knifemdl
{
	No_Model,
	Pickaxe,
	Banana,
	Candy_Cane,
	Karate,
	Fidget_Spinner,
	Freedom_Staff,
	Hammer,
	Crowbar,
	Sword,
	Police_Baton,
};

const char* knifemdl[] =
{
	"Off",
	"Pickaxe",
	"Banana",
	"Candy Cane",
	"Karate",
	"Fidget Spinner",
	"Freedom Staff",
	"Hammer",
	"Crowbar",
	"Sword",
	"Police Baton"
};


const char* Arms[] =
{
	"Off",
	"FBI Glass",
	"Hydra",
	"Blue Crystal ",
	"Clear Crystal",
	"Gold",
	"Prediction Glass",
	"Gloss",

};

const char* HealthBar[] =
{
	"Off",
	"Normal",
	"Edgy"
};

const char* combobox_items2[] = { "None", "Fake Static", "Fake SideWays", "Fake Spin", "Fake Drop Inverse", "Fake Left Spin", "Backwards", "Backwards Dance", "Fake Backward Jitter", "SideWays Drop", "Test" };

const char* combobox_items[] = { "None", "Down", "Up", "Fake Jitter Down", "Fake Up", "Fake Zero", "Fake Down" };

const char* Hitsounds[] =
{
	"Off",
	"Skeet",
	"Normal",
	"Bubble"
};

const char* NameChange[] =
{
	"Erase Name",
	"Ayyware crasher",
	"Apocalpyse",
	"Japan Shit xd",
};

bool Skin_window = false;
bool colors_window = false;
int selected_Weapon = 0;

const char* weaponNamesForCombobox[] = {
	"Desert Eagle",
	"Dual Berettas",
	"Five-SeveN",
	"Glock-18",
	"AK-47",
	"AUG",
	"AWP",
	"FAMAS",
	"G3SG1",
	"Galil AR",
	"M249",
	"M4A4",
	"MAC-10",
	"P90",
	"UMP-45",
	"XM1014",
	"PP-Bizon",
	"MAG-7",
	"Negev",
	"Sawed.Off",
	"Tec-9",
	"P2000",
	"MP7",
	"MP9",
	"Nova",
	"P250",
	"SCAR-20",
	"SG 553",
	"SSG 08",
	"M4A1-S",
	"USP-S",
	"CZ75-Auto" ,
	"R8 Revolver",
	"Knife"
};

const char* knifeNames[] =
{
	"Bayonet",
	"Flip Knife",
	"Gut Knife",
	"karambit",
	"M9 Bayonet",
	"Tactical",
	"Falchion",
	"Bowie Knife",
	"Butterfly",
	"Shadow Daggers",
	"GunGame"
};


const char* keyNames[] =
{
	"",
	"Mouse 1",
	"Mouse 2",
	"Cancel",
	"Middle Mouse",
	"Mouse 4",
	"Mouse 5",
	"",
	"Backspace",
	"Tab",
	"",
	"",
	"Clear",
	"Enter",
	"",
	"",
	"Shift",
	"Control",
	"Alt",
	"Pause",
	"Caps",
	"",
	"",
	"",
	"",
	"",
	"",
	"Escape",
	"",
	"",
	"",
	"",
	"Space",
	"Page Up",
	"Page Down",
	"End",
	"Home",
	"Left",
	"Up",
	"Right",
	"Down",
	"",
	"",
	"",
	"Print",
	"Insert",
	"Delete",
	"",
	"0",
	"1",
	"2",
	"3",
	"4",
	"5",
	"6",
	"7",
	"8",
	"9",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"A",
	"B",
	"C",
	"D",
	"E",
	"F",
	"G",
	"H",
	"I",
	"J",
	"K",
	"L",
	"M",
	"N",
	"O",
	"P",
	"Q",
	"R",
	"S",
	"T",
	"U",
	"V",
	"W",
	"X",
	"Y",
	"Z",
	"",
	"",
	"",
	"",
	"",
	"Numpad 0",
	"Numpad 1",
	"Numpad 2",
	"Numpad 3",
	"Numpad 4",
	"Numpad 5",
	"Numpad 6",
	"Numpad 7",
	"Numpad 8",
	"Numpad 9",
	"Multiply",
	"Add",
	"",
	"Subtract",
	"Decimal",
	"Divide",
	"F1",
	"F2",
	"F3",
	"F4",
	"F5",
	"F6",
	"F7",
	"F8",
	"F9",
	"F10",
	"F11",
	"F12",

};

const char* world_modes[] =
{
	"Off",
	"Gray Mode",
	"Minecraft Mode",
	"MLG Mode"
};

const char* knife_models[] =
{
	"Basic",
	"Bayonet",
	"Flip",
	"Gut",
	"Karambit",
	"M9 Bayonet",
	"Tactical",
	"Falchion",
	"Bowie",
	"Butterfly",
	"Push"
};

const char* Hitmarker_type[] =
{
	"Normal",
	"3D"
};



const char *Skynames[] = { 
	"Default", 
	"cs_baggage_skybox_", 
	"cs_tibet", 
	"embassy", 
	"italy", 
	"jungle", 
	"nukeblank", 
	"office", 
	"sky_csgo_cloudy01", 
	"sky_csgo_night02", 
	"sky_csgo_night02b", 
	"sky_dust", 
	"sky_venice", 
	"vertigo", 
	"vietnam" 
};

static char* AntiaimPitchNames[] =
{
	"None",
	"Down",
	"Up",
	"Dance",
	"Fake Up",
	"Fake Down",
	"Random"
};

static char* AntiaimYawNames[] =
{
	"None",
	"Spin",
	"Static Forward",
	"Static Right",
	"Static Backwards",
	"Static Left",
	"Backwards",
	"Left",
	"Right",
	"Side",
	"Fake LBY 1",
	"Fake LBY 2",
	"Jitter",
	"Backjitter",
	"Fake Side LBY",
	"Random"
};