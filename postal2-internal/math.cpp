#include "math.hpp"
#include "uobject.hpp"
#include "cheat.hpp"


vec_3d math::world_to_screen(vec_3d pos)
{
	UCanvas_execWorldToScreen_Parms pCanvas_WorldToScreen_Parms;
	pCanvas_WorldToScreen_Parms.WorldLoc = pos;
	FName W2S = 0x7e4;
	auto pFunc = cheat::canvas->find_function(W2S, FNAME_Find);
	if (pFunc != NULL) {
		cheat::canvas->process_event(pFunc, &pCanvas_WorldToScreen_Parms, NULL);
		return pCanvas_WorldToScreen_Parms.ReturnValue;
	}
	return vec_3d(0, 0, 0);
}

