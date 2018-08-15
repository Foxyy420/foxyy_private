#include "visuals.hpp"

#include "../options.hpp"
#include "../valve_sdk/csgostructs.hpp"
#include "../helpers/math.hpp"
#include "../helpers/utils.hpp"
#include "../BackTrack.h"
#include "../MiscFunctions.h"

#define RandomInt(nMin, nMax) (rand() % (nMax - nMin + 1) + nMin);
std::unordered_map<char*, char*> killIcons = {};
#define INVALID_EHANDLE_INDEX 0xFFFFFFFF
HANDLE worldmodel_handle;
C_BaseCombatWeapon* worldmodel;


vgui::HFont watermark_font;
vgui::HFont esp_font;
vgui::HFont defuse_font;
vgui::HFont dropped_weapons_font;
vgui::HFont spectator_list_font;
vgui::HFont weapon_icon;

// ESP Context
// This is used so that we dont have to calculate player color and position
// on each individual function over and over
struct
{
	C_BasePlayer* pl;
	bool          is_enemy;
	bool          is_visible;
	Color         clr;
	Color		  filled;
	Vector        head_pos;
	Vector        feet_pos;
	RECT          bbox;
	float         Fade[64];
	int reservedspace_right;


} esp_ctx;

bool Visuals::ValidPlayer(C_BasePlayer *player, bool count_step)
{
	int idx = player->EntIndex();
	constexpr float frequency = 0.35f / 0.5f;
	float step = frequency * g_GlobalVars->frametime;
	if (!player->IsAlive())
		return false;

	// Don't render esp if in firstperson viewing player.
	
	if (player == esp_ctx.pl)
	{
		if (g_LocalPlayer->m_iObserverMode() == 4)
			return false;
	}

	if (player == g_LocalPlayer)
	{
		if (!g_Input->m_fCameraInThirdPerson)
			return false;
	}
	

	if (count_step)
	{
		if (!player->IsDormant()) {
			if (esp_ctx.Fade[idx] < 1.f)
				esp_ctx.Fade[idx] += step;
		}
		else {
			if (esp_ctx.Fade[idx] > 0.f)
				esp_ctx.Fade[idx] -= step;
		}
		esp_ctx.Fade[idx] = (esp_ctx.Fade[idx] > 1.f ? 1.f : esp_ctx.Fade[idx] < 0.f ? 0.f : esp_ctx.Fade[idx]);
	}

	return (esp_ctx.Fade[idx] > 0.f);
}



RECT GetBBox(C_BaseEntity* ent, Vector pointstransf[])
{
	RECT rect{};
	auto collideable = ent->GetCollideable();

	if (!collideable)
		return rect;

	auto min = collideable->OBBMins();
	auto max = collideable->OBBMaxs();

	const matrix3x4_t &trans = ent->m_rgflCoordinateFrame();

	Vector points[] =
	{
		Vector(min.x, min.y, min.z),
		Vector(min.x, max.y, min.z),
		Vector(max.x, max.y, min.z),
		Vector(max.x, min.y, min.z),
		Vector(max.x, max.y, max.z),
		Vector(min.x, max.y, max.z),
		Vector(min.x, min.y, max.z),
		Vector(max.x, min.y, max.z)
	};

	Vector pointsTransformed[8];
	for (int i = 0; i < 8; i++) {
		Math::VectorTransform(points[i], trans, pointsTransformed[i]);
	}

	Vector pos = ent->m_vecOrigin();
	Vector screen_points[8] = {};

	for (int i = 0; i < 8; i++)
		if (!Math::WorldToScreen(pointsTransformed[i], screen_points[i]))
			return rect;
		else
			pointstransf[i] = screen_points[i];

	auto left = screen_points[0].x;
	auto top = screen_points[0].y;
	auto right = screen_points[0].x;
	auto bottom = screen_points[0].y;

	for (int i = 1; i < 8; i++)
	{
		if (left > screen_points[i].x)
			left = screen_points[i].x;
		if (top < screen_points[i].y)
			top = screen_points[i].y;
		if (right < screen_points[i].x)
			right = screen_points[i].x;
		if (bottom > screen_points[i].y)
			bottom = screen_points[i].y;
	}
	return RECT{ (long)left, (long)top, (long)right, (long)bottom };
}


RECT GetViewport()
{
	RECT viewport = { 0, 0, 0, 0 };
	int w, h;
	g_EngineClient->GetScreenSize(w, h);
	viewport.right = w; viewport.bottom = h;

	return viewport;
}
void TextW(bool center, unsigned long font, int x, int y, Color c, wchar_t *pszString)
{
	if (center)
	{
		int wide, tall;
		g_VGuiSurface->GetTextSize(font, pszString, wide, tall);
		x -= wide / 2;
		y -= tall / 2;
	}
	g_VGuiSurface->DrawSetTextColor(c);
	g_VGuiSurface->DrawSetTextFont(font);
	g_VGuiSurface->DrawSetTextPos(x, y);
	g_VGuiSurface->DrawPrintText(pszString, (int)wcslen(pszString), FONT_DRAW_DEFAULT);
}

void Visuals::GetTextSize(unsigned long font, const char *txt, int &width, int &height)
{
	size_t origsize = strlen(txt) + 1;
	const size_t newsize = 100;
	size_t convertedChars = 0;
	wchar_t wcstring[newsize];
	int x, y;

	mbstowcs_s(&convertedChars, wcstring, origsize, txt, _TRUNCATE);

	g_VGuiSurface->GetTextSize(font, wcstring, x, y);

	width = x;
	height = y;
}


void DrawString2(unsigned long font, bool center, int x, int y, Color c, const char *fmt, ...)
{
	wchar_t *pszStringWide = reinterpret_cast< wchar_t* >(malloc((strlen(fmt) + 1) * sizeof(wchar_t)));

	mbstowcs(pszStringWide, fmt, (strlen(fmt) + 1) * sizeof(wchar_t));

	TextW(center, font, x, y, c, pszStringWide);

	free(pszStringWide);
}

void DrawT(int X, int Y, Color Color, int Font, bool Center, char* _Input, ...)
{
	char Buffer[1024] = { '\0' };

	/* set up varargs*/
	va_list Args;

	va_start(Args, _Input);
	vsprintf_s(Buffer, _Input, Args);
	va_end(Args);

	size_t Size = strlen(Buffer) + 1;

	/*

	up widebuffer*/
	wchar_t* WideBuffer = new wchar_t[Size];

	/* char -> wchar */
	mbstowcs_s(nullptr, WideBuffer, Size, Buffer, Size - 1);

	/* check center */
	int Width = 0, Height = 0;

	if (Center)
	{
		g_VGuiSurface->GetTextSize(Font, WideBuffer, Width, Height);
	}

	int r = 255, g = 255, b = 255, a = 255;
	Color.GetColor(r, g, b, a);

	/* call and draw*/
	g_VGuiSurface->DrawSetTextColor(r, g, b, a);
	g_VGuiSurface->DrawSetTextFont(Font);
	g_VGuiSurface->DrawSetTextPos(X - (Width / 2), Y);
	g_VGuiSurface->DrawPrintText(WideBuffer, wcslen(WideBuffer));

	return;
}




