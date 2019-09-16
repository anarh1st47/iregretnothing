#pragma once

struct c_ammo;

struct c_weapon {
	//0x94 owner
private:
	char pad_0000[160]; //0x0000
public:
	c_ammo* ammo; //0x00A0
private:
	char pad_00A4[556]; //0x00A4
	c_ammo* ammo2; //0x02D0
	char pad_02D4[512]; //0x02D4
	wchar_t* N000012ED; //0x04D4
	char pad_04D8[0x600 - 0x04D8];
}; //Size: 0x600