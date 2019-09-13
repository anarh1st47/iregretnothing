#include "cheat.hpp"
#include "interfaces.hpp"
#include "hooks.hpp"
#include <Windows.h>
#include "inputsys.hpp"
#include "menu.hpp"

namespace cheat {
	DWORD WINAPI initialize(void*) {
		input_sys::Initialize();
		interfaces::initialize();
		interfaces::directx();
		menu::initialize();
		hooks::initialize();


		// Panic button
		input_sys::add_hotkey(VK_DELETE, []() {
			cheat::unload();
			});

		// Menu Toggle
		input_sys::add_hotkey(VK_INSERT, []() {
			menu::toggle();
			});

		return EXIT_SUCCESS;
	};

	void unload() {
		hooks::destroy();
		input_sys::destroy();
		FreeLibraryAndExitThread(GetModuleHandle(0), 1);
	}
};