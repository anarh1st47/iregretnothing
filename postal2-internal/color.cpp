#include "color.hpp"
#include "imgui/imgui.h"

#include <cstdint>
#include <memory>

c_color::c_color(int v) {
	std::memcpy(this, (void*)& v, 4);
}

void c_color::from_imvec4(ImVec4 v) {
	this->r = (uint8_t)v.x;
	this->g = (uint8_t)v.y;
	this->b = (uint8_t)v.z;
	this->a = (uint8_t)v.w;
}