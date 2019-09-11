#include "interfaces.hpp"
#include "fname.hpp"



void interfaces::initialize() {
	auto core_handle = GetModuleHandleA("Core.dll");
	while(!core_handle)
		core_handle = GetModuleHandleA("Core.dll");
	while(!g_objects || g_objects == nullptr)
		g_objects = (TArray< u_object* >*) GetProcAddress(core_handle, "?GObjObjects@UObject@@0V?$TArray@PAVUObject@@@@A");
	g_names = (TArray< FNameEntry* >*) GetProcAddress(core_handle, "?Names@FName@@0V?$TArray@PAUFNameEntry@@@@A");
}
