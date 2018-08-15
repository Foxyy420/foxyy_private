#include "KitParser.h"

#include <algorithm>
#include <codecvt>
#include <fstream>
#include <json.hpp>
#include <iomanip>

using json = nlohmann::json;

void KitParser::Dump()
{
	static std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;

	// Search the relative calls

	// call    ItemSystem
	// push    dword ptr [esi+0Ch]
	// lea     ecx, [eax+4]
	// call    CEconItemSchema::GetPaintKitDefinition

	auto sig_address = Utils::PatternScan(GetModuleHandleA("client.dll"), "E8 ? ? ? ? FF 76 0C 8D 48 04 E8");

	// Skip the opcode, read rel32 address
	auto item_system_offset = *reinterpret_cast<int32_t*>(sig_address + 1);

	// Add the offset to the end of the instruction
	auto item_system_fn = reinterpret_cast<CCStrike15ItemSystem* (*)()>(sig_address + 5 + item_system_offset);

	// Skip VTable, first member variable of ItemSystem is ItemSchema
	auto item_schema = reinterpret_cast<CCStrike15ItemSchema*>(uintptr_t(item_system_fn()) + sizeof(void*));

	// Dump paint kits
	{
		// Skip the instructions between, skip the opcode, read rel32 address
		auto get_paint_kit_definition_offset = *reinterpret_cast<int32_t*>(sig_address + 11 + 1);

		// Add the offset to the end of the instruction
		auto get_paint_kit_definition_fn = reinterpret_cast<CPaintKit* (__thiscall *)(CCStrike15ItemSchema*, int)>(sig_address + 11 + 5 + get_paint_kit_definition_offset);

		// The last offset is head_element, we need that

		// push    ebp
		// mov     ebp, esp
		// sub     esp, 0Ch
		// mov     eax, [ecx+298h]

		// Skip instructions, skip opcode, read offset
		auto start_element_offset = *reinterpret_cast<intptr_t*>(uintptr_t(get_paint_kit_definition_fn) + 8 + 2);

		// Calculate head base from start_element's offset
		auto head_offset = start_element_offset - 12;

		auto map_head = reinterpret_cast<Head_t<int, CPaintKit*>*>(uintptr_t(item_schema) + head_offset);

		for (int i = 0; i <= map_head->last_element; ++i)
		{
			auto paint_kit = map_head->memory[i].value;

			if (paint_kit->id == 9001)
				continue;

			const wchar_t* wide_name = g_Localize->Find(paint_kit->item_name.buffer + 1);
			auto name = converter.to_bytes(wide_name);

			if (paint_kit->id < 10000)
				map_skins[std::to_string(paint_kit->id)] = name;
			else
				map_gloves[std::to_string(paint_kit->id)] = name;
		}
	}

	// Dump sticker kits
	{
		auto sticker_sig = Utils::PatternScan(GetModuleHandleA("client.dll"), "53 8D 48 04 E8 ? ? ? ? 8B 4D 10") + 4;

		// Skip the opcode, read rel32 address
		auto get_sticker_kit_definition_offset = *reinterpret_cast<intptr_t*>(sticker_sig + 1);

		// Add the offset to the end of the instruction
		auto get_sticker_kit_definition_fn = reinterpret_cast<CPaintKit* (__thiscall *)(CCStrike15ItemSchema*, int)>(sticker_sig + 5 + get_sticker_kit_definition_offset);

		// The last offset is head_element, we need that

		//	push    ebp
		//	mov     ebp, esp
		//	push    ebx
		//	push    esi
		//	push    edi
		//	mov     edi, ecx
		//	mov     eax, [edi + 2BCh]

		// Skip instructions, skip opcode, read offset
		auto start_element_offset = *reinterpret_cast<intptr_t*>(uintptr_t(get_sticker_kit_definition_fn) + 8 + 2);

		// Calculate head base from start_element's offset
		auto head_offset = start_element_offset - 12;

		auto map_head = reinterpret_cast<Head_t<int, CStickerKit*>*>(uintptr_t(item_schema) + head_offset);

		for (int i = 0; i <= map_head->last_element; ++i)
		{
			auto sticker_kit = map_head->memory[i].value;

			char sticker_name_if_valve_fucked_up_their_translations[64];

			auto sticker_name_ptr = sticker_kit->item_name.buffer + 1;

			if (strstr(sticker_name_ptr, "StickerKit_dhw2014_dignitas"))
			{
				strcpy_s(sticker_name_if_valve_fucked_up_their_translations, "StickerKit_dhw2014_teamdignitas");
				strcat_s(sticker_name_if_valve_fucked_up_their_translations, sticker_name_ptr + 27);
				sticker_name_ptr = sticker_name_if_valve_fucked_up_their_translations;
			}

			const wchar_t* wide_name = g_Localize->Find(sticker_name_ptr);
			auto name = converter.to_bytes(wide_name);

			map_stickers[std::to_string(sticker_kit->id)] = name;
		}
	}


	json skins(map_skins);

	std::ofstream skinso("C:\\Aimdose\\dump\\skins.json");
	if (skinso.good())
		skinso << std::setw(4) << skins << std::endl;

	json gloves(map_gloves);

	std::ofstream gloveso("C:\\Aimdose\\dump\\gloves.json");
	if (gloveso.good())
		gloveso << std::setw(4) << gloves << std::endl;

	json stickers(map_stickers);

	std::ofstream stickero("C:\\Aimdose\\dump\\stickers.json");
	if (stickero.good())
		stickero << std::setw(4) << stickers << std::endl;
}
