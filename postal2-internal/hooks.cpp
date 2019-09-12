#include <Windows.h>
#include "hooker/hooker.hpp"
#include "hooks.hpp"
#include "cheat.hpp"
#include "uobject.hpp"
#include "interfaces.hpp"
#include "math.hpp"
#include "utils.hpp"
#include "memhacks.hpp"
#include "color.hpp"
#include "inputsys.hpp"

bool is_screen_pos(const vec_3d& pos) {
	return cheat::canvas && cheat::canvas->clip_x > pos.x && pos.x > 0.f  && cheat::canvas->clip_y > pos.y && pos.y > 0.f;
}



void draw_box(float x1, float y1, float x2, float y2, c_color color) {
	cheat::canvas->util->draw_line(x1, y1, x1, y2, color);
	cheat::canvas->util->draw_line(x1, y1, x2, y1, color);
	cheat::canvas->util->draw_line(x1, y2, x2, y2, color);
	cheat::canvas->util->draw_line(x2, y1, x2, y2, color);
}

void draw_player_esp(c_pawn* player)
{
	float width, left, right, top, bot;
	auto color = c_color(0xffffffff);
	auto pos_top = player->pos;
	auto orig_height = player->height * 2.f;
	pos_top.z += player->height;

	auto pos_bot = player->pos;
	pos_bot.z -= player->height;

	auto pos_top_2d = math::world_to_screen(pos_top);
	if (!is_screen_pos(pos_top_2d))
		return;
	auto pos_bot_2d = math::world_to_screen(pos_bot);
	if (!is_screen_pos(pos_bot_2d))
		return;
	auto scaled_height = std::fabsf(pos_top_2d.y - pos_bot_2d.y);
	auto coef = orig_height / scaled_height;

	width = (player->height / coef) / 1.5;

	left = pos_top_2d.x + width;
	right = pos_top_2d.x - width;
	top = pos_top_2d.y;
	bot = pos_bot_2d.y;

	draw_box(left, top, right, bot, color);

	cheat::canvas->util->draw_text(bot, left, player->name2);
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
			level->for_each_player([&](c_pawn* player) -> void {
				if (player->health <= 0)
					return;
				draw_player_esp(player);
				//auto screen_pos = math::world_to_screen(player->pos);
				//draw_box(screen_pos.x - 100, screen_pos.y - 100, screen_pos.x + 100, screen_pos.y + 100);
			});
			
			 
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

void hooks::initialize() {
	auto master_process_tick_addr = GetProcAddress(cheat::engine_handle, "?MasterProcessTick@UInteractionMaster@@QAEXM@Z");
	h.create_hook(master_process_tick_h, master_process_tick_addr);

	auto master_process_pre_render_addr = GetProcAddress(cheat::engine_handle, "?MasterProcessPreRender@UInteractionMaster@@QAEXPAVUCanvas@@@Z");
	h.create_hook(master_process_pre_render_h, master_process_pre_render_addr);

	h.enable();
};

void hooks::destroy() {
	h.restore();
}