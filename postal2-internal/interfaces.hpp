#pragma once
#include <Windows.h>
#include "tarray.hpp"
struct FNameEntry;
struct u_object;
struct c_pawn;

namespace interfaces {
	inline TArray< u_object* >* g_objects = nullptr;
	inline TArray< FNameEntry* >* g_names = nullptr;
	void initialize();
};