//--------------------------------------------------------------------------------
bool Visuals::CreateFonts()
{
	watermark_font = g_VGuiSurface->CreateFont_();
	esp_font = g_VGuiSurface->CreateFont_();
	defuse_font = g_VGuiSurface->CreateFont_();
	dropped_weapons_font = g_VGuiSurface->CreateFont_();
	spectator_list_font = g_VGuiSurface->CreateFont_();
	weapon_icon = g_VGuiSurface->CreateFont_();

	g_VGuiSurface->SetFontGlyphSet(watermark_font, "Arial", 13, 700, 0, 0, FONTFLAG_DROPSHADOW);
	g_VGuiSurface->SetFontGlyphSet(spectator_list_font, "Arial", 16, 700, 0, 0, FONTFLAG_OUTLINE, FONTFLAG_ANTIALIAS);
	g_VGuiSurface->SetFontGlyphSet(esp_font, "Tahoma", 13, 350, 0, 0, FONTFLAG_OUTLINE, FONTFLAG_ANTIALIAS);
	g_VGuiSurface->SetFontGlyphSet(defuse_font, "Tahoma", 12, 350, 0, 0, FONTFLAG_OUTLINE, FONTFLAG_ANTIALIAS);
	g_VGuiSurface->SetFontGlyphSet(dropped_weapons_font, "Arial", 10, 700, 0, 0, FONTFLAG_DROPSHADOW);
	g_VGuiSurface->SetFontGlyphSet(weapon_icon, "Counter-Strike", 22, 450, 0, 0, FONTFLAG_ANTIALIAS);

	return true;
}
//--------------------------------------------------------------------------------
void Visuals::DestroyFonts()
{
	// Is there a way to destroy vgui fonts?
	// TODO: Find out
}

#define Assert( _exp ) ((void)0)

void AngleVectors(const Vector &angles, Vector *forward)
{
	Assert(s_bMathlibInitialized);
	Assert(forward);

	float	sp, sy, cp, cy;

	sy = sin(DEG2RAD(angles[1]));
	cy = cos(DEG2RAD(angles[1]));

	sp = sin(DEG2RAD(angles[0]));
	cp = cos(DEG2RAD(angles[0]));

	forward->x = cp*cy;
	forward->y = cp*sy;
	forward->z = -sp;
}




void Visuals::Draw3dCube(float scalar, QAngle angles, Vector middle_origin, Color outline)
{
	Vector forward, right, up;
	Math::AngleVectors(angles, forward, right, up);

	Vector points[8];
	points[0] = middle_origin - (right * scalar) + (up * scalar) - (forward * scalar); // BLT
	points[1] = middle_origin + (right * scalar) + (up * scalar) - (forward * scalar); // BRT
	points[2] = middle_origin - (right * scalar) - (up * scalar) - (forward * scalar); // BLB
	points[3] = middle_origin + (right * scalar) - (up * scalar) - (forward * scalar); // BRB

	points[4] = middle_origin - (right * scalar) + (up * scalar) + (forward * scalar); // FLT
	points[5] = middle_origin + (right * scalar) + (up * scalar) + (forward * scalar); // FRT
	points[6] = middle_origin - (right * scalar) - (up * scalar) + (forward * scalar); // FLB
	points[7] = middle_origin + (right * scalar) - (up * scalar) + (forward * scalar); // FRB

	Vector points_screen[8];
	for (int i = 0; i < 8; i++)
		if (!Math::WorldToScreen(points[i], points_screen[i]))
			return;

	g_VGuiSurface->DrawSetColor(outline);

	// Back frame
	g_VGuiSurface->DrawLine(points_screen[0].x, points_screen[0].y, points_screen[1].x, points_screen[1].y);
	g_VGuiSurface->DrawLine(points_screen[0].x, points_screen[0].y, points_screen[2].x, points_screen[2].y);
	g_VGuiSurface->DrawLine(points_screen[3].x, points_screen[3].y, points_screen[1].x, points_screen[1].y);
	g_VGuiSurface->DrawLine(points_screen[3].x, points_screen[3].y, points_screen[2].x, points_screen[2].y);

	// Frame connector
	g_VGuiSurface->DrawLine(points_screen[0].x, points_screen[0].y, points_screen[4].x, points_screen[4].y);
	g_VGuiSurface->DrawLine(points_screen[1].x, points_screen[1].y, points_screen[5].x, points_screen[5].y);
	g_VGuiSurface->DrawLine(points_screen[2].x, points_screen[2].y, points_screen[6].x, points_screen[6].y);
	g_VGuiSurface->DrawLine(points_screen[3].x, points_screen[3].y, points_screen[7].x, points_screen[7].y);

	// Front frame
	g_VGuiSurface->DrawLine(points_screen[4].x, points_screen[4].y, points_screen[5].x, points_screen[5].y);
	g_VGuiSurface->DrawLine(points_screen[4].x, points_screen[4].y, points_screen[6].x, points_screen[6].y);
	g_VGuiSurface->DrawLine(points_screen[7].x, points_screen[7].y, points_screen[5].x, points_screen[5].y);
	g_VGuiSurface->DrawLine(points_screen[7].x, points_screen[7].y, points_screen[6].x, points_screen[6].y);
}


//--------------------------------------------------------------------------------
bool Visuals::Player::Begin(C_BasePlayer* pl)
{
	esp_ctx.pl = pl;
	esp_ctx.is_enemy = g_LocalPlayer->m_iTeamNum() != pl->m_iTeamNum();
	esp_ctx.is_visible = g_LocalPlayer->CanSeePlayer(pl, HITBOX_CHEST);

	if (!esp_ctx.is_enemy && g_Options.esp_enemies_only)
		return false;

	esp_ctx.clr = esp_ctx.is_enemy ? (esp_ctx.is_visible ? g_Options.color_esp_enemy_visible : g_Options.color_esp_enemy_occluded) : (esp_ctx.is_visible ? g_Options.color_esp_ally_visible : g_Options.color_esp_ally_occluded);
	esp_ctx.filled = g_Options.color_esp_box_filled;

	Vector head = pl->GetRenderOrigin() + Vector(0, 0, pl->GetCollideable()->OBBMaxs().z);
	Vector origin = pl->GetRenderOrigin();
	origin.z -= 5;

	if (!Math::WorldToScreen(head, esp_ctx.head_pos) ||
		!Math::WorldToScreen(origin, esp_ctx.feet_pos))
		return false;

	auto h = fabs(esp_ctx.head_pos.y - esp_ctx.feet_pos.y);
	auto w = h / 1.65f;

	Vector points_transformed[8];
	RECT BBox = GetBBox(pl, points_transformed);
	esp_ctx.bbox = BBox;
	esp_ctx.bbox.top = BBox.bottom;
	esp_ctx.bbox.bottom = BBox.top;

	return true;
}

void Visuals::Player::RenderFill()
{
	g_VGuiSurface->DrawSetColor(esp_ctx.filled);
	g_VGuiSurface->DrawFilledRect(esp_ctx.bbox.left + 2, esp_ctx.bbox.top + 2, esp_ctx.bbox.right - 2, esp_ctx.bbox.bottom - 2);
}

