#pragma once
#include "../color.hpp"

struct c_canvas_util {
	void draw_line(float x1, float y1, float x2, float y2, c_color color = 0xff00ffff);
	void draw_text(float x1, float y1, wchar_t* text, c_color color = 0xff00ffff);
};

