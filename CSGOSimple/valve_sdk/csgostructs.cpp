#include "csgostructs.hpp"
#include "../helpers/math.hpp"
#include "../helpers/utils.hpp"
#include "../hooks.hpp"

bool C_BaseEntity::IsPlayer()
{
	//index: 152
	//ref: "effects/nightvision"
	//sig: 8B 92 ? ? ? ? FF D2 84 C0 0F 45 F7 85 F6
	return CallVFunction<bool(__thiscall*)(C_BaseEntity*)>(this, 152)(this);
}

bool C_BaseEntity::IsWeapon()
{
	//index: 160
	//ref: "CNewParticleEffgitvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvect::DrawModel"
	//sig: 8B 80 ? ? ? ? FF D0 84 C0 74 6F 8B 4D A4
	return CallVFunction<bool(__thiscall*)(C_BaseEntity*)>(this, 160)(this);
}

bool C_BaseEntity::IsPlantedC4()
{
	return GetClientClass()->m_ClassID == ClassId_CPlantedC4;
}

bool C_BaseEntity::IsDefuseKit()
{
	return GetClientClass()->m_ClassID == ClassId_CBaseAnimating;
}

CCSWeaponInfo* C_BaseCombatWeapon::GetCSWeaponData()
{
	return CallVFunction<CCSWeaponInfo*(__thiscall*)(void*)>(this, 445)(this);
}

bool C_BaseCombatWeapon::HasBullets()
{
	return !IsReloading() && m_iClip1() > 0;
}

bool C_BaseCombatWeapon::CanFire()
{
	if (IsReloading() || m_iClip1() <= 0)
		return false;

	if (!g_LocalPlayer)
		return false;

	float flServerTime = g_LocalPlayer->m_nTickBase() * g_GlobalVars->interval_per_tick;

	return m_flNextPrimaryAttack() <= flServerTime;
}

bool C_BaseCombatWeapon::IsGrenade()
{
	return GetCSWeaponData()->iWeaponType == WEAPONTYPE_GRENADE;
}

bool C_BaseCombatWeapon::IsKnife()
{
	return GetCSWeaponData()->iWeaponType == WEAPONTYPE_KNIFE;
}


int C_BasePlayer::GetNumAnimOverlays()
{
	return *(int*)((DWORD)this + 0x297C);
}

AnimationLayer *C_BasePlayer::GetAnimOverlays()
{
	// to find offset: use 9/12/17 dll
	// sig: 55 8B EC 51 53 8B 5D 08 33 C0
	return *(AnimationLayer**)((DWORD)this + 0x2970);
}

AnimationLayer *C_BasePlayer::GetAnimOverlay(int i)
{
	if (i < 15)
		return &GetAnimOverlays()[i];
}


int32_t C_BasePlayer::GetMoveType()
{
	return *(int32_t*)((uintptr_t)this + 0x258);
}

bool C_BaseCombatWeapon::IsWeaponNonAim()
{
	int idx = ItemDefinitionIndex();

	return (idx == WEAPON_C4 || idx == WEAPON_KNIFE || idx == WEAPON_KNIFE_BUTTERFLY || idx == WEAPON_KNIFE_FALCHION
		|| idx == WEAPON_KNIFE_FLIP || idx == WEAPON_KNIFE_GUT || idx == WEAPON_KNIFE_KARAMBIT || idx == WEAPON_KNIFE_M9_BAYONET || idx == WEAPON_KNIFE_PUSH
		|| idx == WEAPON_KNIFE_SURVIVAL_BOWIE || idx == WEAPON_KNIFE_T || idx == WEAPON_KNIFE_TACTICAL || idx == WEAPON_FLASHBANG || idx == WEAPON_HEGRENADE
		|| idx == WEAPON_SMOKEGRENADE || idx == WEAPON_MOLOTOV || idx == WEAPON_DECOY || idx == WEAPON_INCGRENADE);
}

bool C_BaseCombatWeapon::IsInThrow()
{
	if (!m_bPinPulled() || (Global::userCMD->buttons & IN_ATTACK) || (Global::userCMD->buttons & IN_ATTACK2))
	{
		float throwTime = m_fThrowTime();

		if (throwTime > 0)
			return true;
	}
	return false;
}


bool C_BaseCombatWeapon::IsRifle()
{
	switch (GetCSWeaponData()->iWeaponType)
	{
	case WEAPONTYPE_RIFLE:
	case WEAPONTYPE_SHOTGUN:
	case WEAPONTYPE_SUBMACHINEGUN:
	case WEAPONTYPE_MACHINEGUN:
		return true;
	default:
		return false;
	}
}



