#pragma once
#include <cstdint>
struct ImVec4;

struct c_color {
	uint8_t r, g, b, a;
	c_color(int v);
	void from_imvec4(ImVec4 v);
};