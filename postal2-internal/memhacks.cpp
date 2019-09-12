#pragma once
#include "memhacks.hpp"
#include "cheat.hpp"
#include "uobject.hpp"

void hacks::memhacks::godmode() {
	if (!cheat::localplayer)
		return;
	cheat::localplayer->set_health(1337);
}

void hacks::memhacks::inf_ammo() {
	auto &weaps = cheat::localplayer->inv;
	for (auto i = 0; i < 10; i++) {
		auto weap = weaps->weap[i];
		if (!weap.ammo || !weap.ammo2)
			break;

		weap.ammo->bullets = 0x100;
	}
}
