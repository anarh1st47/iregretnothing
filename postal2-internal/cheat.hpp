#pragma once
#include <cstdint>
#include <Windows.h>
#include "math.hpp"

struct c_canvas;

namespace cheat {
	DWORD WINAPI initialize(void*);
	inline c_canvas* canvas = nullptr;

	namespace camera {
		inline vec_3d pos;
		inline vec_3d_int rot;
	};
};