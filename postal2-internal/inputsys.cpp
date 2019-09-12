#include "inputsys.hpp"

//#include "imgui/imgui.h"

#include <d3d11.h>
#include <thread>
#include <stdexcept>

//#include "menu.hpp"

//#include "binds.hpp"
//#include "imgui/directx11/imgui_impl_dx11.h"

//extern IMGUI_API LRESULT   ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


void input_sys::destroy()
{
	if (m_ulOldWndProc)
		SetWindowLongPtr(m_hTargetWindow, GWLP_WNDPROC, m_ulOldWndProc);
	m_ulOldWndProc = 0;
}

HWND input_sys::get_main_window() { return m_hTargetWindow; }

void input_sys::set_main_window()
{
	// Get window handle
	while (!(m_hTargetWindow = FindWindowA("Postal2UnrealWWindowsViewportWindow", nullptr)))
	{
		using namespace std::literals::chrono_literals;
		std::this_thread::sleep_for(50ms);
	}
}

void input_sys::Initialize()
{
	set_main_window();
	m_ulOldWndProc = SetWindowLongPtr(m_hTargetWindow, GWLP_WNDPROC, (LONG_PTR)wnd_proc_h);

	if (!m_ulOldWndProc)
		throw std::runtime_error("[InputSys] SetWindowLongPtr failed.");
	/*
	D3DDEVICE_CREATION_PARAMETERS params;

	if(FAILED(g_D3DDevice9->GetCreationParameters(&params)))
		throw std::runtime_error("[InputSys] GetCreationParameters failed.");

	m_hTargetWindow = params.hFocusWindow;
	m_ulOldWndProc  = SetWindowLongPtr(m_hTargetWindow, GWLP_WNDPROC, (LONG_PTR)WndProc);

	if(!m_ulOldWndProc)
		throw std::runtime_error("[InputSys] SetWindowLongPtr failed.");*/
}

LRESULT __stdcall input_sys::wnd_proc_h(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	//ImGuiIO& io = ImGui::GetIO();
	process_message(msg, wParam, lParam);

	//if (Menu::Get().IsVisible())
	//	return ImGui_ImplDX9_WndProcHandler(hWnd, msg, wParam, lParam);


	return CallWindowProcW((WNDPROC)m_ulOldWndProc, hWnd, msg, wParam, lParam);
}

bool input_sys::process_message(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_MBUTTONDBLCLK:
	case WM_RBUTTONDBLCLK:
	case WM_LBUTTONDBLCLK:
	case WM_XBUTTONDBLCLK:
	case WM_MBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_LBUTTONDOWN:
	case WM_XBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_RBUTTONUP:
	case WM_LBUTTONUP:
	case WM_XBUTTONUP:
		return process_mouse_message(uMsg, wParam, lParam);
	case WM_KEYDOWN:
	case WM_KEYUP:
	case WM_SYSKEYDOWN:
	case WM_SYSKEYUP:
		return process_keybd_message(uMsg, wParam, lParam);
	default:
		return false;
	}
}

bool input_sys::process_mouse_message(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	auto key = VK_LBUTTON;
	auto state = KeyState::None;
	switch (uMsg)
	{
	case WM_MBUTTONDBLCLK:
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
		state = uMsg == WM_MBUTTONUP ? KeyState::Up : KeyState::Down;
		key = VK_MBUTTON;
		break;
	case WM_RBUTTONDBLCLK:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
		state = uMsg == WM_RBUTTONUP ? KeyState::Up : KeyState::Down;
		key = VK_RBUTTON;
		break;
	case WM_LBUTTONDBLCLK:
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
		state = uMsg == WM_LBUTTONUP ? KeyState::Up : KeyState::Down;
		key = VK_LBUTTON;
		break;
	case WM_XBUTTONDBLCLK:
	case WM_XBUTTONDOWN:
	case WM_XBUTTONUP:
		state = uMsg == WM_XBUTTONUP ? KeyState::Up : KeyState::Down;
		key = (HIWORD(wParam) == XBUTTON1 ? VK_XBUTTON1 : VK_XBUTTON2);
		break;
	default:
		return false;
	}
	//m_iKeyMap[key] = state;

	//if (state == KeyState::Up) Binds::KeysUp(key);

	if (state == KeyState::Up && m_iKeyMap[key] == KeyState::Down)
		m_iKeyMap[key] = KeyState::Pressed;
	else
		m_iKeyMap[key] = state;
	//Binds::CheckKeys();
	return true;
}

bool input_sys::process_keybd_message(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	auto key = wParam;
	auto state = KeyState::None;

	switch (uMsg)
	{
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		state = KeyState::Down;
		break;
	case WM_KEYUP:
	case WM_SYSKEYUP:
		state = KeyState::Up;
		//Binds::KeysUp(key);
		break;
	default:
		return false;
	}
	//m_iKeyMap[int(key)] = state;

	if (state == KeyState::Up && m_iKeyMap[int(key)] == KeyState::Down)
	{
		m_iKeyMap[int(key)] = KeyState::Pressed;

		auto& hotkey_callback = m_Hotkeys[key];

		if (hotkey_callback)
			hotkey_callback();
	}
	else
	{
		m_iKeyMap[int(key)] = state;
	}
	//Binds::CheckKeys();

	return true;
}

KeyState input_sys::get_key_state(std::uint32_t vk)
{
	return m_iKeyMap[vk];
}

bool input_sys::is_key_down(std::uint32_t vk)
{
	return m_iKeyMap[vk] == KeyState::Down;
}

bool input_sys::was_key_pressed(std::uint32_t vk)
{
	if (m_iKeyMap[vk] == KeyState::Pressed)
	{
		m_iKeyMap[vk] = KeyState::Up;
		return true;
	}
	return false;
}

void input_sys::add_hotkey(std::uint32_t vk, std::function<void(void)> f)
{
	m_Hotkeys[vk] = f;
}

void input_sys::rem_hotkey(std::uint32_t vk)
{
	m_Hotkeys[vk] = nullptr;
}