void Visuals::ThreeDBox(Vector minin, Vector maxin, Vector pos, Color col)
{
	Vector min = minin + pos;
	Vector max = maxin + pos;

	Vector corners[] = { Vector(min.x, min.y, min.z),
		Vector(min.x, max.y, min.z),
		Vector(max.x, max.y, min.z),
		Vector(max.x, min.y, min.z),
		Vector(min.x, min.y, max.z),
		Vector(min.x, max.y, max.z),
		Vector(max.x, max.y, max.z),
		Vector(max.x, min.y, max.z) };


	int edges[12][2] = { { 0, 1 },{ 1, 2 },{ 2, 3 },{ 3, 0 },{ 4, 5 },{ 5, 6 },{ 6, 7 },{ 7, 4 },{ 0, 4 },{ 1, 5 },{ 2, 6 },{ 3, 7 }, };

	for (const auto edge : edges)
	{
		Vector p1, p2;
		if (!Math::WorldToScreen(corners[edge[0]], p1) || !Math::WorldToScreen(corners[edge[1]], p2))
			return;
		int red = 0;
		int green = 0;
		int blue = 0;
		int alpha = 0;

		g_VGuiSurface->DrawSetColor(col);
		g_VGuiSurface->DrawLine(p1.x, p1.y, p2.x, p2.y);
	}
}


void Visuals::Player::RenderWeapon()
{
	wchar_t buf[80];
	auto clean_item_name = [](const char *name) -> const char*
	{
		if (name[0] == 'C')
			name++;

		auto start = strstr(name, "Weapon");
		if (start != nullptr)
			name = start + 6;

		return name;
	};

	auto weapon = esp_ctx.pl->m_hActiveWeapon().Get();

	if (!weapon) return;

	if (weapon->m_hOwnerEntity().IsValid())
	{
		auto name = clean_item_name(weapon->GetClientClass()->m_pNetworkName);

		if (MultiByteToWideChar(CP_UTF8, 0, name, -1, buf, 80) > 0)
		{
			int tw, th;
			g_VGuiSurface->GetTextSize(esp_font, buf, tw, th);

			g_VGuiSurface->DrawSetTextFont(esp_font);
			g_VGuiSurface->DrawSetTextColor(Color(g_Options.color_esp_pl_weapons));
			g_VGuiSurface->DrawSetTextPos(esp_ctx.bbox.left + (esp_ctx.bbox.right - esp_ctx.bbox.left) * 0.5 - tw * 0.5, esp_ctx.bbox.bottom + 5.5);
			g_VGuiSurface->DrawPrintText(buf, wcslen(buf));
		}
	}
}
//--------------------------------------------------------------------------------
void Visuals::Player::RenderBox(C_BaseEntity* ent)
{

	float
		length_horizontal = (esp_ctx.bbox.right - esp_ctx.bbox.left) * 0.2f,
		length_vertical = (esp_ctx.bbox.bottom - esp_ctx.bbox.top) * 0.2f;

	switch (g_Options.esp_boxtype)
	{
	case 0:
		break;
	case 1:
		g_VGuiSurface->DrawSetColor(esp_ctx.clr);
		g_VGuiSurface->DrawOutlinedRect(esp_ctx.bbox.left, esp_ctx.bbox.top, esp_ctx.bbox.right, esp_ctx.bbox.bottom);
		g_VGuiSurface->DrawSetColor(Color::Black);
		g_VGuiSurface->DrawOutlinedRect(esp_ctx.bbox.left - 1, esp_ctx.bbox.top - 1, esp_ctx.bbox.right + 1, esp_ctx.bbox.bottom + 1);
		g_VGuiSurface->DrawOutlinedRect(esp_ctx.bbox.left + 1, esp_ctx.bbox.top + 1, esp_ctx.bbox.right - 1, esp_ctx.bbox.bottom - 1);
		break;
	case 2:
		g_VGuiSurface->DrawSetColor(Color::Black);
		g_VGuiSurface->DrawFilledRect(esp_ctx.bbox.left - 1, esp_ctx.bbox.top - 1, esp_ctx.bbox.left + 1 + length_horizontal, esp_ctx.bbox.top + 2);
		g_VGuiSurface->DrawFilledRect(esp_ctx.bbox.right - 1 - length_horizontal, esp_ctx.bbox.top - 1, esp_ctx.bbox.right + 1, esp_ctx.bbox.top + 2);
		g_VGuiSurface->DrawFilledRect(esp_ctx.bbox.left - 1, esp_ctx.bbox.bottom - 2, esp_ctx.bbox.left + 1 + length_horizontal, esp_ctx.bbox.bottom + 1);
		g_VGuiSurface->DrawFilledRect(esp_ctx.bbox.right - 1 - length_horizontal, esp_ctx.bbox.bottom - 2, esp_ctx.bbox.right + 1, esp_ctx.bbox.bottom + 1);

		g_VGuiSurface->DrawFilledRect(esp_ctx.bbox.left - 1, esp_ctx.bbox.top + 2, esp_ctx.bbox.left + 2, esp_ctx.bbox.top + 1 + length_vertical);
		g_VGuiSurface->DrawFilledRect(esp_ctx.bbox.right - 2, esp_ctx.bbox.top + 2, esp_ctx.bbox.right + 1, esp_ctx.bbox.top + 1 + length_vertical);
		g_VGuiSurface->DrawFilledRect(esp_ctx.bbox.left - 1, esp_ctx.bbox.bottom - 1 - length_vertical, esp_ctx.bbox.left + 2, esp_ctx.bbox.bottom - 2);
		g_VGuiSurface->DrawFilledRect(esp_ctx.bbox.right - 2, esp_ctx.bbox.bottom - 1 - length_vertical, esp_ctx.bbox.right + 1, esp_ctx.bbox.bottom - 2);

		g_VGuiSurface->DrawSetColor(esp_ctx.clr);
		g_VGuiSurface->DrawLine(esp_ctx.bbox.left, esp_ctx.bbox.top, esp_ctx.bbox.left + length_horizontal - 1, esp_ctx.bbox.top);
		g_VGuiSurface->DrawLine(esp_ctx.bbox.right - length_horizontal, esp_ctx.bbox.top, esp_ctx.bbox.right - 1, esp_ctx.bbox.top);
		g_VGuiSurface->DrawLine(esp_ctx.bbox.left, esp_ctx.bbox.bottom - 1, esp_ctx.bbox.left + length_horizontal - 1, esp_ctx.bbox.bottom - 1);
		g_VGuiSurface->DrawLine(esp_ctx.bbox.right - length_horizontal, esp_ctx.bbox.bottom - 1, esp_ctx.bbox.right - 1, esp_ctx.bbox.bottom - 1);

		g_VGuiSurface->DrawLine(esp_ctx.bbox.left, esp_ctx.bbox.top, esp_ctx.bbox.left, esp_ctx.bbox.top + length_vertical - 1);
		g_VGuiSurface->DrawLine(esp_ctx.bbox.right - 1, esp_ctx.bbox.top, esp_ctx.bbox.right - 1, esp_ctx.bbox.top + length_vertical - 1);
		g_VGuiSurface->DrawLine(esp_ctx.bbox.left, esp_ctx.bbox.bottom - length_vertical, esp_ctx.bbox.left, esp_ctx.bbox.bottom - 1);
		g_VGuiSurface->DrawLine(esp_ctx.bbox.right - 1, esp_ctx.bbox.bottom - length_vertical, esp_ctx.bbox.right - 1, esp_ctx.bbox.bottom - 1);
		break;
	case 3:
		ThreeDBox(ent->GetCollideable()->OBBMins(), ent->GetCollideable()->OBBMaxs(), ent->GetRenderOrigin(), esp_ctx.clr);

	}



}


