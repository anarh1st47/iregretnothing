#pragma once
#include <Windows.h>
#include "tarray.hpp"
struct FNameEntry;
class UObject;

namespace interfaces {
	inline TArray< UObject* >* g_objects = nullptr;
	inline TArray< FNameEntry* >* g_names = nullptr;
	void initialize();
};
