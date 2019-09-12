#include <Windows.h>
#include <thread>
#include <fstream>
#include "interfaces.hpp"
#include "fname.hpp"
#include "uobject.hpp"
#include "hooks.hpp"
#include "cheat.hpp"



BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
	case DLL_PROCESS_ATTACH: {
		CreateThread(0, 0, cheat::initialize, 0, 0, 0);
	}
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
		break;
    case DLL_PROCESS_DETACH:
		hooks::destroy();
        break;
    }
    return TRUE;
}

