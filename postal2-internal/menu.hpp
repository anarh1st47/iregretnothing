#pragma once

#include <string>
#include "imgui/imgui.h"

struct IDirect3DDevice9;

namespace menu {
    void Initialize();
    void Shutdown();

    void OnDeviceLost();
    void OnDeviceReset();

    void Render();

    void Toggle();

	bool IsVisible();

    void CreateStyle();

    inline ImGuiStyle        _style;
	inline bool              _visible;
};