//--------------------------------------------------------------------------------

static std::string GetTimeString()
{
	time_t current_time;
	struct tm *time_info;
	static char timeString[10];
	time(&current_time);
	time_info = localtime(&current_time);
	strftime(timeString, sizeof(timeString), "%X", time_info);
	return timeString;
}

void Visuals::Player::RenderWaterMark()
{

	const wchar_t* watermark = L"????";



	static float rainbow;
	rainbow += 0.0008f;
	if (rainbow > 1.f)
		rainbow = 0.f;

	DrawString(watermark_font, 10, 30, Color(200, 200, 200, 255), FONT_LEFT, "Local Time: %s", GetTimeString().c_str());
	//	DrawString(watermark_font, 10, 47, Color(200, 200, 200, 255), FONT_LEFT, "FPS: %03d", get_fps());

	g_VGuiSurface->DrawSetTextFont(watermark_font);
	g_VGuiSurface->DrawSetTextColor(Color::FromHSB(rainbow, 1.f, 1.f));
	g_VGuiSurface->DrawSetTextPos(10, 10);
	g_VGuiSurface->DrawPrintText(watermark, wcslen(watermark));



}

void Visuals::RecoilCrosshair()
{
	int w, h;

	g_EngineClient->GetScreenSize(w, h);


	g_VGuiSurface->DrawSetColor(Color(255,0,0));   //<-- I am too lazy to make personal color for it.Yellow looks like good color

	int x = w / 2;
	int y = h / 2;
	int dy = h / 97;
	int dx = w / 97;

	QAngle punchAngle = g_LocalPlayer->m_aimPunchAngle();
	x -= (dx*(punchAngle.yaw));
	y += (dy*(punchAngle.pitch));


	g_VGuiSurface->DrawLine(x - 8, y, x + 8, y); //If you want to make the PunchCross bigger or smaller just change the numbers
	g_VGuiSurface->DrawLine(x, y - 8, x, y + 8);
}


void Visuals::Player::Skeleton()
{
	studiohdr_t *studioHdr = g_MdlInfo->GetStudioModel(esp_ctx.pl->GetModel());
	if (studioHdr)
	{
		static matrix3x4_t boneToWorldOut[128];
		if (esp_ctx.pl->SetupBones(boneToWorldOut, MAXSTUDIOBONES, BONE_USED_BY_HITBOX, g_GlobalVars->curtime))
		{
			for (int i = 0; i < studioHdr->numbones; i++)
			{
				mstudiobone_t *bone = studioHdr->pBone(i);
				if (!bone || !(bone->flags & BONE_USED_BY_HITBOX) || bone->parent == -1)
					continue;

				Vector bonePos1;
				if (!Math::WorldToScreen(Vector(boneToWorldOut[i][0][3], boneToWorldOut[i][1][3], boneToWorldOut[i][2][3]), bonePos1))
					continue;

				Vector bonePos2;
				if (!Math::WorldToScreen(Vector(boneToWorldOut[bone->parent][0][3], boneToWorldOut[bone->parent][1][3], boneToWorldOut[bone->parent][2][3]), bonePos2))
					continue;

				g_VGuiSurface->DrawSetColor(Color(g_Options.color_esp_skeleton));
				g_VGuiSurface->DrawLine((int)bonePos1.x, (int)bonePos1.y, (int)bonePos2.x, (int)bonePos2.y);
			}
		}
	}
}




void Visuals::Player::RenderName()
{
	wchar_t buf[128];
	std::string name = esp_ctx.pl->GetName(),
		s_name = (name.length() > 0 ? name : "##ERROR_empty_name");

	if (MultiByteToWideChar(CP_UTF8, 0, s_name.c_str(), -1, buf, 128) > 0)
	{
		int tw, th;
		g_VGuiSurface->GetTextSize(esp_font, buf, tw, th);

		g_VGuiSurface->DrawSetTextFont(esp_font);
		g_VGuiSurface->DrawSetTextColor(Color(g_Options.color_esp_name));
		g_VGuiSurface->DrawSetTextPos(esp_ctx.bbox.left + (esp_ctx.bbox.right - esp_ctx.bbox.left) * 0.5 - tw * 0.5, esp_ctx.bbox.top - th + 1);
		g_VGuiSurface->DrawPrintText(buf, wcslen(buf));
	}
}
//--------------------------------------------------------------------------------
void Visuals::Player::RenderHealth()
{
	int health = esp_ctx.pl->m_iHealth();
	if (health > 100)
		health = 100;

	float box_h = (float)fabs(esp_ctx.bbox.bottom - esp_ctx.bbox.top);
	float off = 6;

	auto height = box_h - (((box_h * health) / 100));

	int green = int(health * 2.55f);
	int red = 255 - green;

	int x = esp_ctx.bbox.left - off;
	int y = esp_ctx.bbox.top;
	int w = 4;
	int h = box_h;

	//DrawhealthIcon(x - 12, y, ESP_ctx.player);

	g_VGuiSurface->DrawSetColor(Color::Black);
	g_VGuiSurface->DrawFilledRect(x, y, x + w, y + h);

	g_VGuiSurface->DrawSetColor(Color(red, green, 0, 255));
	g_VGuiSurface->DrawOutlinedRect(x + 1, y + height + 1, x + w - 1, y + h - 1);
}

void Visuals::EdgyHealthBar()
{
	float bar = 0;
	bar++;
	float HealthValue = esp_ctx.pl->m_iHealth();
	int iHealthValue = HealthValue;
	int Red = 255 - (HealthValue * 2.00);
	int Green = HealthValue * 2.00;
	float flBoxes = std::ceil(esp_ctx.pl->m_iHealth() / 10.f);

	float height = (esp_ctx.bbox.bottom - esp_ctx.bbox.top) * (HealthValue / 100);
	float height2 = (esp_ctx.bbox.bottom - esp_ctx.bbox.top) * (100 / 100); // used for the styles like healthbar lines
	float flHeight = height2 / 10.f;

	float off = 3;
	int x = esp_ctx.bbox.left - off;

	g_VGuiSurface->DrawSetColor(Color::Black);
	g_VGuiSurface->DrawFilledRect(x - 5, esp_ctx.bbox.top - 1, x - 1, esp_ctx.bbox.bottom + 1);
	g_VGuiSurface->DrawSetColor(Color(Red, Green, 0, 255));
	g_VGuiSurface->DrawFilledRect(x - 4, esp_ctx.bbox.bottom - height, x - 2, esp_ctx.bbox.bottom);

	for (int i = 0; i < 10; i++)
	{
		g_VGuiSurface->DrawSetColor(Color::Black);
		g_VGuiSurface->DrawLine(x - 5, esp_ctx.bbox.top + i * flHeight, x - 2, esp_ctx.bbox.top + i * flHeight);
	}

	int y = esp_ctx.bbox.top;

	auto  hp = esp_ctx.pl->m_iHealth();
	if (g_Options.esp_health_text)
	{
		if (hp > 0)
			DrawT(x + -23, y - 0.5, Color(255, 255, 255, 255), defuse_font, true, "%i HP", hp);
	}

}

