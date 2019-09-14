#include <Windows.h>
#include "hooker/hooker.hpp"
#include "hooks.hpp"
#include "cheat.hpp"
#include "interfaces.hpp"
#include "math.hpp"
#include "utils.hpp"
#include "color.hpp"
#include "inputsys.hpp"

#include <d3dcompiler.h>
#include <dxgi.h>
#include <D3D8Types.h>
#include <d3d8.h>
#include <d3dx8.h>
#include "menu.hpp"
#include "imgui/directx8/imgui_impl_dx8.h"

#include "sdk/c_canvas.hpp"
#include "sdk/c_viewport.hpp"
#include "sdk/c_controller.hpp"

#include "hacks/memhacks.hpp"
#include "hacks/esp.hpp"

#pragma comment (lib, "d3dx8.lib")

namespace dx {
	bool is_resizing = false;
}

#define D3D_SDK_VERSION 220


void __fastcall hooks::master_process_tick_h(uinteractionmaster* ecx, void* edx, float delta) {
	static auto ofunc = h.original(&master_process_tick_h);
	if (cheat::canvas) {
		auto viewport = cheat::canvas->viewport;
		if (!viewport)
			return ofunc(ecx, edx, delta);
		auto controller = viewport->controller;
		if (!controller)
			return;
		cheat::localplayer = controller->pawn;
		cheat::level = controller->level;


		if (cheat::localplayer) {
			utils::get_camera();

			hacks::esp::players_esp();
			hacks::memhacks::godmode();
			hacks::memhacks::inf_ammo();
		}
	}
	return ofunc(ecx, edx, delta);
}

void __fastcall hooks::master_process_pre_render_h(uinteractionmaster* ecx, void* edx, c_canvas* canvas) {
	static auto ofunc = h.original(&master_process_pre_render_h);
	cheat::canvas = canvas;
	ofunc(ecx, edx, canvas);
}


BOOL WINAPI hooks::set_cursor_h(int x, int y) {
	static auto ofunc = h.original(set_cursor_h);

	if (menu::is_visible())
		return true;
	return ofunc(x, y);
}

void hooks::initialize() {
	auto master_process_tick_addr = GetProcAddress(cheat::engine_handle, "?MasterProcessTick@UInteractionMaster@@QAEXM@Z");
	h.create_hook(master_process_tick_h, master_process_tick_addr);

	auto master_process_pre_render_addr = GetProcAddress(cheat::engine_handle, "?MasterProcessPreRender@UInteractionMaster@@QAEXPAVUCanvas@@@Z");
	h.create_hook(master_process_pre_render_h, master_process_pre_render_addr);

	h.create_hook(set_cursor_h, SetCursorPos);

	//dx
	auto vtable = *(void**)interfaces::dx_device;
	auto present_addr = (void*)(((int*)vtable)[15]);
	hooks::h.create_hook(hooks::present_h, present_addr);

	auto reset_addr = (void*)(((int*)vtable)[14]);
	hooks::h.create_hook(hooks::reset_h, reset_addr);

	h.enable();
};


int __stdcall hooks::present_h(IDirect3DDevice8* device,
	CONST RECT* src_rect,
	CONST RECT* dst_rect,
	HWND override_window,
	CONST RGNDATA* dirty_region)
{
	static auto present_inited = false;
	static auto ofunc = h.original(present_h);

	if (dx::is_resizing)
		return ofunc(device, src_rect, dst_rect, override_window, dirty_region);
	ImGui_ImplDX8_UpdateDevice(device);
	interfaces::dx_device = device;
	ImGui_ImplDX8_NewFrame();
	menu::render();
	ImGui::Render();
	THRotatorImGui_RenderDrawLists(ImGui::GetDrawData());
	return ofunc(device, src_rect, dst_rect, override_window, dirty_region);
}

int __stdcall hooks::reset_h(IDirect3DDevice8* device, D3DPRESENT_PARAMETERS* pPresentationParameters) {
	dx::is_resizing = true;
	static auto ofunc = h.original(reset_h);

	auto hr = ofunc(device, pPresentationParameters);

	menu::on_device_lost();
	menu::on_device_reset();

	dx::is_resizing = false;
	return hr;
}

void hooks::destroy() {
	h.restore();
}