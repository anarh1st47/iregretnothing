#include "cheat.hpp"
#include "interfaces.hpp"
#include "hooks.hpp"
#include <Windows.h>
#include <fstream>
#include "uobject.hpp"
#include "fname.hpp"
#include "inputsys.hpp"
#include "menu.hpp"

namespace cheat {
	DWORD WINAPI initialize(void*) {
		input_sys::Initialize();
		interfaces::initialize();
		hooks::dx::initialize_device();
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
		/*std::wofstream fout("names_dump.txt");
		for (auto i = 0; i < interfaces::g_names->size; i++) {
			auto meme2 = interfaces::g_names->at(i);
			if (!meme2)
				continue;
			auto name2 = meme2->WideName;
			fout << std::hex << meme2->Header.unk0 << " "<< meme2->Header.unk1 << meme2->Header.unk2 << name2 << std::endl;
		}
		fout.close();*/
		//dont work
		//std::ofstream fout2("names_dump2.txt");
		//for (auto i = 0; i < interfaces::g_objects->size; i++) {
		//	auto meme2 = interfaces::g_objects->at(i);
		//	if (!meme2)
		//		continue;
		//	if (meme2->class_id >= 0x33f1 && meme2->class_id < 0x341b) {
		//		auto player = (c_pawn*)meme2;
		//		auto name = player->name;
		//		_asm nop;
		//	}
		//	//auto name2 = meme2->class_id;
		//	//fout << name2 << std::endl;
		//}
		//fout2.close();
		//_asm nop
		return EXIT_SUCCESS;
	};
	void unload() {
		hooks::destroy();
		input_sys::destroy();
		FreeLibraryAndExitThread(0, 1);
	}
};