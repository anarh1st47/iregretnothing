#include "Menu.hpp"
#define NOMINMAX
#include <Windows.h>
#include <chrono>

//#include "valve_sdk/csgostructs.hpp"
//#include "helpers/input.hpp"
//#include "options.hpp"
#include "ui.hpp"
//#include "config.hpp"
#include "color.hpp"

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui/imgui_internal.h"
#include "imgui/directx8/imgui_impl_dx8.h"
#include "inputsys.hpp"
#include "interfaces.hpp"


// =========================================================
// 
// These are the tabs on the sidebar
// 
// =========================================================
static const char* sidebar_tabs[] = {
    "ESP",
    "AIM",
    "MISC",
    "CONFIG"
};

constexpr static float get_sidebar_item_width() { return 150.0f; }
constexpr static float get_sidebar_item_height() { return  50.0f; }

enum {
	TAB_ESP,
	TAB_AIMBOT,
	TAB_MISC,
	TAB_CONFIG
};

namespace ImGuiEx
{
    inline bool ColorEdit4(const char* label, c_color* v, bool show_alpha = true)
    {
        auto clr = ImVec4 {
            v->r / 255.f,
            v->g / 255.f,
            v->b / 255.f,
            v->a / 255.f
        };

        if(ImGui::ColorEdit4(label, &clr.x, show_alpha)) {
            v->from_imvec4(clr);
            return true;
        }
        return false;
    }
    inline bool ColorEdit3(const char* label, c_color* v)
    {
        return ColorEdit4(label, v, false);
    }
}

template<size_t N>
void render_tabs(const char* (&names)[N], int& activetab, float w, float h, bool sameline)
{
    bool values[N] = { false };

    values[activetab] = true;

    for(auto i = 0; i < N; ++i) {
        if(ImGui::ToggleButton(names[i], &values[i], ImVec2{ w, h })) {
            activetab = i;
        }
        if(sameline && i < N - 1)
            ImGui::SameLine();
    }
}

ImVec2 get_sidebar_size()
{
    constexpr float padding = 10.0f;
    constexpr auto size_w = padding * 2.0f + get_sidebar_item_width();
    constexpr auto size_h = padding * 2.0f + (sizeof(sidebar_tabs) / sizeof(char*)) * get_sidebar_item_height();

    return ImVec2{ size_w, ImMax(325.0f, size_h) };
}

int get_fps()
{
    using namespace std::chrono;
    static int count = 0;
    static auto last = high_resolution_clock::now();
    auto now = high_resolution_clock::now();
    static int fps = 0;

    count++;

    if(duration_cast<milliseconds>(now - last).count() > 1000) {
        fps = count;
        count = 0;
        last = now;
    }

    return fps;
}
void render_empty_tab()
{
	auto& style = ImGui::GetStyle();
	float group_w = ImGui::GetCurrentWindow()->Size.x - style.WindowPadding.x * 2;

	bool placeholder_true = true;

	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
	ImGui::ToggleButton("AIM", &placeholder_true, ImVec2{ group_w, 25.0f });
	ImGui::PopStyleVar();

	ImGui::BeginGroupBox("##body_content");
	{
		auto message = "There's nothing here. Add something you want!";

		 auto pos = ImGui::GetCurrentWindow()->Pos;
		 auto wsize = ImGui::GetCurrentWindow()->Size;

		 pos = pos + wsize / 2.0f;

		 ImGui::RenderText(pos - ImGui::CalcTextSize(message) / 2.0f, message);
	}
	ImGui::EndGroupBox();
}


void menu::initialize()
{
	ImGui::CreateContext();
	//ImGui::GetIO().Fonts->AddFontDefault();
	ImGui::GetIO().Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Arial.ttf", 13);
	ImGui::GetIO().Fonts->Build();
	ImGui_ImplDX8_Init(input_sys::get_main_window(), interfaces::dx_device);
	create_style();
	ImGui_ImplDX8_CreateDeviceObjects();

    _visible = true;
}

void menu::Shutdown()
{
	ImGui_ImplDX8_Shutdown();
	ImGui::DestroyContext();
}

void menu::on_device_lost()
{
    ImGui_ImplDX8_InvalidateDeviceObjects();
}

void menu::on_device_reset()
{
    ImGui_ImplDX8_CreateDeviceObjects();
}

bool menu::is_visible() { return _visible; }

void menu::render()
{
	ImGui::GetIO().MouseDrawCursor = _visible;

    if(!_visible)
        return;

    const auto sidebar_size = get_sidebar_size();
    static int active_sidebar_tab = 0;

    //ImGui::PushStyle(_style);

    ImGui::SetNextWindowPos(ImVec2{ 0, 0 }, ImGuiSetCond_Once);
    ImGui::SetNextWindowSize(ImVec2{ 1000, 0 }, ImGuiSetCond_Once);


	if (ImGui::Begin("I Reget Nothing",
		&_visible,
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoTitleBar)) {

		//auto& style = ImGui::GetStyle();
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0,0));
        {
            ImGui::BeginGroupBox("##sidebar", sidebar_size);
            {
				//ImGui::GetCurrentWindow()->Flags &= ~ImGuiWindowFlags_ShowBorders;

                render_tabs(sidebar_tabs, active_sidebar_tab, get_sidebar_item_width(), get_sidebar_item_height(), false);
            }
            ImGui::EndGroupBox();
        }
        ImGui::PopStyleVar();
        ImGui::SameLine();

        // Make the body the same vertical size as the sidebar
        // except for the width, which we will set to auto
        auto size = ImVec2{ 0.0f, sidebar_size.y };

		ImGui::BeginGroupBox("##body", size);
		render_empty_tab();
        /*if(active_sidebar_tab == TAB_ESP) {
            RenderEspTab();
        } else if(active_sidebar_tab == TAB_AIMBOT) {
            RenderEmptyTab();
        } else if(active_sidebar_tab == TAB_MISC) {
            RenderMiscTab();
        } else if(active_sidebar_tab == TAB_CONFIG) {
            RenderConfigTab();
        }*/
        ImGui::EndGroupBox();

        ImGui::TextColored(ImVec4{ 0.0f, 0.5f, 0.0f, 1.0f }, "FPS: %03d", get_fps());
        ImGui::SameLine(ImGui::GetWindowWidth() - 150 - ImGui::GetStyle().WindowPadding.x);

        ImGui::End();
    }
}

void menu::toggle()
{
    _visible = !_visible;
}

void menu::create_style()
{
	ImGui::StyleColorsDark();
	ImGui::SetColorEditOptions(ImGuiColorEditFlags_HEX);
	_style.FrameRounding = 0.f;
	_style.WindowRounding = 0.f;
	_style.ChildRounding = 0.f;
	_style.Colors[ImGuiCol_Button] = ImVec4(0.260f, 0.590f, 0.980f, 0.670f);
	_style.Colors[ImGuiCol_Header] = ImVec4(0.260f, 0.590f, 0.980f, 0.670f);
	_style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.260f, 0.590f, 0.980f, 1.000f);
	_style.Colors[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.25f, 0.30f, 1.0f);
	_style.Colors[ImGuiCol_WindowBg] = ImVec4(0.000f, 0.009f, 0.120f, 0.940f);
	_style.Colors[ImGuiCol_PopupBg] = ImVec4(0.076f, 0.143f, 0.209f, 1.000f);
	ImGui::GetStyle() = _style;
}

