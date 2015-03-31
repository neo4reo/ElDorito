#include "Ammo.h"

#include "../ElDorito.h"

#include <Windows.h>
#include <iostream>

Ammo::Ammo()
	:
	enabled(false),
	NadePatch(0x7A3BFF,
	{ 0x90, 0x90 },
	{ 0xFE, 0xC8 }),

	GunPatch(0x75FACD,
	{ 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 },
	{ 0x66, 0x29, 0x94, 0x31, 0x8E, 0x02, 0x00, 0x00 }),

	EnergyPatch(0x75BE42,
	{ 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 },
	{ 0xF3, 0x0F, 0x11, 0x87, 0xA8, 0x01, 0x00, 0x00 })
{
}

Ammo::~Ammo()
{
}

std::string Ammo::Info()
{
	std::string Info = "Unlimited ammo: ";
	Info += (enabled ? "Enabled" : "Disabled");
	Info += "\nUsage: ammo (on|off)\n"
		"Bottomless clip on weapon ammo/energy and grenades\n"
		"Respawn to have all grenade types when enabled.\n";

	return Info;
}

void Ammo::Tick(const std::chrono::duration<double>& Delta)
{
}

bool Ammo::Run(const std::vector<std::string>& Args)
{
	if( Args.size() >= 2 )
	{
		if( Args[1].compare("off") == 0 )
		{
			// Disable it.
			std::cout << "Disabling unlimited ammo" << std::endl;
			enabled = false;

			NadePatch.Reset();
			GunPatch.Reset();
			EnergyPatch.Reset();

			return true;
		}
		else if( Args[1].compare("on") == 0 )
		{
			// Nade patch
			const uint8_t allGrenades[] = { 0xC6, 0x84, 0x37, 0x20, 0x03, 0x00, 0x00, 0x63 };
			//eldorado.exe + 1A3273 - C6 84 37 20 03 00 00 00 - mov byte ptr[edi + esi + 00000320], 00
			//SetPageAccess(Pointer::Base() + 0x1a3273, sizeof(allGrenades), PAGE_EXECUTE_READWRITE);
			Pointer::Base(0x1a3273).Write(allGrenades, sizeof(allGrenades));

			//For some reason doesnt freezes up when using Write. Stick to memcpy for now. Look into later.
			//memcpy(Pointer::Base()(0x1a3273), allGrenades, sizeof(allGrenades));

			// Enable
			std::cout
				<< "Enabling unlimited ammo\n"
				"(Respawn/suicide to get all grenades)\n"
				<< std::endl;
			enabled = true;

			NadePatch.Apply();
			GunPatch.Apply();
			EnergyPatch.Apply();

			return true;
		}
	}

	return false;
}