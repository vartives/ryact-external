#pragma once
#include <Windows.h>
#include <TlHelp32.h>
#include <cstdint>
#include <vector>
#include <iostream>
#include <memory>
#include "../imgui/imgui.h"
#include "../imgui/imgui_internal.h"
#include "../imgui/examples/example_win32_directx11/custom_settings.h"

ID3D11ShaderResourceView* sLogo = nullptr;
ID3D11ShaderResourceView* mc_flurry2 = nullptr;
ID3D11ShaderResourceView* Bronze1 = nullptr;
ID3D11ShaderResourceView* Bronze2 = nullptr;
ID3D11ShaderResourceView* Bronze3 = nullptr;
ID3D11ShaderResourceView* Silver1 = nullptr;
ID3D11ShaderResourceView* Silver2 = nullptr;
ID3D11ShaderResourceView* Silver3 = nullptr;
ID3D11ShaderResourceView* Gold1 = nullptr;
ID3D11ShaderResourceView* Gold2 = nullptr;
ID3D11ShaderResourceView* Gold3 = nullptr;
ID3D11ShaderResourceView* Platinum1 = nullptr;
ID3D11ShaderResourceView* Platinum2 = nullptr;
ID3D11ShaderResourceView* Platinum3 = nullptr;
ID3D11ShaderResourceView* Diamond1 = nullptr;
ID3D11ShaderResourceView* Diamond2 = nullptr;
ID3D11ShaderResourceView* Diamond3 = nullptr;
ID3D11ShaderResourceView* Elite = nullptr;
ID3D11ShaderResourceView* Champion = nullptr;
ID3D11ShaderResourceView* Unreal = nullptr;
ID3D11ShaderResourceView* Unranked = nullptr;

ID3D11ShaderResourceView* Fortnite = nullptr;
ImFont* m_pFont;
ImFont* font;
ImFont* smaller_font;
ImFont* Tahoma;
ImVec4 clear_color = ImVec4(0.25f, 0.35f, 0.40f, 1.00f);
ImVec4 accent_color = ImVec4(125.0 / 255.0, 153.0 / 255.0, 155.0 / 255.0, 1.0);
ImU32 accent_color_u32 = ImGui::ColorConvertFloat4ToU32(accent_color);

struct vars {
	bool aimbot_enabled = false;
	float aimbot_fov = 90.0f;
	float aimbot_smoothing = 1.0f;
	int target_bone = 0;
	int filledStyles = 0;

	bool triggerbot_enabled = false;
	float triggerbot_delay = 100.0f;
	bool triggerbot_visible_check = false;

	bool esp_enabled = false;
	float esp_box_color[4] = { 125.0 / 255.0, 153.0 / 255.0, 155.0 / 255.0, 1.0 };
	bool esp_show_health = true;
	bool esp_show_armor = true;

	bool draw_dropped_weapons = true;
	bool draw_explosives = false;
	float max_esp_distance = 500.0f;

	bool enable_logs = true;
	bool auto_save_configs = false;
	float accent_color[4] = { 125.0 / 255.0, 153.0 / 255.0, 155.0 / 255.0, 1.0 };
	float background_color[4] = { 0.1f, 0.1f, 0.1f, 1.0f };

	int selected_config = 0;
	const char* config_list[3] = { "Config 1", "Config 2", "Config 3" };

	float WindowShadowSize = 64.0f;
	float Colors[32][4];
};
vars config;
ImFont* mainfont;
namespace ranks
{
	ImTextureID unranked;

	ImTextureID bronze_1;
	ImTextureID bronze_2;
	ImTextureID bronze_3;

	ImTextureID silver_1;
	ImTextureID silver_2;
	ImTextureID silver_3;

	ImTextureID gold_1;
	ImTextureID gold_2;
	ImTextureID gold_3;

	ImTextureID platin_1;
	ImTextureID platin_2;
	ImTextureID platin_3;

	ImTextureID diamond_1;
	ImTextureID diamond_2;
	ImTextureID diamond_3;

	ImTextureID elite;
	ImTextureID champion;
	ImTextureID unreal;


	ImTextureID android;
	ImTextureID windows;
	ImTextureID xbox;
	ImTextureID linux;
	ImTextureID playstation;
	ImTextureID ios;
	ImTextureID apple;
	ImTextureID switchs;
	ImTextureID* mac;

}