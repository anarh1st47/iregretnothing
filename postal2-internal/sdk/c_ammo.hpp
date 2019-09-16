#pragma once
#include <cstdint>

struct c_ammo
{
private:
	char pad_0000[716]; //0x0000
public:
	int32_t bullets; //0x02CC
private:
	char pad_02D0[176]; //0x02D0
}; //Size: 0x0380