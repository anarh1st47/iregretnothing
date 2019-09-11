#include <Windows.h>
#include "hooker/hooker.hpp"
#include "hooks.hpp"
#include "cheat.hpp"
#include "uobject.hpp"
#include "interfaces.hpp"
#include "math.hpp"
#include "utils.hpp"


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
		auto localplayer = viewport->controller->pawn;
		auto level = viewport->controller->level;

		
		if (localplayer) {
			utils::get_camera();
			auto i = 0;
			auto weapon = *(uintptr_t*)((*(uintptr_t*)&localplayer->inv->weap[0].ammo) + 0xa0);
			auto max_ents = *(DWORD*)(*(DWORD*)(weapon + 140) + 48);
			while (i < level->cnt_ents)
			{
				auto actorlist = level->actorlist;
				if (actorlist[i])
				{
					if (*(double*)(*(int*)&level + 0xD0) - *(float*)((int)(actorlist[i]) + 0x3C) < 0.1)
					{
						if (actorlist[i]->is_player()) {
							if (*(int*)actorlist[i] > 0) {
								auto player = (c_pawn*)actorlist[i];
								auto pos2 = math::world_to_screen(player->pos);
								draw_box(pos2.x - 100, pos2.y - 100, pos2.x + 100, pos2.y + 100);
								__asm nop
							}
						}
					}
				}
				i++;
			}

			localplayer->health = 1337 * 3;
			static auto old_pos = localplayer->pos;
			auto delta = localplayer->pos - old_pos;
			
			old_pos = localplayer->pos;



			auto weaps = localplayer->inv;
			for (auto i = 0; i < 10; i++) {
				auto weap = weaps->weap[i];
				if (!weap.ammo || !weap.ammo2)
					break;
				
				weap.ammo->bullets = 0x100;
			}
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

	/*static auto engine_handle = GetModuleHandleA("Engine.dll");
	static auto draw_line = (void(__thiscall*)(void*, float, float, float, float, c_color))GetProcAddress(engine_handle, "?DrawLine@FCanvasUtil@@QAEXMMMMVFColor@@@Z");
	draw_line(cheat::canvas->util, 100, 100, 100, 500.f, 0xff00ffff);*/

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

	auto master_process_tick_addr =  GetProcAddress(core_handle, "?MasterProcessTick@UInteractionMaster@@QAEXM@Z");
	h.create_hook(master_process_tick_h, master_process_tick_addr);


	auto master_process_pre_render_addr = GetProcAddress(core_handle, "?MasterProcessPreRender@UInteractionMaster@@QAEXPAVUCanvas@@@Z");
	h.create_hook(master_process_pre_render_h, master_process_pre_render_addr); 

	auto master_process_post_render_addr = GetProcAddress(core_handle, "?MasterProcessPostRender@UInteractionMaster@@QAEXPAVUCanvas@@@Z");
	h.create_hook(master_process_post_render_h, master_process_post_render_addr);

	
	//auto event_player_calc_view = GetProcAddress(core_handle, "?eventPlayerCalcView@APlayerController@@QAEXAAPAVAActor@@AAVFVector@@AAVFRotator@@@Z");
	//h.create_hook(event_player_calc_view_h, event_player_calc_view);

	h.enable();
};