void Visuals::AngleLines()
{
	Vector src3D, dst3D, forward, src, dst;
	trace_t tr;
	Ray_t ray;
	CTraceFilter filter;

	filter.pSkip = esp_ctx.pl;
	Math::AngleVectors(QAngle(0, esp_ctx.pl->m_flLowerBodyYawTarget(), 0), forward);
	src3D = esp_ctx.pl->m_vecOrigin();
	dst3D = src3D + (forward * 50.f);

	ray.Init(src3D, dst3D);
	g_EngineTrace->TraceRay(ray, 0, &filter, &tr);

	if (!Math::WorldToScreen(src3D, src) || !Math::WorldToScreen(tr.endpos, dst))
		return;

	g_VGuiSurface->DrawSetColor(Color::Green);
	g_VGuiSurface->DrawLine(src.x, src.y, dst.x, dst.y);


	Math::AngleVectors(QAngle(0, esp_ctx.pl->m_angEyeAngles().yaw, 0), forward);
	dst3D = src3D + (forward * 50.f);

	ray.Init(src3D, dst3D);
	g_EngineTrace->TraceRay(ray, 0, &filter, &tr);

	if (!Math::WorldToScreen(src3D, src) || !Math::WorldToScreen(tr.endpos, dst))
		return;

	g_VGuiSurface->DrawSetColor(Color::Blue);
	g_VGuiSurface->DrawLine(src.x, src.y, dst.x, dst.y);
}



//--------------------------------------------------------------------------------
void Visuals::Player::RenderArmour()
{
	auto  armour = esp_ctx.pl->m_ArmorValue();
	float box_h = (float)fabs(esp_ctx.bbox.right - esp_ctx.bbox.left);
	//float off = (box_h / 6.f) + 5;
	float off = 4;

	auto height = (((box_h * armour) / 100));

	int x = esp_ctx.bbox.left + off;
	int y = esp_ctx.bbox.bottom;
	int w = 4;
	int h = box_h;

	g_VGuiSurface->DrawSetColor(Color::Black);
	g_VGuiSurface->DrawFilledRect(esp_ctx.bbox.left - 1, esp_ctx.bbox.bottom + 2, esp_ctx.bbox.right + 1, esp_ctx.bbox.bottom + 5);
	//g_VGuiSurface->DrawFilledRect(x, y, x + w, y + h);
	g_VGuiSurface->DrawSetColor(Color(0, 97, 255, 255));
	//g_VGuiSurface->DrawFilledRect(esp_ctx.bbox.left + 5, esp_ctx.bbox.bottom + 2, esp_ctx.bbox.left + w - 1, y + height - 2);
	g_VGuiSurface->DrawFilledRect(esp_ctx.bbox.left, esp_ctx.bbox.bottom + 2, esp_ctx.bbox.left + height, esp_ctx.bbox.bottom + 4);

}
//--------------------------------------------------------------------------------





//--------------------------------------------------------------------------------
void Visuals::Player::RenderSnapline()
{
	int screen_w, screen_h;
	g_EngineClient->GetScreenSize(screen_w, screen_h);

	g_VGuiSurface->DrawSetColor(esp_ctx.clr);

	g_VGuiSurface->DrawLine(
		screen_w / 2,
		screen_h,
		esp_ctx.feet_pos.x,
		esp_ctx.feet_pos.y);
}
//--------------------------------------------------------------------------------
void Visuals::Misc::RenderCrosshair()
{
	int w, h;

	g_EngineClient->GetScreenSize(w, h);

	g_VGuiSurface->DrawSetColor(g_Options.color_esp_crosshair);

	int cx = w / 2;
	int cy = h / 2;

	g_VGuiSurface->DrawLine(cx - 25, cy, cx + 25, cy);
	g_VGuiSurface->DrawLine(cx, cy - 25, cx, cy + 25);
}

void Visuals::AsusWalls() {
	if (g_EngineClient->IsConnected())
	{

		static auto staticdrop = g_CVar->FindVar("r_DrawSpecificStaticProp");
		staticdrop->SetValue(0);

		for (auto i = g_MatSystem->FirstMaterial(); i != g_MatSystem->InvalidMaterial(); i = g_MatSystem->NextMaterial(i))
		{
			IMaterial *pMaterial = g_MatSystem->GetMaterial(i);

			if (!pMaterial)
				continue;
			if (strstr(pMaterial->GetMaterialName(), "crate") || strstr(pMaterial->GetMaterialName(), "box") || strstr(pMaterial->GetMaterialName(), "door")) {
				pMaterial->AlphaModulate(0.9f);
				pMaterial->SetMaterialVarFlag(MATERIAL_VAR_MODEL, true);
			}
		}

	}
}

void Visuals::SkyColor()
{  
	for (MaterialHandle_t i = g_MatSystem->FirstMaterial(); i != g_MatSystem->InvalidMaterial(); i = g_MatSystem->NextMaterial(i))
	{
		IMaterial *pMaterial = g_MatSystem->GetMaterial(i);

		if (!pMaterial)
			continue;

		const char* group = pMaterial->GetTextureGroupName();
		const char* name = pMaterial->GetName();

		if (strstr(pMaterial->GetTextureGroupName(), "SkyBox"))
		{
			
		}
	}

}

