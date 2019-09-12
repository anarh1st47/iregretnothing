#pragma once
#include <cstdint>
#include <Windows.h>
#include "math.hpp"

struct c_canvas;
struct c_pawn;

namespace cheat {
	DWORD WINAPI initialize(void*);
	void unload();
	inline c_canvas* canvas = nullptr;
	inline HMODULE core_handle;
	inline HMODULE engine_handle;
	inline c_pawn* localplayer;

	namespace camera {
		inline vec_3d pos;
		inline vec_3d_int rot;
	};
};