bool C_BaseCombatWeapon::IsPistol()
{
	return GetCSWeaponData()->iWeaponType == WEAPONTYPE_PISTOL;
}

bool C_BaseCombatWeapon::IsSniper()
{
	return GetCSWeaponData()->iWeaponType == WEAPONTYPE_SNIPER_RIFLE;
}

bool C_BaseCombatWeapon::IsReloading()
{
	static auto inReload = *(uint32_t*)(Utils::PatternScan(GetModuleHandleW(L"client.dll"), "C6 87 ? ? ? ? ? 8B 06 8B CE FF 90") + 2);
	return *(bool*)((uintptr_t)this + inReload);
}

float C_BaseCombatWeapon::GetInaccuracy()
{
	return CallVFunction<float(__thiscall*)(void*)>(this, 468)(this);
}

float C_BaseCombatWeapon::GetSpread()
{
	return CallVFunction<float(__thiscall*)(void*)>(this, 437)(this);
}

bool C_BaseCombatWeapon::CanFirePostPone()
{
	float rdyTime = m_flPostponeFireReadyTime();

	if (rdyTime > 0 && rdyTime < g_GlobalVars->curtime)
		return true;

	return false;
}

void C_BaseCombatWeapon::UpdateAccuracyPenalty()
{
	CallVFunction<void(__thiscall*)(void*)>(this, 469)(this);
}

CUserCmd*& C_BasePlayer::m_pCurrentCommand()
{
	static auto currentCommand = *(uint32_t*)(Utils::PatternScan(GetModuleHandleW(L"client.dll"), "89 BE ? ? ? ? E8 ? ? ? ? 85 FF") + 2);
	return *(CUserCmd**)((uintptr_t)this + currentCommand);
}

Vector C_BasePlayer::GetEyePos()
{
	return m_vecOrigin() + m_vecViewOffset();
}

int C_BasePlayer::GetSequenceActivity(int sequence)
{
	auto hdr = g_MdlInfo->GetStudioModel(this->GetModel());

	if (!hdr)
		return -1;

	// sig for stuidohdr_t version: 53 56 8B F1 8B DA 85 F6 74 55
	// sig for C_BaseAnimating version: 55 8B EC 83 7D 08 FF 56 8B F1 74 3D
	// c_csplayer vfunc 242, follow calls to find the function.

	static auto get_sequence_activity = reinterpret_cast<int(__fastcall*)(void*, studiohdr_t*, int)>(Offsets::getSequenceActivity);

	return get_sequence_activity(this, hdr, sequence);
}

player_info_t C_BasePlayer::GetPlayerInfo()
{
	player_info_t info;
	g_EngineClient->GetPlayerInfo(EntIndex(), &info);
	return info;
}

bool C_BasePlayer::IsAlive()
{
	return m_lifeState() == LIFE_ALIVE;
}

bool C_BasePlayer::HasC4()
{
	static auto fnHasC4
		= reinterpret_cast<bool(__thiscall*)(void*)>(
			Utils::PatternScan(GetModuleHandleW(L"client.dll"), "56 8B F1 85 F6 74 31")
			);

	return fnHasC4(this);
}

Vector C_BasePlayer::GetHitboxPos(int hitbox_id)
{
	matrix3x4_t boneMatrix[MAXSTUDIOBONES];

	if (SetupBones(boneMatrix, MAXSTUDIOBONES, BONE_USED_BY_HITBOX, 0.0f)) {
		auto studio_model = g_MdlInfo->GetStudioModel(GetModel());
		if (studio_model) {
			auto hitbox = studio_model->GetHitboxSet(0)->GetHitbox(hitbox_id);
			if (hitbox) {
				auto
					min = Vector{},
					max = Vector{};

				Math::VectorTransform(hitbox->bbmin, boneMatrix[hitbox->bone], min);
				Math::VectorTransform(hitbox->bbmax, boneMatrix[hitbox->bone], max);

				return (min + max) / 2.0f;
			}
		}
	}
	return Vector{};
}

