#pragma once
#include <cstdint>

struct u_object {
	char pad[0x40];
	uintptr_t class_id;
};