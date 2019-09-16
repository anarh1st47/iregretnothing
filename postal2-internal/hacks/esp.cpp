#include <d3dx8.h>
#include <string>

#include "esp.hpp"
#include "../cheat.hpp"
#include "../interfaces.hpp"
#include "../utils.hpp"
#include "../color.hpp"
#include "../options.hpp"


#include "../sdk/c_pawn.hpp"

//testing
#include "../structs.hpp"

namespace testing {


	void draw_box(float x1, float y1, float x2, float y2, c_color color) {
		cheat::canvas->util->draw_line(x1, y1, x1, y2, color);
		cheat::canvas->util->draw_line(x1, y1, x2, y1, color);
		cheat::canvas->util->draw_line(x1, y2, x2, y2, color);
		cheat::canvas->util->draw_line(x2, y1, x2, y2, color);
	}

	ID3DXFont* pFont = nullptr;

	void createFont()
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

		if (D3DXCreateFontIndirect(interfaces::dx_device, &log_font, &pFont) != D3D_OK)
		{
			__debugbreak();
		}
	}


	VOID draw_text(const wchar_t* text, int x, int y)
	{
		RECT Rect = { x,y,x + 1000,y + 1000 };

		if (pFont != NULL ) {
			LPDIRECT3DDEVICE8 font_dev;
			pFont->GetDevice(&font_dev);
			if (font_dev != interfaces::dx_device) {
				pFont->Release();
				pFont = nullptr;
				createFont();
			}
			else {
				pFont->Begin();
				pFont->DrawTextW(text, -1, &Rect, 0, 0xFFFF0000);
				pFont->End();
			}
		}
		else {
			createFont();
		}
	}
}

namespace hacks {
	void draw_player_esp(c_pawn* player);
}

void hacks::draw_player_esp(c_pawn* player) {
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

	if(options::esp_player_box)
		testing::draw_box(left, top, right, bot, color);
	if (options::esp_player_name)
		testing::draw_text(player->name2, left, bot);
	if (options::esp_player_health) {
		testing::draw_text((std::to_wstring(player->health) + L" HP").c_str(), left, top);
		//i thought about health bar but there is no max health defined :lillulmoa:
		//testing::draw_box(right - 5, top, right, bot, color);
		//testing::draw_box(right - 5, top, right, bot, color);
	}
}

void hacks::esp::players_esp() {
	if (!options::esp_player_box && !options::esp_player_name && !options::esp_player_health)
		return;

	cheat::level->for_each_player([&](c_pawn* player) -> void {
		if (player->health <= 0)
			return;
		draw_player_esp(player);
		});
}
