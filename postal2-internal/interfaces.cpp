#include <d3dcompiler.h>
#include <dxgi.h>
#include <D3D8Types.h>
#include <d3d8.h>
#include <d3dx8.h>

#include "interfaces.hpp"
#include "sdk/fname.hpp"
#include "cheat.hpp"

void interfaces::directx() {
	//Thanks OBS

	HRESULT hr;
	WNDCLASSEXA wc = { sizeof(WNDCLASSEX), CS_CLASSDC, DefWindowProc, 0L, 0L, GetModuleHandleA(NULL), NULL, NULL, NULL, NULL, "DX", NULL };
	RegisterClassExA(&wc);
	HWND hwnd = CreateWindowA("DX", "cheat", WS_OVERLAPPEDWINDOW, 100, 100, 300, 300, NULL, NULL, wc.hInstance, NULL);
	if (!hwnd) {
		return;
	}

	auto mod = LoadLibraryA("d3d8.dll");
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
		&dx_device);
	if (FAILED(hr)) {
		return;
	}

}

void interfaces::initialize() {
	do {
		cheat::core_handle = GetModuleHandleA("Core.dll");
	} while (!cheat::core_handle);
	do {
		cheat::engine_handle = GetModuleHandleA("Engine.dll");
	} while (!cheat::engine_handle);
	while (!g_objects)
		g_objects = (TArray< u_object* >*) GetProcAddress(cheat::core_handle, "?GObjObjects@UObject@@0V?$TArray@PAVUObject@@@@A");
	while (!g_names)
		g_names = (TArray< FNameEntry* >*) GetProcAddress(cheat::core_handle, "?Names@FName@@0V?$TArray@PAUFNameEntry@@@@A");
}
