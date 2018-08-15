#pragma once

#include "helpers\math.hpp"

template< typename Function > Function call_vfunc(PVOID Base, DWORD Index)
{
	PDWORD* VTablePointer = (PDWORD*)Base;
	PDWORD VTableFunctionBase = *VTablePointer;
	DWORD dwAddress = VTableFunctionBase[Index];
	return (Function)(dwAddress);
}
template <typename T> __forceinline T GetVirtualFunction(PVOID Base, DWORD Index)
{
	return (T)((uint32_t *) *(uint32_t **)Base)[Index];
}

struct ColorRGBExp32
{
	byte r, g, b;
	signed char exponent;
};

struct dlight_t
{
	int		flags;
	Vector	origin;
	float	radius;
	ColorRGBExp32 color;
	float	die;
	float	decay;
	float	minlight;
	int		key;
	int		style;
	Vector	m_Direction;
	float	m_InnerAngle;
	float	m_OuterAngle;
	float GetRadius() const
	{
		return radius;
	}
	float GetRadiusSquared() const
	{
		return radius * radius;
	}
	float IsRadiusGreaterThanZero() const
	{
		return radius > 0.0f;
	}
};


class IVEffects
{
public:
	dlight_t* CL_AllocDlight(int key)
	{
		typedef dlight_t*(__thiscall* OriginalFn)(PVOID, int);
		return call_vfunc<OriginalFn>(this, 4)(this, key);
	}
	dlight_t* CL_AllocElight(int key)
	{
		typedef dlight_t*(__thiscall* OriginalFn)(PVOID, int);
		return call_vfunc<OriginalFn>(this, 5)(this, key);
	}
	dlight_t* GetElightByKey(int key)
	{
		typedef dlight_t*(__thiscall* OriginalFn)(PVOID, int);
		return call_vfunc<OriginalFn>(this, 8)(this, key);
	}
};
