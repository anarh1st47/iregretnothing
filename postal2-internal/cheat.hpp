#pragma once
#include <cstdint>
#include <Windows.h>

struct c_canvas;

namespace cheat {
	DWORD WINAPI initialize(void*);
	inline c_canvas* canvas = nullptr;
};