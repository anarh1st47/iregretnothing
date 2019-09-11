#include <Windows.h>
#include "hooker/hooker.hpp"
#include "hooks.hpp"
#include "cheat.hpp"
#include "uobject.hpp"
#include "interfaces.hpp"
#include "math.hpp"
#include "utils.hpp"
#include "memhacks.hpp"


using c_color = int;


void draw_line(float x1, float y1, float x2, float y2) {
	static auto engine_handle = GetModuleHandleA("Engine.dll");
	static auto draw_line = (void(__thiscall*)(void*, float, float, float, float, c_color))GetProcAddress(engine_handle, "?DrawLine@FCanvasUtil@@QAEXMMMMVFColor@@@Z");
	draw_line(cheat::canvas->util, x1, y1, x2, y2, 0xff00ffff);
}

void draw_box(float x1, float y1, float x2, float y2) {
	draw_line(x1, y1, x1, y2);
	draw_line(x1, y1, x2, y1);
	draw_line(x1, y2, x2, y2);
	draw_line(x2, y1, x2, y2);
}


void __fastcall hooks::master_process_tick_h(uinteractionmaster* ecx, void* edx, float delta) {
	static auto ofunc = h.original(&master_process_tick_h);
	if (cheat::canvas) {
		auto viewport = cheat::canvas->viewport;
		if(!viewport)
			return ofunc(ecx, edx, delta);
		auto controller = viewport->controller;
		if (!controller)
			return;
		cheat::localplayer = controller->pawn;
		auto level = controller->level;


		if (cheat::localplayer) {
			utils::get_camera();
			for (auto i = 0; i < level->cnt_ents; i++) {
				auto actorlist = level->actorlist;
				auto &ent = actorlist[i];
				if (!ent)
					continue;
				if (level->unk_meme - ent->unk_meme >= 0.1)
					continue;
				if (auto player = ent->is_player(); !player || *(int*)player <= 0)
					continue;
				else {
					auto screen_pos = math::world_to_screen(player->pos);
					draw_box(screen_pos.x - 100, screen_pos.y - 100, screen_pos.x + 100, screen_pos.y + 100);
				}
			}
			 
			hacks::memhacks::godmode();

			hacks::memhacks::inf_ammo();
		}
	}
	return ofunc(ecx, edx, delta);
}

void __fastcall hooks::master_process_pre_render_h(uinteractionmaster* ecx, void* edx, c_canvas* canvas) {
	static auto ofunc = h.original(&master_process_pre_render_h);
	cheat::canvas = canvas;
	ofunc(ecx, edx, canvas);
}

void __fastcall hooks::master_process_post_render_h(uinteractionmaster* ecx, void* edx, c_canvas* canvas) {
	static auto ofunc = h.original(&master_process_post_render_h);
	//cheat::canvas = canvas;
	ofunc(ecx, edx, canvas);

}



void __fastcall hooks::event_player_calc_view_h(void* ecx, void* edx, c_pawn** a2, vec_3d& cam_location, vec_3d_int& cam_rotation) {
	static auto ofunc = h.original(&event_player_calc_view_h);
	ofunc(ecx, edx, a2, cam_location, cam_rotation);
	cheat::camera::pos = cam_location;
	//cheat::camera::rot = vec_3d{ (float)cam_rotation.x, (float)cam_rotation.y, (float)cam_rotation.z };
}


void hooks::initialize() {
	auto core_handle = GetModuleHandleA("Engine.dll");
	while (!core_handle)
		core_handle = GetModuleHandleA("Engine.dll");

	auto master_process_tick_addr = GetProcAddress(core_handle, "?MasterProcessTick@UInteractionMaster@@QAEXM@Z");
	h.create_hook(master_process_tick_h, master_process_tick_addr);


	auto master_process_pre_render_addr = GetProcAddress(core_handle, "?MasterProcessPreRender@UInteractionMaster@@QAEXPAVUCanvas@@@Z");
	h.create_hook(master_process_pre_render_h, master_process_pre_render_addr);

	auto master_process_post_render_addr = GetProcAddress(core_handle, "?MasterProcessPostRender@UInteractionMaster@@QAEXPAVUCanvas@@@Z");
	h.create_hook(master_process_post_render_h, master_process_post_render_addr);


	//auto event_player_calc_view = GetProcAddress(core_handle, "?eventPlayerCalcView@APlayerController@@QAEXAAPAVAActor@@AAVFVector@@AAVFRotator@@@Z");
	//h.create_hook(event_player_calc_view_h, event_player_calc_view);

	h.enable();
};