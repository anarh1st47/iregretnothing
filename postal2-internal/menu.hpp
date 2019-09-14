#pragma once

#include "imgui/imgui.h"

struct IDirect3DDevice9;

namespace menu {
    void initialize();
    void Shutdown();

    void on_device_lost();
    void on_device_reset();

    void render();

    void toggle();

	bool is_visible();

    void create_style();

    inline ImGuiStyle        _style;
	inline bool              _visible;
};