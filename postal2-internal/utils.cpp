#include "utils.hpp"
#include "cheat.hpp"
#include "math.hpp"

#include "sdk/fname.hpp"
#include "sdk/params.hpp"
#include "sdk/c_canvas.hpp"
#include "sdk/c_viewport.hpp"
#include "sdk/c_controller.hpp"

void utils::get_camera() {
	APlayerController_eventPlayerCalcView_Parms Parms;
	FName PlayerCalcView = 0x659;
	auto pFunc = cheat::canvas->viewport->controller->find_function(PlayerCalcView, FNAME_Find);

	if (pFunc != NULL)
	{
		cheat::canvas->viewport->controller->process_event(pFunc, &Parms, NULL);
		cheat::camera::pos = Parms.CameraLocation;
		cheat::camera::rot = Parms.CameraRotation;
		//ViewActor = Parms.ViewActor;
	}
}

bool utils::is_screen_pos(const vec_3d& pos) {
	return cheat::canvas && cheat::canvas->clip_x > pos.x && pos.x > 0.f && cheat::canvas->clip_y > pos.y && pos.y > 0.f;
}

std::uint8_t* utils::pattern_scan(void* module, const char* signature) {
	static auto pattern_to_byte = [](const char* pattern) {
		auto bytes = std::vector<int>{};
		auto start = const_cast<char*>(pattern);
		auto end = const_cast<char*>(pattern) + strlen(pattern);

		for (auto current = start; current < end; ++current) {
			if (*current == '?') {
				++current;
				if (*current == '?')
					++current;
				bytes.push_back(-1);
			}
			else {
				bytes.push_back(strtoul(current, &current, 16));
			}
		}
		return bytes;
	};

	auto dosHeader = (PIMAGE_DOS_HEADER)module;
	auto ntHeaders = (PIMAGE_NT_HEADERS)((std::uint8_t*)module + dosHeader->e_lfanew);

	auto sizeOfImage = ntHeaders->OptionalHeader.SizeOfImage;
	auto patternBytes = pattern_to_byte(signature);
	auto scanBytes = reinterpret_cast<std::uint8_t*>(module);

	auto s = patternBytes.size();
	auto d = patternBytes.data();

	for (auto i = 0ul; i < sizeOfImage - s; ++i) {
		bool found = true;
		for (auto j = 0ul; j < s; ++j) {
			if (scanBytes[i + j] != d[j] && d[j] != -1) {
				found = false;
				break;
			}
		}
		if (found) {
			return &scanBytes[i];
		}
	}
	return nullptr;
}