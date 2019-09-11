#include "interfaces.hpp"
#include "fname.hpp"
#include "cheat.hpp"



void interfaces::initialize() {
	do {
		cheat::core_handle = GetModuleHandleA("Core.dll");
	} while (!cheat::core_handle);
	while(!g_objects)
		g_objects = (TArray< u_object* >*) GetProcAddress(cheat::core_handle, "?GObjObjects@UObject@@0V?$TArray@PAVUObject@@@@A");
	while (!g_names)
		g_names = (TArray< FNameEntry* >*) GetProcAddress(cheat::core_handle, "?Names@FName@@0V?$TArray@PAUFNameEntry@@@@A");
}
