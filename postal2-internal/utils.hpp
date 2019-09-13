#pragma once
#include <Windows.h>
#include <cstdint>
#include <vector>

struct vec_3d;

namespace utils {
	/*
	 * @brief Scan for a given byte pattern on a module
	 *
	 * @param module    Base of the module to search
	 * @param signature IDA-style byte array pattern
	 *
	 * @returns Address of the first occurence
	 * Credits to @MarkHC
	 */
	std::uint8_t* pattern_scan(void* module, const char* signature);
	void get_camera();
	bool is_screen_pos(const vec_3d& pos);
}