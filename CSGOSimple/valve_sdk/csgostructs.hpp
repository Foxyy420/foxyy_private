#pragma once

#include "sdk.hpp"
#include <array>

#define NETVAR(type, name, table, netvar)                           \
    type& name() const {                                          \
        static int _##name = NetvarSys::Get().GetOffset(table, netvar);     \
        return *(type*)((uintptr_t)this + _##name);                 \
    }

#define PNETVAR(type, name, table, netvar)                           \
    type* name() const {                                          \
        static int _##name = NetvarSys::Get().GetOffset(table, netvar);     \
        return (type*)((uintptr_t)this + _##name);                 \
    }

#define NETPROP(name, table, netvar) static RecvProp* name() \
{ \
	static auto prop_ptr = NetvarSys::Get().GetNetvarProp(table,netvar); \
	return prop_ptr; \
}


enum CSWeaponType
{
	WEAPONTYPE_KNIFE = 0,
	WEAPONTYPE_PISTOL,
	WEAPONTYPE_SUBMACHINEGUN,
	WEAPONTYPE_RIFLE,
	WEAPONTYPE_SHOTGUN,
	WEAPONTYPE_SNIPER_RIFLE,
	WEAPONTYPE_MACHINEGUN,
	WEAPONTYPE_C4,
	WEAPONTYPE_PLACEHOLDER,
	WEAPONTYPE_GRENADE,
	WEAPONTYPE_UNKNOWN
};

class C_BaseEntity;
class AnimationLayer;
class C_CSGOPlayerAnimState;

// Created with ReClass.NET by KN4CK3R
class CHudTexture
{
public:
	char szShortName[64];    //0x0000
	char szTextureFile[64];  //0x0040
	bool bRenderUsingFont;   //0x0080
	bool bPrecached;         //0x0081
	int8_t cCharacterInFont; //0x0082
	uint8_t pad_0083[1];     //0x0083
	uint32_t hFont;          //0x0084
	int32_t iTextureId;      //0x0088
	float afTexCoords[4];    //0x008C
	uint8_t pad_009C[16];    //0x009C
};

#pragma pack(push, 1)
class CCSWeaponInfo
{
public:
	char _0x0000[20];
	__int32 max_clip;            //0x0014 
	char _0x0018[12];
	__int32 max_reserved_ammo;    //0x0024 
	char _0x0028[96];
	char* hud_name;                //0x0088 
	char* weapon_name;            //0x008C 
	char _0x0090[56];
	int iWeaponType;
	char _0x0094[4];
	__int32 price;                //0x00D0 
	__int32 reward;                //0x00D4 
	char _0x00D8[20];
	BYTE full_auto;                //0x00EC 
	char _0x00ED[3];
	int    iDamage;                //0x00F0 
	float flArmorRatio;            //0x00F4 
	__int32 bullets;            //0x00F8 
	float flPenetration;            //0x00FC 
	char _0x0100[8];
	float flRange;                //0x0108 
	float flRangeModifier;        //0x010C 
	char _0x0110[16];
	BYTE silencer;                //0x0120 
	char _0x0121[15];
	float max_speed;            //0x0130 
	float max_speed_alt;        //0x0134 
	char _0x0138[76];
	__int32 recoil_seed;        //0x0184 
	char _0x0188[32];
	bool bFullAuto; // 0x00E8 
};
#pragma pack(pop)


class C_CSGOPlayerAnimState
{
public:

	int32_t & m_iLastClientSideAnimationUpdateFramecount()
	{
		return *(int32_t*)((uintptr_t)this + 0x70);
	}

	float_t &m_flEyeYaw()
	{
		return *(float_t*)((uintptr_t)this + 0x78);
	}

	float_t &m_flEyePitch()
	{
		return *(float_t*)((uintptr_t)this + 0x7C);
	}

	float_t &m_flGoalFeetYaw()
	{
		return *(float_t*)((uintptr_t)this + 0x80);
	}

	float_t &m_flCurrentFeetYaw()
	{
		return *(float_t*)((uintptr_t)this + 0x84);
	}

	bool &m_bCurrentFeetYawInitialized()
	{
		return *(bool*)((uintptr_t)this + 0x88);
	}

	Vector &m_vecVelocity()
	{
		// Only on ground velocity
		return *(Vector*)((uintptr_t)this + 0xC8);
	}

	float_t m_flVelocity()
	{
		return *(float_t*)((uintptr_t)this + 0xEC);
	}

	char pad_0x0000[0x344]; //0x0000
}; //Size=0x344

class C_EconItemView
{
public:
	NETVAR(int32_t, m_bInitialized, "DT_BaseAttributableItem", "m_bInitialized");
	NETVAR(short, m_iItemDefinitionIndex, "DT_BaseAttributableItem", "m_iItemDefinitionIndex");
	NETVAR(int32_t, m_iEntityLevel, "DT_BaseAttributableItem", "m_iEntityLevel");
	NETVAR(int32_t, m_iAccountID, "DT_BaseAttributableItem", "m_iAccountID");
	NETVAR(int32_t, m_iItemIDLow, "DT_BaseAttributableItem", "m_iItemIDLow");
	NETVAR(int32_t, m_iItemIDHigh, "DT_BaseAttributableItem", "m_iItemIDHigh");
	NETVAR(int32_t, m_iEntityQuality, "DT_BaseAttributableItem", "m_iEntityQuality");
};

class C_BaseEntity : public IClientEntity
{
public:
	static __forceinline C_BaseEntity* GetEntityByIndex(int index)
	{
		return static_cast<C_BaseEntity*>(g_EntityList->GetClientEntity(index));
	}
	static __forceinline C_BaseEntity* get_entity_from_handle(CBaseHandle h)
	{
		return static_cast<C_BaseEntity*>(g_EntityList->GetClientEntityFromHandle(h));
	}

	NETVAR(int32_t, m_nModelIndex, "DT_BaseEntity", "m_nModelIndex");
	NETVAR(float, m_flSimulationTime, "DT_BaseEntity", "m_flSimulationTime");
	NETVAR(int32_t, m_iTeamNum, "DT_BaseEntity", "m_iTeamNum");
	NETVAR(Vector, m_vecOrigin, "DT_BaseEntity", "m_vecOrigin");
	NETVAR(bool, m_bShouldGlow, "DT_DynamicProp", "m_bShouldGlow");
	NETVAR(CHandle<C_BasePlayer>, m_hOwnerEntity, "DT_BaseEntity", "m_hOwnerEntity");

	const matrix3x4_t& m_rgflCoordinateFrame()
	{
		static auto _m_rgflCoordinateFrame = NetvarSys::Get().GetOffset("DT_BaseEntity", "m_CollisionGroup") - 0x30;
		return *(matrix3x4_t*)((uintptr_t)this + _m_rgflCoordinateFrame);
	}

	Vector GetEyePosition() {
		Vector Origin = *(Vector*)((DWORD)this + 0x00000134);
		Vector View = *(Vector*)((DWORD)this + 0x00000104);
		return(Origin + View);
	}


	bool IsPlayer();
	bool IsWeapon();
	bool IsPlantedC4();
	bool IsDefuseKit();
};

class PlayerList_t
{
public:
	C_BasePlayer * pPlayer;
	bool bFriend;
	bool bHighPriority;
	bool bBaim;
	int bFixX;
private:
};
extern PlayerList_t PlayerList[65];

class C_PlantedC4
{
public:
	NETVAR(bool, m_bBombTicking, "DT_PlantedC4", "m_bBombTicking");
	NETVAR(bool, m_bBombDefused, "DT_PlantedC4", "m_bBombDefused");
	NETVAR(float, m_flC4Blow, "DT_PlantedC4", "m_flC4Blow");
	NETVAR(float, m_flTimerLength, "DT_PlantedC4", "m_flTimerLength");
	NETVAR(float, m_flDefuseLength, "DT_PlantedC4", "m_flDefuseLength");
	NETVAR(float, m_flDefuseCountDown, "DT_PlantedC4", "m_flDefuseCountDown");
	NETVAR(CHandle<C_BasePlayer>, m_hBombDefuser, "DT_PlantedC4", "m_hBombDefuser");
};

class C_BaseAttributableItem : public C_BaseEntity
{
public:
	NETVAR(uint64_t, m_OriginalOwnerXuid, "DT_BaseAttributableItem", "m_OriginalOwnerXuidLow");
	NETVAR(int32_t, m_OriginalOwnerXuidLow, "DT_BaseAttributableItem", "m_OriginalOwnerXuidLow");
	NETVAR(int32_t, m_OriginalOwnerXuidHigh, "DT_BaseAttributableItem", "m_OriginalOwnerXuidHigh");
	NETVAR(int32_t, m_nFallbackStatTrak, "DT_BaseAttributableItem", "m_nFallbackStatTrak");
	NETVAR(int32_t, m_nFallbackPaintKit, "DT_BaseAttributableItem", "m_nFallbackPaintKit");
	NETVAR(int32_t, m_nFallbackSeed, "DT_BaseAttributableItem", "m_nFallbackSeed");
	NETVAR(float_t, m_flFallbackWear, "DT_BaseAttributableItem", "m_flFallbackWear");

	C_EconItemView& m_Item()
	{
		// Cheating. It should be this + m_Item netvar but then the netvars inside C_EconItemView wont work properly.
		// A real fix for this requires a rewrite of the netvar manager
		return *(C_EconItemView*)this;
	}
};


class C_BaseCombatWeapon : public C_BaseAttributableItem
{
public:
	NETVAR(float_t, m_flNextPrimaryAttack, "DT_BaseCombatWeapon", "m_flNextPrimaryAttack");
	NETVAR(float_t, m_flNextSecondaryAttack, "DT_BaseCombatWeapon", "m_flNextSecondaryAttack");
	NETVAR(int32_t, m_iClip1, "DT_BaseCombatWeapon", "m_iClip1");
	NETVAR(int32_t, m_iClip2, "DT_BaseCombatWeapon", "m_iClip2");
	NETVAR(float_t, m_flRecoilIndex, "DT_WeaponCSBase", "m_flRecoilIndex");
	NETVAR(int32_t, m_iItemDefinitionIndex, "CBaseCombatWeapon", "m_iItemDefinitionIndex");
	NETVAR(float_t, m_flPostponeFireReadyTime, "CWeaponCSBase", "m_flPostponeFireReadyTime");
	NETVAR(bool, m_bPinPulled, "CBaseCSGrenade", "m_bPinPulled");
	NETVAR(float_t, m_fThrowTime, "CBaseCSGrenade", "m_fThrowTime");
	NETVAR(int32_t, m_zoomLevel, "CWeaponCSBaseGun", "m_zoomLevel");

	CCSWeaponInfo* GetCSWeaponData();
	bool HasBullets();
	bool CanFire();
	bool IsGrenade();
	bool IsKnife();
	bool IsReloading();
	bool IsRifle();
	bool IsPistol();
	bool IsSniper();
	float GetInaccuracy();
	float GetSpread();
	bool CanFirePostPone();
	void UpdateAccuracyPenalty();
	bool IsWeaponNonAim();
	bool IsInThrow();

};

class AnimationLayer
{
public:
	char  pad_0000[20];
	// These should also be present in the padding, don't see the use for it though
	//float	m_flLayerAnimtime;
	//float	m_flLayerFadeOuttime;
	uint32_t m_nOrder; //0x0014
	uint32_t m_nSequence; //0x0018
	float_t m_flPrevCycle; //0x001C
	float_t m_flWeight; //0x0020
	float_t m_flWeightDeltaRate; //0x0024
	float_t m_flPlaybackRate; //0x0028
	float_t m_flCycle; //0x002C
	void *m_pOwner; //0x0030 // player's thisptr
	char  pad_0038[4]; //0x0034
}; //Size: 0x0038


class C_BasePlayer : public C_BaseEntity
{
public:
	static __forceinline C_BasePlayer* GetPlayerByUserId(int id)
	{
		return static_cast<C_BasePlayer*>(GetEntityByIndex(g_EngineClient->GetPlayerForUserID(id)));
	}
	static __forceinline C_BasePlayer* GetPlayerByIndex(int i)
	{
		return static_cast<C_BasePlayer*>(GetEntityByIndex(i));
	}

	NETVAR(bool, m_IsRescuing, "DT_CSPlayer", "m_bIsRescuing");
	NETVAR(bool, m_IsGrabbingHostage, "DT_CSPlayer", "m_bIsGrabbingHostage");


	NETVAR(bool, m_bHasHeavyArmor, "CCSPlayer", "m_bHasHeavyArmor");
	NETVAR(int, m_iObserverMode, "CBasePlayer", "m_iObserverMode");
	NETVAR(bool, m_bHasDefuser, "DT_CSPlayer", "m_bHasDefuser");
	NETVAR(bool, m_bGunGameImmunity, "DT_CSPlayer", "m_bGunGameImmunity");
	NETVAR(int32_t, m_iShotsFired, "DT_CSPlayer", "m_iShotsFired");
	NETVAR(QAngle, m_angEyeAngles, "DT_CSPlayer", "m_angEyeAngles[0]");
	NETVAR(QAngle, m_angEyeAngles2, "DT_CSPlayer", "m_angEyeAngles[1]");
	NETVAR(int, m_ArmorValue, "DT_CSPlayer", "m_ArmorValue");
	NETVAR(bool, m_bHasHelmet, "DT_CSPlayer", "m_bHasHelmet");
	NETVAR(bool, m_bIsScoped, "DT_CSPlayer", "m_bIsScoped");;
	NETVAR(float, m_flLowerBodyYawTarget, "DT_CSPlayer", "m_flLowerBodyYawTarget");
	NETVAR(float, m_flFlashDuration, "DT_CSPlayer", "m_flFlashDuration");
	NETVAR(int32_t, m_iHealth, "DT_BasePlayer", "m_iHealth");
	NETVAR(int32_t, m_lifeState, "DT_BasePlayer", "m_lifeState");
	NETVAR(int32_t, m_fFlags, "DT_BasePlayer", "m_fFlags");
	NETVAR(int32_t, m_nTickBase, "DT_BasePlayer", "m_nTickBase");
	NETVAR(Vector, m_vecViewOffset, "DT_BasePlayer", "m_vecViewOffset[0]");
	NETVAR(QAngle, m_viewPunchAngle, "DT_BasePlayer", "m_viewPunchAngle");
	NETVAR(QAngle, m_aimPunchAngle, "DT_BasePlayer", "m_aimPunchAngle");
	NETVAR(CHandle<C_BaseViewModel>, m_hViewModel, "DT_BasePlayer", "m_hViewModel[0]");
	NETVAR(Vector, m_vecVelocity, "DT_BasePlayer", "m_vecVelocity[0]");
	NETVAR(float, m_flMaxspeed, "DT_BasePlayer", "m_flMaxspeed");
	NETVAR(CHandle<C_BasePlayer>, m_hObserverTarget, "DT_BasePlayer", "m_hObserverTarget");
	NETVAR(CHandle<C_BaseCombatWeapon>, m_hActiveWeapon, "DT_BaseCombatCharacter", "m_hActiveWeapon");
	PNETVAR(CHandle<C_BaseCombatWeapon>, m_hMyWeapons, "DT_BaseCombatCharacter", "m_hMyWeapons");
	PNETVAR(CBaseHandle, m_hMyWearables, "DT_BaseCombatCharacter", "m_hMyWearables");

	CUserCmd*& m_pCurrentCommand();
	void CreateAnimationState(C_CSGOPlayerAnimState *state);
	static void UpdateAnimationState(C_CSGOPlayerAnimState *state, QAngle angle);
	float_t m_flSpawnTime();
	static void ResetAnimationState(C_CSGOPlayerAnimState *state);



	bool IsMoving()
	{
		if (this->m_vecVelocity().Length2D() > 0.0f)
			return true;

		return false;
	}

	bool IsBreakingLBY()
	{
		return IsMoving();
	}

	bool IsInFakewalk()
	{
		if (this->m_vecVelocity().Length2D() > 0.0f && this->m_vecVelocity().Length2D() <= 40.0f)
			return true;

		return false;
	}

	int GetNumAnimOverlays();
	AnimationLayer *GetAnimOverlays();
	AnimationLayer *GetAnimOverlay(int i);
	int GetSequenceActivity(int sequence);
	std::string GetName(bool console_safe = false);

	Vector        GetEyePos();
	player_info_t GetPlayerInfo();
	bool          IsAlive();
	bool          HasC4();
	Vector        GetHitboxPos(int hitbox_id);
	bool          GetHitboxPos(int hitbox, Vector &output);
	Vector        GetBonePos(int bone);
	bool          CanSeePlayer(C_BasePlayer* player, int hitbox);
	bool          CanSeePlayer(C_BasePlayer* player, const Vector& pos);

	int32_t GetMoveType();

	QAngle &visuals_Angles();

	Vector* GetEyeY()
	{
		return (Vector*)((uintptr_t)this + 0xAA08);
	}
	Vector* GetEyeX()
	{
		return (Vector*)((uintptr_t)this + 0xAA0C);
	}
	bool IsFlashed()
	{
		return m_flFlashDuration() > 0;
	}

};

class C_BaseViewModel : public C_BaseEntity
{
public:
	NETVAR(int32_t, m_nModelIndex, "DT_BaseViewModel", "m_nModelIndex");
	NETVAR(int32_t, m_nViewModelIndex, "DT_BaseViewModel", "m_nViewModelIndex");
	NETVAR(CHandle<C_BaseCombatWeapon>, m_hWeapon, "DT_BaseViewModel", "m_hWeapon");
	NETVAR(CHandle<C_BasePlayer>, m_hOwner, "DT_BaseViewModel", "m_hOwner");
	NETPROP(m_nSequence, "DT_BaseViewModel", "m_nSequence");
	/*
	NETPROP(GetSequenceProp, "DT_BaseViewModel", "m_nSequence");
	void SendViewModelMatchingSequence(int sequence)
	{
	typedef  void(__thiscall* OriginalFn)(void*, int);
	return CallVFunction<OriginalFn>(this, 241)(this, sequence);
	}
	*/
};

class CBaseViewModel {
public:
	inline int GetModelIndex() {

		return *(int*)((DWORD)this + NetvarSys::Get().GetOffset("DT_BaseViewModel", "m_nModelIndex"));
	}

	inline void SetModelIndex(int nModelIndex) {
		*(int*)((DWORD)this + NetvarSys::Get().GetOffset("DT_BaseViewModel", "m_nModelIndex")) = nModelIndex;
	}

	inline DWORD GetOwner() {

		return *(PDWORD)((DWORD)this + NetvarSys::Get().GetOffset("DT_BaseViewModel", "m_hOwner"));
	}

	inline DWORD GetWeapon() {

		return *(PDWORD)((DWORD)this + NetvarSys::Get().GetOffset("DT_BaseViewModel", "m_hWeapon"));
	}
};