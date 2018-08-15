#pragma once
#include "MainInclude.hpp"

#include <vector>

extern unsigned long indicator;

struct HitMarkerInfo
{
	float m_flExpTime;
	int m_iDmg;
};

struct EventInfo
{
	std::string m_szMessage;
	float m_flExpTime;
};


class HitMarkerEvent : public IGameEventListener2, public Singleton<HitMarkerEvent>
{
public:

	void FireGameEvent(IGameEvent *event);
	int  GetEventDebugID(void);

	void RegisterSelf();
	void UnregisterSelf();

	void Paint(void);

private:
	std::vector<HitMarkerInfo> hitMarkerInfo;
	std::vector<EventInfo> eventInfo;
	float _flHurtTime;
};

class BulletImpactEvent : public IGameEventListener2, public Singleton<BulletImpactEvent>
{
public:

	void FireGameEvent(IGameEvent *event);
	int  GetEventDebugID(void);

	void RegisterSelf();
	void UnregisterSelf();
};
