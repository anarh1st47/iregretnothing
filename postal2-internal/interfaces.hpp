#pragma once
template<typename T>
struct TArray;
struct FNameEntry;
struct u_object;
struct c_pawn;
struct IDirect3DDevice8;

namespace interfaces {
	inline TArray< u_object* >* g_objects = nullptr;
	inline TArray< FNameEntry* >* g_names = nullptr;
	inline IDirect3DDevice8* dx_device = nullptr;
	void directx();
	void initialize();
};