bool C_BasePlayer::GetHitboxPos(int hitbox, Vector &output)
{
	if (hitbox >= HITBOX_MAX)
		return false;

	const model_t *model = this->GetModel();
	if (!model)
		return false;

	studiohdr_t *studioHdr = g_MdlInfo->GetStudioModel(model);
	if (!studioHdr)
		return false;

	matrix3x4_t matrix[MAXSTUDIOBONES];
	if (!this->SetupBones(matrix, MAXSTUDIOBONES, 0x100, 0))
		return false;

	mstudiobbox_t *studioBox = studioHdr->GetHitboxSet(0)->GetHitbox(hitbox);
	if (!studioBox)
		return false;

	Vector min, max;

	Math::VectorTransform(studioBox->bbmin, matrix[studioBox->bone], min);
	Math::VectorTransform(studioBox->bbmax, matrix[studioBox->bone], max);

	output = (min + max) * 0.5f;

	return true;
}

Vector C_BasePlayer::GetBonePos(int bone)
{
	matrix3x4_t boneMatrix[MAXSTUDIOBONES];

	if (SetupBones(boneMatrix, MAXSTUDIOBONES, BONE_USED_BY_ANYTHING, 0.0f)) {
		return boneMatrix[bone].at(3);
	}
	return Vector{};
}

std::string C_BasePlayer::GetName(bool console_safe)
{
	// Cleans player's name so we don't get new line memes. Use this everywhere you get the players name.
	// Also, if you're going to use the console for its command and use the players name, set console_safe.
	player_info_t pinfo = this->GetPlayerInfo();

	char* pl_name = pinfo.szName;
	char buf[128];
	int c = 0;

	for (int i = 0; pl_name[i]; ++i)
	{
		if (c >= sizeof(buf) - 1)
			break;

		switch (pl_name[i])
		{
		case '"': if (console_safe) break;
		case '\\':
		case ';': if (console_safe) break;
		case '\n':
			break;
		default:
			buf[c++] = pl_name[i];
		}
	}

	buf[c] = '\0';
	return std::string(buf);
}

bool C_BasePlayer::CanSeePlayer(C_BasePlayer* player, int hitbox)
{
	CGameTrace tr;
	Ray_t ray;
	CTraceFilter filter;
	filter.pSkip = this;

	auto endpos = player->GetHitboxPos(hitbox);

	ray.Init(GetEyePos(), endpos);
	g_EngineTrace->TraceRay(ray, MASK_SHOT | CONTENTS_GRATE, &filter, &tr);

	return tr.hit_entity == player || tr.fraction > 0.97f;
}

float_t C_BasePlayer::m_flSpawnTime()
{
	return *(float_t*)((uintptr_t)this + 0xA290);
}

void C_BasePlayer::ResetAnimationState(C_CSGOPlayerAnimState *state)
{
	if (!state)
		return;

	using ResetAnimState_t = void(__thiscall*)(C_CSGOPlayerAnimState*);
	static auto ResetAnimState = (ResetAnimState_t)Utils::PatternScan(GetModuleHandle(L"client.dll"), "56 6A 01 68 ? ? ? ? 8B F1");
	if (!ResetAnimState)
		return;

	ResetAnimState(state);
}

void C_BasePlayer::UpdateAnimationState(C_CSGOPlayerAnimState *state, QAngle angle)
{
	if (!state)
		return;

	static auto UpdateAnimState = Utils::PatternScan(GetModuleHandle(L"client.dll"), "55 8B EC 83 E4 F8 83 EC 18 56 57 8B F9 F3 0F 11 54 24");
	if (!UpdateAnimState)
		return;

	__asm
	{
		mov ecx, state

		movss xmm1, dword ptr[angle + 4]
		movss xmm2, dword ptr[angle]

		call UpdateAnimState
	}
}

void C_BasePlayer::CreateAnimationState(C_CSGOPlayerAnimState *state)
{
	using CreateAnimState_t = void(__thiscall*)(C_CSGOPlayerAnimState*, C_BasePlayer*);
	static auto CreateAnimState = (CreateAnimState_t)Utils::PatternScan(GetModuleHandle(L"client.dll"), "55 8B EC 56 8B F1 B9 ? ? ? ? C7 46");
	if (!CreateAnimState)
		return;

	CreateAnimState(state, this);
}


bool C_BasePlayer::CanSeePlayer(C_BasePlayer* player, const Vector& pos)
{
	CGameTrace tr;
	Ray_t ray;
	CTraceFilter filter;
	filter.pSkip = this;

	auto start = GetEyePos();
	auto dir = (pos - start).Normalized();

	ray.Init(start, pos);
	g_EngineTrace->TraceRay(ray, MASK_SHOT | CONTENTS_GRATE, &filter, &tr);

	return tr.hit_entity == player || tr.fraction > 0.97f;
}