void Visuals::NightMode()
{
	bool done = false;
	if (g_Options.nightmode)
	{
		static auto sv_skyname = g_CVar->FindVar("sv_skyname");
		static auto r_DrawSpecificStaticProp = g_CVar->FindVar("r_DrawSpecificStaticProp");
		r_DrawSpecificStaticProp->SetValue(1);
		sv_skyname->SetValue("sky_csgo_night02b");

		for (MaterialHandle_t i = g_MatSystem->FirstMaterial(); i != g_MatSystem->InvalidMaterial(); i = g_MatSystem->NextMaterial(i))
		{
			IMaterial *pMaterial = g_MatSystem->GetMaterial(i);

			if (!pMaterial)
				continue;

			const char* group = pMaterial->GetTextureGroupName();
			const char* name = pMaterial->GetName();

			if (strstr(pMaterial->GetTextureGroupName(), "SkyBox"))
			{
				pMaterial->ColorModulate(g_Options.sky_color[0] * 1, g_Options.sky_color[1] * 1, g_Options.sky_color[2] * 1);
			}
			if (strstr(group, "World textures"))
			{
				pMaterial->ColorModulate(g_Options.nightmode_intensity, g_Options.nightmode_intensity, g_Options.nightmode_intensity);
			}
			if (strstr(group, "StaticProp"))
			{
				pMaterial->ColorModulate(g_Options.nightmode_intensity, g_Options.nightmode_intensity, g_Options.nightmode_intensity);
			}
			if (strstr(name, "models/props/de_dust/palace_bigdome"))
			{
				pMaterial->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, true);
			}
			if (strstr(name, "models/props/de_dust/palace_pillars"))
			{
				pMaterial->ColorModulate(g_Options.nightmode_intensity, g_Options.nightmode_intensity, g_Options.nightmode_intensity);
			}

			if (strstr(group, "Particle textures"))
			{
				pMaterial->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, true);
			}


			done = true;
		}
	}
	else
	{
		for (MaterialHandle_t i = g_MatSystem->FirstMaterial(); i != g_MatSystem->InvalidMaterial(); i = g_MatSystem->NextMaterial(i))
		{
			IMaterial *pMaterial = g_MatSystem->GetMaterial(i);

			if (!pMaterial)
				continue;

			const char* group = pMaterial->GetTextureGroupName();
			const char* name = pMaterial->GetName();

			if (strstr(pMaterial->GetTextureGroupName(), "SkyBox"))
			{
				pMaterial->ColorModulate(1, 1, 1);
			}
			if (strstr(group, "World textures"))
			{

				pMaterial->ColorModulate(1, 1, 1);
			}
			if (strstr(group, "StaticProp"))
			{

				pMaterial->ColorModulate(1, 1, 1);
			}
			if (strstr(name, "models/props/de_dust/palace_bigdome"))
			{
				pMaterial->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, false);
			}
			if (strstr(name, "models/props/de_dust/palace_pillars"))
			{

				pMaterial->ColorModulate(1, 1, 1);
			}
			if (strstr(group, "Particle textures"))
			{
				pMaterial->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, false);
			}
		}


	}

}

void Visuals::DLight(C_BaseEntity* entity)
{
	player_info_t pinfo;
	if (esp_ctx.pl && entity && entity != esp_ctx.pl)
	{
		if (g_EngineClient->GetPlayerInfo(entity->EntIndex(), &pinfo) && esp_ctx.pl->IsAlive() && !entity->IsDormant())
		{

			dlight_t* pElight = g_Dlight->CL_AllocElight(entity->EntIndex());
			pElight->origin = entity->m_vecOrigin() + Vector(0.0f, 0.0f, 35.0f);
			pElight->radius = 25.0f;
			pElight->color.r = static_cast<int>(g_Options.DLight_color[0] * 255.f);
			pElight->color.g = static_cast<int>(g_Options.DLight_color[1] * 255.f);
			pElight->color.b = static_cast<int>(g_Options.DLight_color[2] * 255.f);
			pElight->color.exponent = 10;
			pElight->die = g_GlobalVars->curtime + 0.05f;
			pElight->decay = pElight->radius / 5.0f;
			pElight->key = entity->EntIndex();


			dlight_t* pDlight = g_Dlight->CL_AllocDlight(entity->EntIndex());
			pDlight->origin = entity->m_vecOrigin();
			pDlight->radius = 25.0f;
			pDlight->color.r = static_cast<int>(g_Options.DLight_color[0] * 255.f);
			pDlight->color.g = static_cast<int>(g_Options.DLight_color[1] * 255.f);
			pDlight->color.b = static_cast<int>(g_Options.DLight_color[2] * 255.f);
			pDlight->color.exponent = 10;
			pDlight->die = g_GlobalVars->curtime + 0.05f;
			pDlight->decay = pDlight->radius / 5.0f;
			pDlight->key = entity->EntIndex();

		}
	}
}

void Visuals::LagCompHitbox(int index)
{
	float duration = g_Options.visuals_lagcomp_duration;

	bool playerTeam = esp_ctx.pl->m_iTeamNum() == 2;

	if (index < 0)
		return;

	Color(140, 0, 68);

	auto entity = reinterpret_cast<C_BasePlayer*>(g_EntityList->GetClientEntity(index));

	if (!entity)
		return;

	studiohdr_t* pStudioModel = g_MdlInfo->GetStudioModel(entity->GetModel());

	if (!pStudioModel)
		return;

	static matrix3x4_t pBoneToWorldOut[128];

	if (!entity->SetupBones(pBoneToWorldOut, MAXSTUDIOBONES, 256, g_GlobalVars->curtime))
		return;

	mstudiohitboxset_t* pHitboxSet = pStudioModel->GetHitboxSet(0);
	if (!pHitboxSet)
		return;

	for (int i = 0; i < pHitboxSet->numhitboxes; i++)
	{
		mstudiobbox_t* pHitbox = pHitboxSet->GetHitbox(i);
		if (!pHitbox)
			continue;

		Vector vMin, vMax;
		Math::VectorTransform(pHitbox->bbmin, pBoneToWorldOut[pHitbox->bone], vMin); //nullptr???
		Math::VectorTransform(pHitbox->bbmax, pBoneToWorldOut[pHitbox->bone], vMax);

		if (pHitbox->m_flRadius > -1)
		{
			g_DebugOverlay->AddCapsuleOverlay(vMin, vMax, pHitbox->m_flRadius, 160, 0, 76, 100, duration);
		}
	}

}

