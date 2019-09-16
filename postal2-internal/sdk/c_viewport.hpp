#pragma once
struct c_controller;

struct c_viewport {
private:
	char _pad[0x30];
public:
	c_controller* controller;
};