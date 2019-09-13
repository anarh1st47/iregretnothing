#pragma once
struct c_controller;

struct c_viewport {
	char _pad[0x30];
	c_controller* controller;
};