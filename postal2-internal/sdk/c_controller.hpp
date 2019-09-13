#pragma once
struct c_level;
struct c_pawn;
using FName = int;

struct c_controller
{
	char pad_0000[140]; //0x0000
	c_level* level; //0x008C
	char pad_0090[472]; //0x0090
	c_pawn* pawn; //0x0268
	char pad_026C[3544]; //0x026C

	void* find_function(FName name, int unk);
	void* process_event(void* _func, void* params, int unk);
}; //Size: 0x1044