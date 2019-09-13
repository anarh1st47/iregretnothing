#pragma once

#include "imgui/imgui.h"

struct IDirect3DDevice9;

namespace menu {
    void initialize();
    void Shutdown();

    void OnDeviceLost();
    void OnDeviceReset();

    void render();

    void toggle();

	bool is_visible();

    void create_style();

    inline ImGuiStyle        _style;
	inline bool              _visible;
};