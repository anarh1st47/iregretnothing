#pragma once
#include "memhacks.hpp"
#include "../cheat.hpp"
#include "../options.hpp"

#include "../sdk/c_pawn.hpp"
#include "../sdk/c_ammo.hpp"
#include "../sdk/c_inventory.hpp"
#include "../sdk/c_weapon.hpp"
#include "../sdk/c_level.hpp"

void hacks::memhacks::godmode() {
	if (!options::misc_godmode)
		return;

	if (!cheat::localplayer)
		return;
	cheat::localplayer->set_health(1337);
}

void hacks::memhacks::healthhack() {
	if (!options::misc_health)
		return;
	cheat::level->for_each_player([&](c_pawn* player) -> void {
		if (player->health <= 0)
			return;
		if (player != cheat::localplayer)
			player->set_health(1);
		});
}

void hacks::memhacks::inf_ammo() {
	if (!options::misc_infammo)
		return;

	auto& weaps = cheat::localplayer->inv;
	for (auto i = 0; i < 10; i++) {
		auto weap = weaps->weap[i];
		if (!weap.ammo || !weap.ammo2)
			break;
		weap.ammo->bullets = 0x100;
	}

}
