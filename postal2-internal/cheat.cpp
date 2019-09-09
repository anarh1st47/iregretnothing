#include "cheat.hpp"
#include "interfaces.hpp"
#include "hooks.hpp"
#include <Windows.h>
#include <fstream>
#include "uobject.hpp"
#include "fname.hpp"

namespace cheat {
	DWORD WINAPI initialize(void*) {
		interfaces::initialize();
		hooks::initialize();
		std::wofstream fout("names_dump.txt");
		for (auto i = 0; i < interfaces::g_names->size; i++) {
			auto meme2 = interfaces::g_names->at(i);
			if (!meme2)
				continue;
			auto name2 = meme2->WideName;
			fout << name2 << std::endl;
		}
		fout.close();
		//dont work
		std::ofstream fout2("names_dump2.txt");
		for (auto i = 0; i < interfaces::g_objects->size; i++) {
			auto meme2 = interfaces::g_objects->at(i);
			if (!meme2)
				continue;
			auto name2 = meme2->Name;
			fout << name2 << std::endl;
		}
		fout2.close();
		//_asm nop
		return EXIT_SUCCESS;
	};
};