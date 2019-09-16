#pragma once
#include <cstdint>
#include "../math.hpp"

struct c_inventory;
struct c_weapon;

struct c_pawn {
private:
	char pad_0000[232]; //0x0000
public:
	vec_3d pos; //0x00E8
private:
	char pad_00F4[52]; //0x00F4
	vec_3d pos2; //0x0128
	char pad_0134[160 - 0x10 + 4]; //0x0134
public:
	float height; //0x01c8
private:
	char _pad[0x10 - 4];
	char pad_01D8[52]; //0x01D8
	vec_3d pos3; //0x020C
	char pad_0218[0x50]; //0x0218 
	c_pawn* next; // 268 next
	char pad_2222[0x94];
	wchar_t* name; //0x0300
	char pad_0304[8]; //0x0304
public:
	c_weapon* active_weapon; //30c
	char pad_0310[4]; //0x0310
	class N00001192* inventory; //0x0314
	char pad_0318[40]; //0x0318
	int32_t health; //0x0340
	char pad_0344[104]; //0x0344
	wchar_t* name2; //0x03AC
	char pad_03B0[32]; //0x03B0
	class N000011A5* N00000B49; //0x03D0
	char pad_03D4[156]; //0x03D4
	wchar_t* N00000B71; //0x0470
	char pad_0474[44]; //0x0474
	float N00000B7D; //0x04A0
	char pad_04A4[20]; //0x04A4
	float N00000B83; //0x04B8
	char pad_04BC[48]; //0x04BC
	float N00000B90; //0x04EC
	char pad_04F0[8]; //0x04F0
	wchar_t* N00000B93; //0x04F8
	wchar_t* N00000B94; //0x04FC
	char pad_0500[52]; //0x0500
	wchar_t* N00000BA2; //0x0534
	char pad_0538[4]; //0x0538
public:
	c_inventory* inv; //0x053C
private:
	char pad_0540[1232]; //0x0540
	wchar_t* N00000CD8; //0x0A10
	char pad_0A14[8]; //0x0A14
	wchar_t* N00000CDB; //0x0A1C
	char pad_0A20[1596]; //0x0A20
public:
	void set_health(int val) {
		health = val * 3;
	}
}; //Size: 0x105C
