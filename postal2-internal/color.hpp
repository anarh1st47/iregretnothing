#pragma once
#include <cstddef>

struct c_color {
	std::byte r, g, b, a;
	c_color(int v) {
		std::memcpy(this, (void*)&v, 4);
	}
};