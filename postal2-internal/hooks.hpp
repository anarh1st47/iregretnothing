#pragma once
struct c_canvas;
struct c_pawn;
struct vec_3d;
struct vec_3d_int;

namespace hooks {
	using uinteractionmaster = void;

	void __fastcall master_process_tick_h(uinteractionmaster* ecx, void* edx, float delta);
	void __fastcall master_process_pre_render_h(uinteractionmaster* ecx, void* edx, c_canvas* canvas);
	void __fastcall master_process_post_render_h(uinteractionmaster* ecx, void* edx, c_canvas* canvas);
	void __fastcall event_player_calc_view_h(void* ecx, void* edx, c_pawn** a2, vec_3d& cam_location, vec_3d_int& cam_rotation);
	void initialize();
	void destroy();
};