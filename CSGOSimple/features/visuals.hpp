#pragma once
#include <deque>


class C_BasePlayer;
class C_BaseEntity;
class C_BaseCombatWeapon;
class C_PlantedC4;
class Color;
class ClientClass;
class Vector;
class QAngle;

extern unsigned long esp_font;

namespace Visuals
{
	void AsusWalls();
	bool ValidPlayer(C_BasePlayer *player, bool = true);

	

	void LagCompHitbox(int index);
	void AngleLines();
	void EdgyHealthBar();
	void DLight(C_BaseEntity* entity);
	void Draw3dCube(float scalar, QAngle angles, Vector middle_origin, Color outline);
	void DrawString(unsigned long font, int x, int y, Color color, unsigned long alignment, const char* msg, ...);
	void ThreeDBox(Vector minin, Vector maxin, Vector pos, Color col);
	void GetTextSize(unsigned long font, const char *txt, int &width, int &height);
	void RecoilCrosshair();
	
	void NightMode();
	void SkyColor(); 

	namespace Player
	{
		bool Begin(C_BasePlayer* pl);

		void RenderBox(C_BaseEntity* ent);
		void RenderName();
		void RenderHealth();
		void RenderArmour();
		void RenderWeapon();
		void RenderSnapline();
		void RenderWaterMark();
		void RenderFill();
		void Skeleton();

	}

	namespace Misc
	{

		void RenderCrosshair();
		void RenderWeapon(C_BaseCombatWeapon* ent);
		void RenderDefuseKit(C_BaseEntity* ent);
		void RenderPlantedC4(C_BaseEntity* ent);
	}

	bool CreateFonts();
	void DestroyFonts();


};



