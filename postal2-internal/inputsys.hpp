#pragma once

#define NOMINMAX
#include <Windows.h>
#include <cstdint>
#include <functional>



enum class KeyState
{
	None = 1,
	Down,
	Up,
	Pressed /*Down and then up*/
};


DEFINE_ENUM_FLAG_OPERATORS(KeyState);

namespace input_sys {

	void Initialize();
	void destroy();
	void set_main_window();
	HWND get_main_window();

	KeyState get_key_state(uint32_t vk);
	bool is_key_down(uint32_t vk);
	bool was_key_pressed(uint32_t vk);

	void add_hotkey(uint32_t vk, std::function<void(void)> f);
	void rem_hotkey(uint32_t vk);

	static LRESULT WINAPI wnd_proc_h(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	bool process_message(UINT uMsg, WPARAM wParam, LPARAM lParam);
	bool process_mouse_message(UINT uMsg, WPARAM wParam, LPARAM lParam);
	bool process_keybd_message(UINT uMsg, WPARAM wParam, LPARAM lParam);


	inline HWND            m_hTargetWindow;
	inline LONG_PTR        m_ulOldWndProc;
	inline KeyState       m_iKeyMap[256];
	inline std::function<void(void)> m_Hotkeys[256];
};