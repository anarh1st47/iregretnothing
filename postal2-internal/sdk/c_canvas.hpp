#pragma once
struct c_font;
struct c_viewport;
struct c_canvas_util;
using FName = int;

struct c_canvas //: public UObject
{
	char pad_0000[40]; //0x0000
	c_font* font; //0x0028
	char pad_002C[24]; //0x002C
	float clip_x; // 44h
	float clip_y; // 48h
	char _pad2[0x24];
	c_font* font1;
	c_font* font2;
	c_font* font3;
	c_font* font4;
	c_viewport* viewport; // 80h
	c_canvas_util* util; // 84h

	void* find_function(FName name, int unk);
	void* process_event(void* _func, void* params, int unk);
};