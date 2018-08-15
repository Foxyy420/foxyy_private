#pragma once

#define NOMINMAX
#include <Windows.h>
#include <string>
#include <initializer_list>
#include "../valve_sdk/sdk.hpp"


namespace Utils
{
    void AttachConsole();

    void DetachConsole();

    bool ConsolePrint(const char* fmt, ...);
    
    char ConsoleReadKey();

    int WaitForModules(std::int32_t timeout, const std::initializer_list<std::wstring>& modules);

    std::uint8_t* PatternScan(void* module, const char* signature);

    void SetClantag(const char* tag);

    void SetName(const char* name);

    void RankRevealAll();

	DWORD FindPattern(std::string moduleName, BYTE* Mask, char* szMask);


	void LoadNamedSkys(const char *sky_name);
}
