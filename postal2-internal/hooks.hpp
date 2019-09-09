#pragma once
struct c_canvas;

namespace hooks {
	using uinteractionmaster = void;

	void __fastcall master_process_tick_h(uinteractionmaster* ecx, void* edx, float delta);
	void __fastcall master_process_pre_render_h(uinteractionmaster* ecx, void* edx, c_canvas* canvas);
	void initialize();
};