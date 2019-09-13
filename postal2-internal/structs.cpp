#include <Windows.h>
#include "utils.hpp"
#include "cheat.hpp"
#include "structs.hpp"

c_pawn* c_actor::is_player() {
	static auto func = (c_pawn * (__cdecl*)(c_actor*))utils::pattern_scan(cheat::engine_handle, "55 8B EC 56 8B 75 08 85 f6 74 16 68 80");
	return func(this);
}


void c_canvas_util::draw_line(float x1, float y1, float x2, float y2, c_color color) {
	static auto draw_line = (void(__thiscall*)(void*, float, float, float, float, c_color))GetProcAddress(cheat::engine_handle, "?DrawLine@FCanvasUtil@@QAEXMMMMVFColor@@@Z");
	draw_line(this, x1, y1, x2, y2, color);
}
void c_canvas_util::draw_text(float x, float y, wchar_t* text, c_color color) {
	static auto draw_text = (void(__thiscall*)(void*, float, float, wchar_t*, c_font*, c_color))GetProcAddress(cheat::engine_handle, "?DrawString@FCanvasUtil@@QAEHHHPB_WPAVUFont@@VFColor@@@Z");
	draw_text(this, x + .7f, y + .7f, text, cheat::canvas->font, 0xff0000ff);
	draw_text(this, x, y, text, cheat::canvas->font, color);
}


void* c_canvas::find_function(FName name, int unk) {
	auto func = (void* (__thiscall*)(c_canvas*, FName, int))GetProcAddress(cheat::core_handle, "?FindFunction@UObject@@QAEPAVUFunction@@VFName@@H@Z");
	return func(this, name, unk);
}
void* c_canvas::process_event(void* _func, void* params, int unk) {
	auto func = (void* (__thiscall*)(c_canvas*, void*, void*, int))GetProcAddress(cheat::core_handle, "?ProcessEvent@UObject@@UAEXPAVUFunction@@PAX1@Z");
	return func(this, _func, params, unk);
}


void* c_controller::find_function(FName name, int unk) {
	auto func = (void* (__thiscall*)(c_controller*, FName, int))GetProcAddress(cheat::core_handle, "?FindFunction@UObject@@QAEPAVUFunction@@VFName@@H@Z");
	return func(this, name, unk);
}
void* c_controller::process_event(void* _func, void* params, int unk) {
	auto func = (void* (__thiscall*)(c_controller*, void*, void*, int))GetProcAddress(cheat::core_handle, "?ProcessEvent@UObject@@UAEXPAVUFunction@@PAX1@Z");
	return func(this, _func, params, unk);
}

void c_level::for_each_player(std::function<void(c_pawn*)> f) {
	for (auto i = 0; i < cnt_ents; i++) {
		auto& ent = actorlist[i];
		if (!ent)
			continue;
		if (unk_meme - ent->unk_meme >= 0.1)
			continue;
		auto player = ent->is_player();
		if (!player || *(int*)player <= 0)
			continue;
		f(player);
	}
}