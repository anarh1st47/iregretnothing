#include <Windows.h>
#include "hooker/hooker.hpp"
#include "hooks.hpp"
#include "cheat.hpp"
#include "uobject.hpp"



void __fastcall hooks::master_process_tick_h(uinteractionmaster* ecx, void* edx, float delta) {
	static auto ofunc = h.original(&master_process_tick_h);
	if (cheat::canvas) {
		auto viewport = cheat::canvas->Viewport;
		auto player = viewport->controller->pawn;
		if (player) {
			player->health = 1337 * 3;
			static auto old_pos = player->pos;
			auto delta = old_pos - player->pos;
			//speedhack ? 
			//player->pos += delta;
			//player->pos += delta;
			//player->pos += delta;
			//player->pos2 += delta;
			//player->pos2 += delta;
			//player->pos2 += delta;
			//player->pos3 += delta;
			//player->pos3 += delta;
			//player->pos3 += delta;
			old_pos = player->pos;



			auto weaps = player->inv;
			for (auto i = 0; i < 10; i++) {
				auto weap = weaps->weap[i];
				if (!weap.ammo || !weap.ammo2)
					break;
				
				weap.ammo->bullets = 0x100;
			}
			__asm nop
		}
	}
	return ofunc(ecx, edx, delta);
}

void __fastcall hooks::master_process_pre_render_h(uinteractionmaster* ecx, void* edx, c_canvas* canvas) {
	static auto ofunc = h.original(&master_process_pre_render_h);
	cheat::canvas = canvas;
	return ofunc(ecx, edx, canvas);
}


void hooks::initialize() {
	auto core_handle = GetModuleHandleA("Engine.dll");
	while (!core_handle)
		core_handle = GetModuleHandleA("Engine.dll");

	auto master_process_tick_addr =  GetProcAddress(core_handle, "?MasterProcessTick@UInteractionMaster@@QAEXM@Z");
	h.create_hook(master_process_tick_h, master_process_tick_addr);


	auto master_process_pre_render_addr = GetProcAddress(core_handle, "?MasterProcessPreRender@UInteractionMaster@@QAEXPAVUCanvas@@@Z");
	h.create_hook(master_process_pre_render_h, master_process_pre_render_addr);

	//
	h.enable();
};