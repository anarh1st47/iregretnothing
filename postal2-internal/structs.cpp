#include "uobject.hpp"
#include <Windows.h>
#include "utils.hpp"
#include "color.hpp"

c_pawn* c_actor::is_player(){
		static auto func = (c_pawn * (__cdecl*)(c_actor*))utils::pattern_scan(cheat::engine_handle, "55 8b ec 56 8B 75 08 85 f6 74 16 68 80");
		return func(this);
}


void c_canvas_util::draw_line(float x1, float y1, float x2, float y2, c_color color) {
	static auto draw_line = (void(__thiscall*)(void*, float, float, float, float, c_color))GetProcAddress(cheat::engine_handle, "?DrawLine@FCanvasUtil@@QAEXMMMMVFColor@@@Z");
	draw_line(this, x1, y1, x2, y2, color);
}
void c_canvas_util::draw_text(float x1, float y1, wchar_t* text, c_color color) {
	static auto draw_text = (void(__thiscall*)(void*, float, float, wchar_t*, c_font*, c_color))GetProcAddress(cheat::engine_handle, "?DrawString@FCanvasUtil@@QAEHHHPB_WPAVUFont@@VFColor@@@Z");
	draw_text(this, x1, y1, text, cheat::canvas->font, color);
}