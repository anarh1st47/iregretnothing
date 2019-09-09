#pragma once
#include <cstdint>
#include "math.hpp"

using FPointer = uintptr_t;
using FQWord = uint64_t;
using FName = char[8];
class UClass;


class UObject
{
public:
	FPointer			VfTableObject;							// 0x0000 (0x04)
	FPointer			HashNext;                               // 0x0004 (0x04)
	FQWord			    ObjectFlags;                            // 0x0008 (0x08)
	FPointer			HashOuterNext;                          // 0x0010 (0x04)
	FPointer			StateFrame;                             // 0x0014 (0x04)
	UObject*            Linker;                                 // 0x0018 (0x04)
	FPointer			LinkerIndex;							// 0x001C (0x04)
	int					ObjectInternalInteger;					// 0x0020 (0x04)
	int                 NetIndex;                               // 0x0024 (0x04)
	UObject*            Outer;                                  // 0x0028 (0x04)
	FName			    Name;                                   // 0x002C (0x08)
	UClass*             Class;                                  // 0x0034 (0x04)
	UObject*            ObjectArchetype;						// 0x0038 (0x04)

private:
	//static UClass* pClassPointer;
};

struct c_ammo
{
	char pad_0000[716]; //0x0000
	int32_t bullets; //0x02CC
	char pad_02D0[176]; //0x02D0
}; //Size: 0x0380

struct c_weapon
{
	char pad_0000[160]; //0x0000
	c_ammo* ammo; //0x00A0
	char pad_00A4[556]; //0x00A4
	c_ammo* ammo2; //0x02D0
	char pad_02D4[512]; //0x02D4
	wchar_t* N000012ED; //0x04D4
	char pad_04D8[0x600- 0x04D8];
}; //Size: 0x600

struct c_inventory {
	c_weapon weap[10];
};

struct c_pawn
{
	char pad_0000[232]; //0x0000
	vec_3d pos; //0x00E8
	char pad_00F4[52]; //0x00F4
	vec_3d pos2; //0x0128
	char pad_0134[216]; //0x0134
	vec_3d pos3; //0x020C
	char pad_0218[232]; //0x0218
	wchar_t* N00000B15; //0x0300
	char pad_0304[16]; //0x0304
	class N00001192* inventory; //0x0314
	char pad_0318[40]; //0x0318
	int32_t health; //0x0340
	char pad_0344[104]; //0x0344
	wchar_t* N00000B40; //0x03AC
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
	c_inventory* inv; //0x053C
	char pad_0540[1232]; //0x0540
	wchar_t* N00000CD8; //0x0A10
	char pad_0A14[8]; //0x0A14
	wchar_t* N00000CDB; //0x0A1C
	char pad_0A20[1596]; //0x0A20
}; //Size: 0x105C

struct c_controller{
	char _pad[0x268];
	c_pawn* pawn;
};

struct c_viewport {
	char _pad[0x30];
	c_controller* controller;
	/*char _pad[0x560];
	void* weapon;
	char _pad2[0xc];
	void* actor1;
	void* actor2;*/
};

struct c_canvas //: public UObject
{
	char _pad[0xa0];
	/*class UFont* Font;
	FLOAT FontScaleX;
	FLOAT FontScaleY;
	FLOAT SpaceX;
	FLOAT SpaceY;
	FLOAT OrgX;
	FLOAT OrgY;
	FLOAT ClipX;
	FLOAT ClipY;
	FLOAT CurX;
	FLOAT CurY;
	FLOAT Z;
	BYTE Style;
	FLOAT CurYL;
	INT DrawColor;
	INT bCenter : 1;
	INT bNoSmooth : 1;
	INT SizeX;
	INT SizeY;
	int ColorModulate[4];
	INT bForceAlpha : 1;
	FLOAT ForcedAlpha;
	INT bRenderLevel : 1;
	UFont* TinyFont;
	UFont* SmallFont;
	UFont* MedFont;
	uint64_t TinyFontName;
	uint64_t SmallFontName;
	uint64_t MedFontName;*/
	c_viewport* Viewport;
	//void* pCanvasUtil;
};