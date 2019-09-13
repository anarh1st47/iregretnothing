#pragma once
#include "../math.hpp"

struct APlayerController_eventPlayerCalcView_Parms
{
	struct AActor* ViewActor;
	vec_3d CameraLocation;
	vec_3d_int CameraRotation;
};

struct UCanvas_execWorldToScreen_Parms {
	vec_3d WorldLoc; //0x0
	vec_3d _pad1[0x2];//0xc
	vec_3d ReturnValue;//0x24
};
