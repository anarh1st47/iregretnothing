#include <Windows.h>
#include "hooker/hooker.hpp"
#include "hooks.hpp"
#include "cheat.hpp"
#include "uobject.hpp"
#include "interfaces.hpp"
#include "math.hpp"
#include "utils.hpp"
#include "memhacks.hpp"
#include "color.hpp"
#include "inputsys.hpp"

#include <d3dcompiler.h>
#include <dxgi.h>
#include <Include/D3D8Types.h>
#include <Include/d3d8.h>

//#define SAFE_RELEASE(x) if (x) { x->Release(); x = NULL; }
#define D3D_SDK_VERSION 220

bool is_screen_pos(const vec_3d& pos) {
	return cheat::canvas && cheat::canvas->clip_x > pos.x && pos.x > 0.f  && cheat::canvas->clip_y > pos.y && pos.y > 0.f;
}

void draw_box(float x1, float y1, float x2, float y2, c_color color) {
	cheat::canvas->util->draw_line(x1, y1, x1, y2, color);
	cheat::canvas->util->draw_line(x1, y1, x2, y1, color);
	cheat::canvas->util->draw_line(x1, y2, x2, y2, color);
	cheat::canvas->util->draw_line(x2, y1, x2, y2, color);
}

void draw_player_esp(c_pawn* player)
{
	float width, left, right, top, bot;
	auto color = c_color(0xffffffff);
	auto pos_top = player->pos;
	auto orig_height = player->height * 2.f;
	pos_top.z += player->height;

	auto pos_bot = player->pos;
	pos_bot.z -= player->height;

	auto pos_top_2d = math::world_to_screen(pos_top);
	if (!is_screen_pos(pos_top_2d))
		return;
	auto pos_bot_2d = math::world_to_screen(pos_bot);
	if (!is_screen_pos(pos_bot_2d))
		return;
	auto scaled_height = std::fabsf(pos_top_2d.y - pos_bot_2d.y);
	auto coef = orig_height / scaled_height;

	width = (player->height / coef) / 1.5;

	left = pos_top_2d.x + width;
	right = pos_top_2d.x - width;
	top = pos_top_2d.y;
	bot = pos_bot_2d.y;

	draw_box(left, top, right, bot, color);

	cheat::canvas->util->draw_text(bot, left, player->name2);
}

void __fastcall hooks::master_process_tick_h(uinteractionmaster* ecx, void* edx, float delta) {
	static auto ofunc = h.original(&master_process_tick_h);
	if (cheat::canvas) {
		auto viewport = cheat::canvas->viewport;
		if(!viewport)
			return ofunc(ecx, edx, delta);
		auto controller = viewport->controller;
		if (!controller)
			return;
		cheat::localplayer = controller->pawn;
		auto level = controller->level;


		if (cheat::localplayer) {
			utils::get_camera();
			level->for_each_player([&](c_pawn* player) -> void {
				if (player->health <= 0)
					return;
				draw_player_esp(player);
				//auto screen_pos = math::world_to_screen(player->pos);
				//draw_box(screen_pos.x - 100, screen_pos.y - 100, screen_pos.x + 100, screen_pos.y + 100);
			});
			
			 
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

	//if (menu::IsVisible())
	//	return true;
	return ofunc(x, y);
}

void hooks::initialize() {
	auto master_process_tick_addr = GetProcAddress(cheat::engine_handle, "?MasterProcessTick@UInteractionMaster@@QAEXM@Z");
	h.create_hook(master_process_tick_h, master_process_tick_addr);

	auto master_process_pre_render_addr = GetProcAddress(cheat::engine_handle, "?MasterProcessPreRender@UInteractionMaster@@QAEXPAVUCanvas@@@Z");
	h.create_hook(master_process_pre_render_h, master_process_pre_render_addr);

	h.create_hook(set_cursor_h, SetCursorPos);

	dx::initialize();

	h.enable();
};


struct IDXGISwapChain;
bool isResizing = false;



int __stdcall hooks::present_h(IDirect3DDevice8* device,
	CONST RECT* src_rect,
	CONST RECT* dst_rect,
	HWND override_window,
	CONST RGNDATA* dirty_region)
{
	static auto present_inited = false;
	static auto ofunc = h.original(present_h);

	if (isResizing)
		return ofunc(device, src_rect, dst_rect, override_window, dirty_region);

	return ofunc(device, src_rect, dst_rect, override_window, dirty_region);
}

int __stdcall hooks::reset_h(IDirect3DDevice8* device, D3DPRESENT_PARAMETERS* pPresentationParameters)
{
	isResizing = true;
	static auto ofunc = h.original(reset_h);


	//ImGui_ImplDX11_InvalidateDeviceObjects();

	//SAFE_RELEASE(RenderTargetView);
	//SAFE_RELEASE(oldRenderTargetView);

	auto hr = ofunc(device, pPresentationParameters);


	//ImGui_ImplDX11_CreateDeviceObjects();

	isResizing = false;
	return hr;
}

LRESULT CALLBACK DXGIMsgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) { return DefWindowProc(hwnd, uMsg, wParam, lParam); }

void hooks::dx::initialize() {
	auto mod = GetModuleHandleA("d3d8.dll");
	HRESULT hr;
	WNDCLASSEXA wc = { sizeof(WNDCLASSEX), CS_CLASSDC, DXGIMsgProc, 0L, 0L, GetModuleHandleA(NULL), NULL, NULL, NULL, NULL, "DX", NULL };
	RegisterClassExA(&wc);
	HWND hwnd = CreateWindowA("DX", "cheat", WS_OVERLAPPEDWINDOW, 100, 100, 300, 300, NULL, NULL, wc.hInstance, NULL);
	if (!hwnd) {
		return;
	}

	if (!mod) {
		return;
	}

	auto create = (IDirect3D8 * (__stdcall*)(uintptr_t)) GetProcAddress(mod, "Direct3DCreate8");
	if (!create) {
		return;
	}

	auto d3d8 = create(D3D_SDK_VERSION);
	if (!d3d8) {
		return;
	}

	D3DPRESENT_PARAMETERS pp = {};
	pp.Windowed = true;
	pp.SwapEffect = D3DSWAPEFFECT_FLIP;
	pp.BackBufferFormat = D3DFMT_A8R8G8B8;
	pp.BackBufferWidth = 2;
	pp.BackBufferHeight = 2;
	pp.BackBufferCount = 1;
	pp.hDeviceWindow = hwnd;
	IDirect3DDevice8* device;

	hr = d3d8->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL,
		hwnd,
		D3DCREATE_HARDWARE_VERTEXPROCESSING, &pp,
		&device);
	if (FAILED(hr)) {
		return;
	}

	auto vtable = *(void**)device;
	auto present_addr = (void*)(((int*)vtable)[15]);
	hooks::h.create_hook(hooks::present_h, present_addr);

	auto reset_addr = (void*)(((int*)vtable)[14]);
	hooks::h.create_hook(hooks::present_h, reset_addr);
}


void hooks::destroy() {
	h.restore();
} 