//--------------------------------------------------------------------------------
void Visuals::Misc::RenderWeapon(C_BaseCombatWeapon* ent)
{
	wchar_t buf[80];
	auto clean_item_name = [](const char* name) -> const char* {
		if (name[0] == 'C')
			name++;

		auto start = strstr(name, "Weapon");
		if (start != nullptr)
			name = start + 6;

		return name;
	};

	// We don't want to Render weapons that are being held
	if (ent->m_hOwnerEntity().IsValid())
		return;

	Vector pointsTransformed[8];
	auto bbox = GetBBox(ent, pointsTransformed);

	

	

	if (bbox.right == 0 || bbox.bottom == 0)
		return;

	float
		length_horizontal = (bbox.right - bbox.left) * 0.2f,
		length_vertical = (bbox.bottom - bbox.top) * 0.2f;

	switch (g_Options.esp_dropped_weapons)
	{
	case 0: //off
		break;
	case 1: //bounding box
		g_VGuiSurface->DrawSetColor(g_Options.color_esp_weapons);
		g_VGuiSurface->DrawLine(bbox.left, bbox.top, bbox.right, bbox.top);
		g_VGuiSurface->DrawLine(bbox.left, bbox.bottom, bbox.right, bbox.bottom);
		g_VGuiSurface->DrawLine(bbox.left, bbox.top, bbox.left, bbox.bottom);
		g_VGuiSurface->DrawLine(bbox.right, bbox.top, bbox.right, bbox.bottom);
		break;
	case 2:
		g_VGuiSurface->DrawSetColor(g_Options.color_esp_weapons);
		g_VGuiSurface->DrawLine(bbox.left, bbox.top, bbox.left + length_horizontal - 1, bbox.top);
		g_VGuiSurface->DrawLine(bbox.right - length_horizontal, bbox.top, bbox.right - 1, bbox.top);
		g_VGuiSurface->DrawLine(bbox.left, bbox.bottom - 1, bbox.left + length_horizontal - 1, bbox.bottom - 1);
		g_VGuiSurface->DrawLine(bbox.right - length_horizontal, bbox.bottom - 1, bbox.right - 1, bbox.bottom - 1);

		g_VGuiSurface->DrawLine(bbox.left,bbox.top, bbox.left, bbox.top + length_vertical - 1);
		g_VGuiSurface->DrawLine(bbox.right - 1, bbox.top, bbox.right - 1, bbox.top + length_vertical - 1);
		g_VGuiSurface->DrawLine(bbox.left, bbox.bottom - length_vertical, bbox.left, bbox.bottom - 1);
		g_VGuiSurface->DrawLine(bbox.right - 1, bbox.bottom - length_vertical, bbox.right - 1, bbox.bottom - 1);
		break;

		
		break;
	case 3:
		g_VGuiSurface->DrawSetColor(g_Options.color_esp_weapons);
		g_VGuiSurface->DrawLine(pointsTransformed[0].x, pointsTransformed[0].y, pointsTransformed[1].x, pointsTransformed[1].y);
		g_VGuiSurface->DrawLine(pointsTransformed[0].x, pointsTransformed[0].y, pointsTransformed[6].x, pointsTransformed[6].y);
		g_VGuiSurface->DrawLine(pointsTransformed[1].x, pointsTransformed[1].y, pointsTransformed[5].x, pointsTransformed[5].y);
		g_VGuiSurface->DrawLine(pointsTransformed[6].x, pointsTransformed[6].y, pointsTransformed[5].x, pointsTransformed[5].y);

		g_VGuiSurface->DrawLine(pointsTransformed[2].x, pointsTransformed[2].y, pointsTransformed[1].x, pointsTransformed[1].y);
		g_VGuiSurface->DrawLine(pointsTransformed[4].x, pointsTransformed[4].y, pointsTransformed[5].x, pointsTransformed[5].y);
		g_VGuiSurface->DrawLine(pointsTransformed[6].x, pointsTransformed[6].y, pointsTransformed[7].x, pointsTransformed[7].y);
		g_VGuiSurface->DrawLine(pointsTransformed[3].x, pointsTransformed[3].y, pointsTransformed[0].x, pointsTransformed[0].y);

		g_VGuiSurface->DrawLine(pointsTransformed[3].x, pointsTransformed[3].y, pointsTransformed[2].x, pointsTransformed[2].y);
		g_VGuiSurface->DrawLine(pointsTransformed[2].x, pointsTransformed[2].y, pointsTransformed[4].x, pointsTransformed[4].y);
		g_VGuiSurface->DrawLine(pointsTransformed[7].x, pointsTransformed[7].y, pointsTransformed[4].x, pointsTransformed[4].y);
		g_VGuiSurface->DrawLine(pointsTransformed[7].x, pointsTransformed[7].y, pointsTransformed[3].x, pointsTransformed[3].y);
		break;
	}

	

	auto namee = clean_item_name(ent->GetClientClass()->m_pNetworkName);

	if (MultiByteToWideChar(CP_UTF8, 0, namee, -1, buf, 80) > 0) {
		int w = bbox.right - bbox.left;
		int tw, th;
		g_VGuiSurface->GetTextSize(esp_font, buf, tw, th);

		g_VGuiSurface->DrawSetTextFont(esp_font);
		g_VGuiSurface->DrawSetTextColor(g_Options.color_esp_weapons);
		g_VGuiSurface->DrawSetTextPos((bbox.left + w * 0.5f) - tw * 0.5f, bbox.bottom + 1);
		g_VGuiSurface->DrawPrintText(buf, wcslen(buf));
	}
}
//--------------------------------------------------------------------------------
void Visuals::Misc::RenderDefuseKit(C_BaseEntity* ent)
{
	if (ent->m_hOwnerEntity().IsValid())
		return;

	Vector pointsTransformed[8];
	auto bbox = GetBBox(ent, pointsTransformed);

	if (bbox.right == 0 || bbox.bottom == 0)
		return;


	float
		length_horizontal = (bbox.right - bbox.left) * 0.2f,
		length_vertical = (bbox.bottom - bbox.top) * 0.2f;

	g_VGuiSurface->DrawSetColor(g_Options.color_esp_defuse);

	switch (g_Options.esp_dropped_Kits)
	{
	case 0: //off
		break;
	case 1: //bounding box
		g_VGuiSurface->DrawLine(bbox.left, bbox.top, bbox.right, bbox.top);
		g_VGuiSurface->DrawLine(bbox.left, bbox.bottom, bbox.right, bbox.bottom);
		g_VGuiSurface->DrawLine(bbox.left, bbox.top, bbox.left, bbox.bottom);
		g_VGuiSurface->DrawLine(bbox.right, bbox.top, bbox.right, bbox.bottom);
		break;
	case 2:
		g_VGuiSurface->DrawLine(bbox.left, bbox.top, bbox.left + length_horizontal - 1, bbox.top);
		g_VGuiSurface->DrawLine(bbox.right - length_horizontal, bbox.top, bbox.right - 1, bbox.top);
		g_VGuiSurface->DrawLine(bbox.left, bbox.bottom - 1, bbox.left + length_horizontal - 1, bbox.bottom - 1);
		g_VGuiSurface->DrawLine(bbox.right - length_horizontal, bbox.bottom - 1, bbox.right - 1, bbox.bottom - 1);

		g_VGuiSurface->DrawLine(bbox.left, bbox.top, bbox.left, bbox.top + length_vertical - 1);
		g_VGuiSurface->DrawLine(bbox.right - 1, bbox.top, bbox.right - 1, bbox.top + length_vertical - 1);
		g_VGuiSurface->DrawLine(bbox.left, bbox.bottom - length_vertical, bbox.left, bbox.bottom - 1);
		g_VGuiSurface->DrawLine(bbox.right - 1, bbox.bottom - length_vertical, bbox.right - 1, bbox.bottom - 1);
		break;


		break;
	case 3:
		g_VGuiSurface->DrawLine(pointsTransformed[0].x, pointsTransformed[0].y, pointsTransformed[1].x, pointsTransformed[1].y);
		g_VGuiSurface->DrawLine(pointsTransformed[0].x, pointsTransformed[0].y, pointsTransformed[6].x, pointsTransformed[6].y);
		g_VGuiSurface->DrawLine(pointsTransformed[1].x, pointsTransformed[1].y, pointsTransformed[5].x, pointsTransformed[5].y);
		g_VGuiSurface->DrawLine(pointsTransformed[6].x, pointsTransformed[6].y, pointsTransformed[5].x, pointsTransformed[5].y);

		g_VGuiSurface->DrawLine(pointsTransformed[2].x, pointsTransformed[2].y, pointsTransformed[1].x, pointsTransformed[1].y);
		g_VGuiSurface->DrawLine(pointsTransformed[4].x, pointsTransformed[4].y, pointsTransformed[5].x, pointsTransformed[5].y);
		g_VGuiSurface->DrawLine(pointsTransformed[6].x, pointsTransformed[6].y, pointsTransformed[7].x, pointsTransformed[7].y);
		g_VGuiSurface->DrawLine(pointsTransformed[3].x, pointsTransformed[3].y, pointsTransformed[0].x, pointsTransformed[0].y);

		g_VGuiSurface->DrawLine(pointsTransformed[3].x, pointsTransformed[3].y, pointsTransformed[2].x, pointsTransformed[2].y);
		g_VGuiSurface->DrawLine(pointsTransformed[2].x, pointsTransformed[2].y, pointsTransformed[4].x, pointsTransformed[4].y);
		g_VGuiSurface->DrawLine(pointsTransformed[7].x, pointsTransformed[7].y, pointsTransformed[4].x, pointsTransformed[4].y);
		g_VGuiSurface->DrawLine(pointsTransformed[7].x, pointsTransformed[7].y, pointsTransformed[3].x, pointsTransformed[3].y);
		break;
	}


	const wchar_t* buf = L"Defuse Kit";

	int w = bbox.right - bbox.left;
	int tw, th;
	g_VGuiSurface->GetTextSize(esp_font, buf, tw, th);

	g_VGuiSurface->DrawSetTextFont(esp_font);
	g_VGuiSurface->DrawSetTextColor(esp_ctx.clr);
	g_VGuiSurface->DrawSetTextPos((bbox.left + w * 0.5f) - tw * 0.5f, bbox.bottom + 1);
	g_VGuiSurface->DrawPrintText(buf, wcslen(buf));
}
//--------------------------------------------------------------------------------
void Visuals::Misc::RenderPlantedC4(C_BaseEntity* ent)
{
	Vector pointsTransformed[8];
	auto bbox = GetBBox(ent, pointsTransformed);

	if (bbox.right == 0 || bbox.bottom == 0)
		return;


	float
		length_horizontal = (bbox.right - bbox.left) * 0.2f,
		length_vertical = (bbox.bottom - bbox.top) * 0.2f;

	g_VGuiSurface->DrawSetColor(g_Options.color_esp_c4);

	switch (g_Options.esp_dropped_c4)
	{
	case 0: //off
		break;
	case 1: //bounding box
		g_VGuiSurface->DrawLine(bbox.left, bbox.top, bbox.right, bbox.top);
		g_VGuiSurface->DrawLine(bbox.left, bbox.bottom, bbox.right, bbox.bottom);
		g_VGuiSurface->DrawLine(bbox.left, bbox.top, bbox.left, bbox.bottom);
		g_VGuiSurface->DrawLine(bbox.right, bbox.top, bbox.right, bbox.bottom);
		break;
	case 2:
		g_VGuiSurface->DrawLine(bbox.left, bbox.top, bbox.left + length_horizontal - 1, bbox.top);
		g_VGuiSurface->DrawLine(bbox.right - length_horizontal, bbox.top, bbox.right - 1, bbox.top);
		g_VGuiSurface->DrawLine(bbox.left, bbox.bottom - 1, bbox.left + length_horizontal - 1, bbox.bottom - 1);
		g_VGuiSurface->DrawLine(bbox.right - length_horizontal, bbox.bottom - 1, bbox.right - 1, bbox.bottom - 1);

		g_VGuiSurface->DrawLine(bbox.left, bbox.top, bbox.left, bbox.top + length_vertical - 1);
		g_VGuiSurface->DrawLine(bbox.right - 1, bbox.top, bbox.right - 1, bbox.top + length_vertical - 1);
		g_VGuiSurface->DrawLine(bbox.left, bbox.bottom - length_vertical, bbox.left, bbox.bottom - 1);
		g_VGuiSurface->DrawLine(bbox.right - 1, bbox.bottom - length_vertical, bbox.right - 1, bbox.bottom - 1);
		break;


		break;
	case 3:
		g_VGuiSurface->DrawLine(pointsTransformed[0].x, pointsTransformed[0].y, pointsTransformed[1].x, pointsTransformed[1].y);
		g_VGuiSurface->DrawLine(pointsTransformed[0].x, pointsTransformed[0].y, pointsTransformed[6].x, pointsTransformed[6].y);
		g_VGuiSurface->DrawLine(pointsTransformed[1].x, pointsTransformed[1].y, pointsTransformed[5].x, pointsTransformed[5].y);
		g_VGuiSurface->DrawLine(pointsTransformed[6].x, pointsTransformed[6].y, pointsTransformed[5].x, pointsTransformed[5].y);

		g_VGuiSurface->DrawLine(pointsTransformed[2].x, pointsTransformed[2].y, pointsTransformed[1].x, pointsTransformed[1].y);
		g_VGuiSurface->DrawLine(pointsTransformed[4].x, pointsTransformed[4].y, pointsTransformed[5].x, pointsTransformed[5].y);
		g_VGuiSurface->DrawLine(pointsTransformed[6].x, pointsTransformed[6].y, pointsTransformed[7].x, pointsTransformed[7].y);
		g_VGuiSurface->DrawLine(pointsTransformed[3].x, pointsTransformed[3].y, pointsTransformed[0].x, pointsTransformed[0].y);

		g_VGuiSurface->DrawLine(pointsTransformed[3].x, pointsTransformed[3].y, pointsTransformed[2].x, pointsTransformed[2].y);
		g_VGuiSurface->DrawLine(pointsTransformed[2].x, pointsTransformed[2].y, pointsTransformed[4].x, pointsTransformed[4].y);
		g_VGuiSurface->DrawLine(pointsTransformed[7].x, pointsTransformed[7].y, pointsTransformed[4].x, pointsTransformed[4].y);
		g_VGuiSurface->DrawLine(pointsTransformed[7].x, pointsTransformed[7].y, pointsTransformed[3].x, pointsTransformed[3].y);
		break;
	}

	const wchar_t* buf = L"C4";

	int w = bbox.right - bbox.left;
	int tw, th;
	g_VGuiSurface->GetTextSize(esp_font, buf, tw, th);

	g_VGuiSurface->DrawSetTextFont(esp_font);
	g_VGuiSurface->DrawSetTextColor(esp_ctx.clr);
	g_VGuiSurface->DrawSetTextPos((bbox.left + w * 0.5f) - tw * 0.5f, bbox.bottom + 1);
	g_VGuiSurface->DrawPrintText(buf, wcslen(buf));
}

void Visuals::DrawString(unsigned long font, int x, int y, Color color, unsigned long alignment, const char* msg, ...)
{

	va_list va_alist;
	char buf[1024];
	va_start(va_alist, msg);
	_vsnprintf(buf, sizeof(buf), msg, va_alist);
	va_end(va_alist);
	wchar_t wbuf[1024];
	MultiByteToWideChar(CP_UTF8, 0, buf, 256, wbuf, 256);

	int r = 255, g = 255, b = 255, a = 255;
	color.GetColor(r, g, b, a);

	int width, height;
	g_VGuiSurface->GetTextSize(font, wbuf, width, height);

	if (alignment & FONT_RIGHT)
		x -= width;
	if (alignment & FONT_CENTER)
		x -= width / 2;

	g_VGuiSurface->DrawSetTextFont(font);
	g_VGuiSurface->DrawSetTextColor(r, g, b, a);
	g_VGuiSurface->DrawSetTextPos(x, y - height / 2);
	g_VGuiSurface->DrawPrintText(wbuf, wcslen(wbuf));
}