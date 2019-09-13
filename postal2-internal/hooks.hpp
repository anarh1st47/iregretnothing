#pragma once
#include <cstdint>
#include <d3d8types.h>


struct c_canvas;
struct c_pawn;
struct vec_3d;
struct vec_3d_int;
struct IDXGISwapChain;
struct IDirect3DDevice8;
struct IDirect3D8;

namespace hooks {
	using uinteractionmaster = void;

	void __fastcall master_process_tick_h(uinteractionmaster* ecx, void* edx, float delta);
	void __fastcall master_process_pre_render_h(uinteractionmaster* ecx, void* edx, c_canvas* canvas);
	BOOL __stdcall set_cursor_h(int x, int y);
	void initialize();
	int __stdcall present_h(IDirect3DDevice8* device,
		CONST RECT* src_rect,
		CONST RECT* dst_rect,
		HWND override_window,
		CONST RGNDATA* dirty_region);
	int __stdcall reset_h(IDirect3DDevice8* device, D3DPRESENT_PARAMETERS* pPresentationParameters);
	void destroy();

};