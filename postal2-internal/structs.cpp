#include "uobject.hpp"
#include <Windows.h>
#include "utils.hpp"

c_pawn* c_actor::is_player(){
		static auto func = (c_pawn * (__cdecl*)(c_actor*))utils::pattern_scan(cheat::engine_handle, "55 8b ec 56 8B 75 08 85 f6 74 16 68 80");
		return func(this);
}