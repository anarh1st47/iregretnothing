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
#include <D3D8Types.h>
#include <d3d8.h>
#include <d3dx8.h>
#include "menu.hpp"
#include "imgui/directx8/imgui_impl_dx8.h"

#pragma comment (lib, "d3dx8.lib")

#define D3D_SDK_VERSION 220


void draw_box(float x1, float y1, float x2, float y2, c_color color) {
	cheat::canvas->util->draw_line(x1, y1, x1, y2, color);
	cheat::canvas->util->draw_line(x1, y1, x2, y1, color);
	cheat::canvas->util->draw_line(x1, y2, x2, y2, color);
	cheat::canvas->util->draw_line(x2, y1, x2, y2, color);
}

ID3DXFont* pFont = nullptr;

void createFont(IDirect3DDevice8* pDevice)
{
	LOGFONT log_font = {
		20, //height
		0,  //width; 
		0,  // lfEscapement; 
		0,  //lfOrientation; 
		FW_BOLD, // lfWeight; 
		FALSE, // lfItalic; 
		FALSE, // lfUnderline; 
		FALSE, // lfStrikeOut; 
		DEFAULT_CHARSET, // lfCharSet; 
		OUT_DEFAULT_PRECIS, //lfOutPrecision; 
		CLIP_DEFAULT_PRECIS, // lfClipPrecision; 
		ANTIALIASED_QUALITY,// lfQuality; 
		DEFAULT_PITCH,// lfPitchAndFamily; 
		L"Tahoma"// lfFaceName[LF_FACESIZE]; 
	};

	HFONT font = CreateFontA(10, 10, 0, 0, FW_NORMAL, false, false, false, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, 0, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Arial");

	if (D3DXCreateFontIndirect(pDevice, &log_font, &pFont) != D3D_OK)
	{
		//console("D3DXCreateFontIndirect error!");
	}
}


VOID draw_text(const wchar_t* text, int x, int y)
{
	RECT Rect = { x,y,x + 1000,y + 1000 };

	//if (Device_Interface != NULL)
	//{
	//  HFONT Logical_Font_Characteristics = CreateFont(16, 0, 0, 0, 400, 0, 0, 0, 0, 0, 0, 2, 0, "Arial");

	//  if (Logical_Font_Characteristics != NULL)
	//  {
	//      D3DXCreateFont(Device_Interface, Logical_Font_Characteristics, &pFont);
	//      DeleteObject(Logical_Font_Characteristics);
	//  }
	//}

	if (pFont != NULL)
	{
		pFont->Begin();
		pFont->DrawTextW(text, -1, &Rect, 0, 0xFFFF0000);
		pFont->End();
	}
	else {
		createFont(hooks::dx::device);
	}
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
	if (!utils::is_screen_pos(pos_top_2d))
		return;
	auto pos_bot_2d = math::world_to_screen(pos_bot);
	if (!utils::is_screen_pos(pos_bot_2d))
		return;
	auto scaled_height = std::fabsf(pos_top_2d.y - pos_bot_2d.y);
	auto coef = orig_height / scaled_height;

	width = (player->height / coef) / 1.5;

	left = pos_top_2d.x - width;
	right = pos_top_2d.x + width;
	top = pos_top_2d.y;
	bot = pos_bot_2d.y;

	draw_box(left, top, right, bot, color);

	draw_text(player->name2, left, bot);
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
	auto vtable = *(void**)dx::device;
	auto present_addr = (void*)(((int*)vtable)[15]);
	hooks::h.create_hook(hooks::present_h, present_addr);

	auto reset_addr = (void*)(((int*)vtable)[14]);
	hooks::h.create_hook(hooks::reset_h, reset_addr);

	h.enable();
};

void DrawLine(int bx, int by, int bw, D3DCOLOR Color)
{
	D3DRECT rec;
	rec.x1 = bx - bw;//makes line longer/shorter going lef
	rec.y1 = by; // base y
		rec.x2 = bx + bw;//makes line longer/shorter going right
	rec.y2 = by + 10;//makes line one pixel tall
	hooks::dx::device->Clear(1, &rec, D3DCLEAR_TARGET, Color, 0, 0);

}




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
	hooks::dx::device = device;
	ImGui_ImplDX8_NewFrame();
	menu::render();

	/*if (!pFont)
	{
		createFont(device);
	}
	else
	{
		D3DX_Font(device);
	}
*/

	//DrawLine(100, 100, 400, 0xffffffff);
	ImGui::Render();
	//ImGui_ImplDX8_RenderDrawData(ImGui::GetDrawData());
	THRotatorImGui_RenderDrawLists(ImGui::GetDrawData());
	return ofunc(device, src_rect, dst_rect, override_window, dirty_region);
}

int __stdcall hooks::reset_h(IDirect3DDevice8* device, D3DPRESENT_PARAMETERS* pPresentationParameters)
{
	dx::is_resizing = true;
	static auto ofunc = h.original(reset_h);


	//ImGui_ImplDX11_InvalidateDeviceObjects();

	//SAFE_RELEASE(RenderTargetView);
	//SAFE_RELEASE(oldRenderTargetView);

	auto hr = ofunc(device, pPresentationParameters);


	//ImGui_ImplDX11_CreateDeviceObjects();

	dx::is_resizing = false;
	return hr;
}

//Thanks OBS
void hooks::dx::initialize_device() {

	HRESULT hr;
	WNDCLASSEXA wc = { sizeof(WNDCLASSEX), CS_CLASSDC, DefWindowProc, 0L, 0L, GetModuleHandleA(NULL), NULL, NULL, NULL, NULL, "DX", NULL };
	RegisterClassExA(&wc);
	HWND hwnd = CreateWindowA("DX", "cheat", WS_OVERLAPPEDWINDOW, 100, 100, 300, 300, NULL, NULL, wc.hInstance, NULL);
	if (!hwnd) {
		return;
	}

	auto mod = GetModuleHandleA("d3d8.dll");
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
	

	hr = d3d8->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL,
		hwnd,
		D3DCREATE_HARDWARE_VERTEXPROCESSING, &pp,
		&device);
	if (FAILED(hr)) {
		return;
	}
}


void hooks::destroy() {
	h.restore();
} 