#pragma once

#include <list>
#include <string>
#include "../utility/offsets.h"
#include <string.h>
#include "../sdk/functions.h"
#include <iostream>
#include <vector>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <immintrin.h>
#include "../utility/utils.h"
#include "drawing.h"
#include "../../imgui/xorstr.hpp"
#include "../../imgui/imgui_internal.h"
#include "../../imgui/examples/example_win32_directx11/main_settings.h"
#include "../../imgui/examples/example_win32_directx11/imgui_settings.h"
#include <random>  
#include <ctime>

namespace userinformations
{
	const char* username = "";
	const char* version = "";
	const char* menuname = "";
	std::string expiry = "";
	LPCSTR shopLink = "";
	bool IsBooster = true;
}


void draw_text(ImFont* font, float fontSize, ImVec2 position, ImColor color, const char* text)
{
	ImGui::GetBackgroundDrawList()->AddText(font, fontSize, position, color, text);
}
void draw_text_outlined(ImFont* font, float fontSize, ImVec2 position, ImColor color, const char* text)
{
	ImGui::GetBackgroundDrawList()->AddText(font, fontSize, ImVec2(position.x - 1, position.y - 1), ImColor(0, 0, 0), text);
	ImGui::GetBackgroundDrawList()->AddText(font, fontSize, ImVec2(position.x + 1, position.y - 1), ImColor(0, 0, 0), text);
	ImGui::GetBackgroundDrawList()->AddText(font, fontSize, ImVec2(position.x - 1, position.y + 1), ImColor(0, 0, 0), text);
	ImGui::GetBackgroundDrawList()->AddText(font, fontSize, ImVec2(position.x + 1, position.y + 1), ImColor(0, 0, 0), text);
	ImGui::GetBackgroundDrawList()->AddText(font, fontSize, position, color, text);
}
ImVec2 ScreenCenter(globals.ScreenWidth / 2, globals.ScreenHeight / 2);
static ImColor box_color;

enum MenuTab : int
{
	Combat = 0,
	Visuals = 1,
	Radarington = 2,
	Misc = 3,
	Settings = 4
};
MenuTab eMenuTab;

std::mutex base2;

float TargetDistance = FLT_MAX;
uintptr_t TargetEntity = NULL;
uintptr_t DesyncTargetEntity = NULL;


fvector location;
frotator rotation;
float fov;
inline bool misc_rgb = false;
#define IM_PI 3.14159265358979323846f

ImU32 gradient_color(float hue) {
	float saturation = 1.0f;
	float value = 1.0f;

	ImVec4 color;
	ImGui::ColorConvertHSVtoRGB(hue, saturation, value, color.x, color.y, color.z);
	color.w = 1.0f;

	return IM_COL32(int(color.x * 255), int(color.y * 255), int(color.z * 255), 255);
}
void DrawLine2(int x1, int y1, int x2, int y2, float color[4], int thickness) {

	ImDrawList* drawList = ImGui::GetBackgroundDrawList();

	ImU32 convertedColor = ImGui::ColorConvertFloat4ToU32(ImVec4(color[0], color[1], color[2], color[3]));
	ImU32 convertedOutlineColor = ImGui::ColorConvertFloat4ToU32(ImVec4({ 0.0f, 0.0f, 0.0f, 1.0f }));

	if (visuals.skel_outline)
		drawList->AddLine(ImVec2(x1, y1), ImVec2(x2, y2), IM_COL32(0, 0, 0, 255), thickness + 1.5);
	drawList->AddLine(ImVec2(x1, y1), ImVec2(x2, y2), convertedColor, thickness);
}

void draw_spinning_gradient_circle(float x, float y, float radius, int segments, float thickness, float rotation_speed) {
	ImDrawList* draw_list = ImGui::GetBackgroundDrawList();

	// Calculate rotation angle based on time
	float rotation = ImGui::GetTime() * rotation_speed;

	ImVec2 wheel_center(x, y);

	const float aeps = 0.5f / radius; // Half a pixel arc length in radians (2pi cancels out).
	const int segment_per_arc = ImMax(4, static_cast<int>(radius) / 12);

	for (int n = 0; n < segments; n++) {
		// Calculate the start and end angles for the segment
		const float a0 = (n) / static_cast<float>(segments) * 2.0f * IM_PI - aeps + rotation;
		const float a1 = (n + 1.0f) / static_cast<float>(segments) * 2.0f * IM_PI + aeps + rotation;

		const int vert_start_idx = draw_list->VtxBuffer.Size;
		draw_list->PathArcTo(wheel_center, radius, a0, a1, segment_per_arc);
		draw_list->PathStroke(gradient_color(n / static_cast<float>(segments)), false, thickness);
		const int vert_end_idx = draw_list->VtxBuffer.Size;

		ImVec2 gradient_p0(wheel_center.x + ImCos(a0) * radius, wheel_center.y + ImSin(a0) * radius);
		ImVec2 gradient_p1(wheel_center.x + ImCos(a1) * radius, wheel_center.y + ImSin(a1) * radius);

		// Calculate colors based on the rotation angle
		ImU32 color0 = gradient_color(fmodf(n / static_cast<float>(segments) + rotation / (2 * IM_PI), 1.0f));
		ImU32 color1 = gradient_color(fmodf((n + 1) / static_cast<float>(segments) + rotation / (2 * IM_PI), 1.0f));

		ImGui::ShadeVertsLinearColorGradientKeepAlpha(draw_list, vert_start_idx, vert_end_idx, gradient_p0, gradient_p1, color0, color1);
	}
}
void circle(const fvector2d& pos, int radius, uint32_t color, int segments, float thickness) {
	ImGui::GetBackgroundDrawList()->AddCircle(ImVec2(pos.x, pos.y), radius, color, segments, thickness);
}
void draw_fov_circle(float x, float y, float radius, ImColor color, int segments, float thickness) {
	circle(fvector2d(x, y), radius, color, 99, thickness);
}
inline float custom_sinf(float _X) {
	return _mm_cvtss_f32(_mm_sin_ps(_mm_set_ss(_X)));
}


inline float custom_cosf(float _X) {
	return _mm_cvtss_f32(_mm_cos_ps(_mm_set_ss(_X)));
}

inline float custom_acosf(float _X) {
	return _mm_cvtss_f32(_mm_acos_ps(_mm_set_ss(_X)));
}

inline float custom_tanf(float _X) {
	return _mm_cvtss_f32(_mm_tan_ps(_mm_set_ss(_X)));
}

inline float custom_atan2f(float _X, float _Y) {
	return _mm_cvtss_f32(_mm_atan2_ps(_mm_set_ss(_X), _mm_set_ss(_Y)));
}

inline float custom_atan(float _X) {
	return _mm_cvtss_f32(_mm_atan_ps(_mm_set_ss(_X)));
}


inline float custom_pow(float _X, float _Y) {
	return _mm_cvtss_f32(_mm_pow_ps(_mm_set_ss(_X), _mm_set_ss(_Y)));
}

inline float custom_pow(float _X) {
	return _mm_cvtss_f32(_mm_sqrt_ps(_mm_set_ss(_X)));
}

inline float custom_asinf(float _X) {
	return _mm_cvtss_f32(_mm_asin_ps(_mm_set_ss(_X)));
}

void calc_range(float* x, float* y, float range) {
	
}

fvector2d RotatePoint(fvector2d radar_pos, fvector2d radar_size, fvector LocalLocation, fvector TargetLocation) {
	float DX = TargetLocation.x - LocalLocation.x;
	float DY = TargetLocation.y - LocalLocation.y;

	float x = DY * -1;
	x = -x;
	float y = DX * -1;

	float Range = 34 * 1000;

	calc_range(&x, &y, Range);

	int RadX = radar_pos.x;
	int RadY = radar_pos.y;

	float RadSizeX = radar_size.x;
	float RadSizeY = radar_size.y;

	int MaxX = RadSizeX + RadX - 5;
	int MaxY = RadSizeY + RadY - 5;

	fvector2d ReturnValue = fvector2d();

	ReturnValue.x = RadX + (RadSizeX / 2) + (x / Range * RadSizeX);
	ReturnValue.y = RadY + (RadSizeY / 2) + (y / Range * RadSizeY);

	ReturnValue.x = std::clamp(static_cast<int>(ReturnValue.x), RadX, MaxX);
	ReturnValue.y = std::clamp(static_cast<int>(ReturnValue.y), RadY, MaxY);

	return ReturnValue;
}
void DrawFilledRect(int x, int y, int w, int h, ImU32 color)
{
	ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2(x, y), ImVec2(x + w, y + h), color, 0, 0);
}
void CustomDrawLine(int x1, int y1, int x2, int y2, ImU32 color, int thickness)
{
	ImGui::GetBackgroundDrawList()->AddLine(ImVec2(x1, y1), ImVec2(x2, y2), color, thickness);
}
void DrawLine(int x1, int y1, int x2, int y2, float color[4], int thickness) {

	ImDrawList* drawList = ImGui::GetBackgroundDrawList();

	ImU32 convertedColor = ImGui::ColorConvertFloat4ToU32(ImVec4(color[0], color[1], color[2], color[3]));
	ImU32 convertedOutlineColor = ImGui::ColorConvertFloat4ToU32(ImVec4({ 0.0f, 0.0f, 0.0f, 1.0f }));

	if (visuals.skel_outline)
		drawList->AddLine(ImVec2(x1, y1), ImVec2(x2, y2), IM_COL32(0, 0, 0, 255), thickness + 1.5);
	drawList->AddLine(ImVec2(x1, y1), ImVec2(x2, y2), convertedColor, thickness);
}

void AddLine(ImGuiWindow& window, float width, float height, float a[3], float b[3], ImU32 color, float& minX, float& maxX, float& minY, float& maxY) {
	float ac[3] = { a[0], a[1], a[2] };
	float bc[3] = { b[0], b[1], b[2] };
	window.DrawList->AddLine(ImVec2(ac[0], ac[1]), ImVec2(bc[0], bc[1]), color, 1.0f);

	minX = min(ac[0], minX);
	minX = min(bc[0], minX);

	maxX = max(ac[0], maxX);
	maxX = max(bc[0], maxX);

	minY = min(ac[1], minY);
	minY = min(bc[1], minY);

	maxY = max(ac[1], maxY);
	maxY = max(bc[1], maxY);
}
void skeleton(uint64_t acotrmesh, ImColor col) {

	fvector neck2 = game_helper.getsocketlocation(acotrmesh, 68);
	fvector2d vneck2 = game_helper.ProjectWorldToScreen(neck2);


	fvector neck = game_helper.getsocketlocation(acotrmesh, 67);
	fvector2d vneck = game_helper.ProjectWorldToScreen(neck);
	fvector rightChest = game_helper.getsocketlocation(acotrmesh, 8);
	fvector2d vrightChest = game_helper.ProjectWorldToScreen(rightChest);
	fvector leftChest = game_helper.getsocketlocation(acotrmesh, 37);
	fvector2d vleftChest = game_helper.ProjectWorldToScreen(leftChest);
	fvector leftShoulder = game_helper.getsocketlocation(acotrmesh, 38);
	fvector2d vleftShoulder = game_helper.ProjectWorldToScreen(leftShoulder);
	fvector rightShoulder = game_helper.getsocketlocation(acotrmesh, 9);
	fvector2d vrightShoulder = game_helper.ProjectWorldToScreen(rightShoulder);
	fvector leftElbow = game_helper.getsocketlocation(acotrmesh, 39);
	fvector2d vleftElbow = game_helper.ProjectWorldToScreen(leftElbow);
	fvector rightElbow = game_helper.getsocketlocation(acotrmesh, 10);
	fvector2d vrightElbow = game_helper.ProjectWorldToScreen(rightElbow);
	fvector leftWrist = game_helper.getsocketlocation(acotrmesh, 62);
	fvector2d vleftWrist = game_helper.ProjectWorldToScreen(leftWrist);
	fvector rightWrist = game_helper.getsocketlocation(acotrmesh, 33);
	fvector2d vrightWrist = game_helper.ProjectWorldToScreen(rightWrist);
	fvector pelvis = game_helper.getsocketlocation(acotrmesh, 2);
	fvector2d vpelvis = game_helper.ProjectWorldToScreen(pelvis);
	fvector leftAss = game_helper.getsocketlocation(acotrmesh, 78);
	fvector2d vleftAss = game_helper.ProjectWorldToScreen(leftAss);
	fvector rightAss = game_helper.getsocketlocation(acotrmesh, 71);
	fvector2d vrightAss = game_helper.ProjectWorldToScreen(rightAss);
	fvector leftKnee = game_helper.getsocketlocation(acotrmesh, 79);
	fvector2d vleftKnee = game_helper.ProjectWorldToScreen(leftKnee);
	fvector rightKnee = game_helper.getsocketlocation(acotrmesh, 72);
	fvector2d vrightKnee = game_helper.ProjectWorldToScreen(rightKnee);
	fvector leftAnkle = game_helper.getsocketlocation(acotrmesh, 82);
	fvector2d vleftAnkle = game_helper.ProjectWorldToScreen(leftAnkle);
	fvector rightAnkle = game_helper.getsocketlocation(acotrmesh, 86);
	fvector2d vrightAnkle = game_helper.ProjectWorldToScreen(rightAnkle);

	CustomDrawLine(vleftChest.x, vleftChest.y, vrightChest.x, vrightChest.y, col, visuals.skeleton_thickness);
	CustomDrawLine(vleftChest.x, vleftChest.y, vleftShoulder.x, vleftShoulder.y, col, visuals.skeleton_thickness);
	CustomDrawLine(vrightChest.x, vrightChest.y, vrightShoulder.x, vrightShoulder.y, col, visuals.skeleton_thickness);
	CustomDrawLine(vleftShoulder.x, vleftShoulder.y, vleftElbow.x, vleftElbow.y, col, visuals.skeleton_thickness);
	CustomDrawLine(vrightShoulder.x, vrightShoulder.y, vrightElbow.x, vrightElbow.y, col, visuals.skeleton_thickness);
	CustomDrawLine(vleftElbow.x, vleftElbow.y, vleftWrist.x, vleftWrist.y, col, visuals.skeleton_thickness);
	CustomDrawLine(vrightElbow.x, vrightElbow.y, vrightWrist.x, vrightWrist.y, col, visuals.skeleton_thickness);
	CustomDrawLine(vneck.x, vneck.y, vpelvis.x, vpelvis.y, col, visuals.skeleton_thickness);
	CustomDrawLine(vpelvis.x, vpelvis.y, vleftAss.x, vleftAss.y, col, visuals.skeleton_thickness);
	CustomDrawLine(vpelvis.x, vpelvis.y, vrightAss.x, vrightAss.y, col, visuals.skeleton_thickness);
	CustomDrawLine(vleftAss.x, vleftAss.y, vleftKnee.x, vleftKnee.y, col, visuals.skeleton_thickness);
	CustomDrawLine(vrightAss.x, vrightAss.y, vrightKnee.x, vrightKnee.y, col, visuals.skeleton_thickness);
	CustomDrawLine(vleftKnee.x, vleftKnee.y, vleftAnkle.x, vleftAnkle.y, col, visuals.skeleton_thickness);
	CustomDrawLine(vrightKnee.x, vrightKnee.y, vrightAnkle.x, vrightAnkle.y, col, visuals.skeleton_thickness);
}
void DrawOutlinedText(ImFont* pFont, const std::string& text, const ImVec2& pos, float size, ImU32 color, bool center)
{
	//ImGuiWindow* window = ImGui::GetCurrentWindow();
	auto* DrawList = ImGui::GetBackgroundDrawList();
	std::stringstream stream(text);
	std::string line;

	float y = 0.0f;
	int i = 0;

	while (std::getline(stream, line))
	{
		ImVec2 textSize = pFont->CalcTextSizeA(size, FLT_MAX, 0.0f, line.c_str());

		if (center)
		{
			DrawList->AddText(pFont, size, ImVec2((pos.x - textSize.x / 2.0f) + 1, (pos.y + textSize.y * i) + 1), ImGui::GetColorU32(ImVec4(0, 0, 0, 255)), line.c_str());
			DrawList->AddText(pFont, size, ImVec2((pos.x - textSize.x / 2.0f) - 1, (pos.y + textSize.y * i) - 1), ImGui::GetColorU32(ImVec4(0, 0, 0, 255)), line.c_str());
			DrawList->AddText(pFont, size, ImVec2((pos.x - textSize.x / 2.0f) + 1, (pos.y + textSize.y * i) - 1), ImGui::GetColorU32(ImVec4(0, 0, 0, 255)), line.c_str());
			DrawList->AddText(pFont, size, ImVec2((pos.x - textSize.x / 2.0f) - 1, (pos.y + textSize.y * i) + 1), ImGui::GetColorU32(ImVec4(0, 0, 0, 255)), line.c_str());

			DrawList->AddText(pFont, size, ImVec2(pos.x - textSize.x / 2.0f, pos.y + textSize.y * i), ImGui::GetColorU32(color), line.c_str());
		}
		else
		{
			DrawList->AddText(pFont, size, ImVec2((pos.x) + 1, (pos.y + textSize.y * i) + 1), ImGui::GetColorU32(ImVec4(0, 0, 0, 255)), line.c_str());
			DrawList->AddText(pFont, size, ImVec2((pos.x) - 1, (pos.y + textSize.y * i) - 1), ImGui::GetColorU32(ImVec4(0, 0, 0, 255)), line.c_str());
			DrawList->AddText(pFont, size, ImVec2((pos.x) + 1, (pos.y + textSize.y * i) - 1), ImGui::GetColorU32(ImVec4(0, 0, 0, 255)), line.c_str());
			DrawList->AddText(pFont, size, ImVec2((pos.x) - 1, (pos.y + textSize.y * i) + 1), ImGui::GetColorU32(ImVec4(0, 0, 0, 255)), line.c_str());

			DrawList->AddText(pFont, size, ImVec2(pos.x, pos.y + textSize.y * i), ImGui::GetColorU32(color), line.c_str());
		}

		y = pos.y + textSize.y * (i + 1);
		i++;
	}
}
std::string TextFormat(const char* format, ...)
{
	va_list argptr;
	va_start(argptr, format);

	char buffer[2048];
	vsprintf(buffer, format, argptr);

	va_end(argptr);

	return buffer;
}

bool ShowRadar = true;
bool rect_radar = true;
float radar_position_x{ 25.0f };
float radar_position_y{ 80.0f };
float radar_size{ 200.0f };
float RadarDistance = { 20.f };


void fortnite_radar(float x, float y, float size, bool rect = false)
{
	if (!ShowRadar) return;

	ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize, ImGuiCond_Once);
	const auto flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar |
		ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoBringToFrontOnFocus |
		ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground;
	ImGui::Begin("##radar", nullptr, flags);

	ImDrawList* drawList = ImGui::GetWindowDrawList();
	ImVec2 center = ImVec2(x + size / 2, y + size / 2);
	float radius = size / 2;

	// Colors from your c:: namespace
	ImU32 bgColor = ImGui::GetColorU32(c::background);
	ImU32 borderColor = ImGui::GetColorU32(c::main_color);

	ImVec4 dimmedMain = c::main_color; dimmedMain.w = 0.6f;
	ImVec4 faintMain = c::main_color; faintMain.w = 0.15f;
	ImVec4 glowMain = c::main_color; glowMain.w = 0.1f;

	ImU32 tickColor = ImGui::GetColorU32(dimmedMain);
	ImU32 faintLine = ImGui::GetColorU32(faintMain);
	ImU32 sweepColor = ImGui::GetColorU32(faintMain);

	// Radar base
	drawList->AddCircleFilled(center, radius, bgColor, 64);
	drawList->AddCircle(center, radius, borderColor, 64, 2.5f);

	// Glow ring
	drawList->AddCircle(center, radius + 4, ImGui::GetColorU32(glowMain), 64, 3.0f);

	// Concentric range rings
	for (int i = 1; i <= 3; ++i)
	{
		float r = radius * i / 4.0f;
		drawList->AddCircle(center, r, faintLine, 64, 1.0f);
	}

	// Cross lines
	drawList->AddLine(ImVec2(center.x - radius, center.y), ImVec2(center.x + radius, center.y), faintLine, 1.0f);
	drawList->AddLine(ImVec2(center.x, center.y - radius), ImVec2(center.x, center.y + radius), faintLine, 1.0f);

	// Radar sweep
	float time = ImGui::GetTime();
	float sweepAngle = fmodf(time * 90.0f, 360.0f);
	float rad = sweepAngle * IM_PI / 180.0f;
	ImVec2 sweepEnd = ImVec2(center.x + cosf(rad) * radius, center.y + sinf(rad) * radius);
	drawList->AddLine(center, sweepEnd, sweepColor, 1.2f);

	// Center pulse
	float pulseSize = 4.0f + sinf(time * 3.0f) * 2.0f;
	ImVec4 pulseColorVec = c::main_color;
	pulseColorVec.w = 0.5f + 0.5f * sinf(time * 3.0f);
	ImU32 pulseColor = ImGui::GetColorU32(pulseColorVec);
	drawList->AddCircleFilled(center, pulseSize, pulseColor, 32);

	ImGui::End();
}


void render_elements()
{
	if (globals.showfps || !userinformations::IsBooster)
	{
		
		// Prepare watermark strings
		const char* menu_name = userinformations::menuname;
		const char* version = userinformations::version;

		char fps_text[32];
		std::snprintf(fps_text, sizeof(fps_text), "%.0f FPS", ImGui::GetIO().Framerate);

		// Measure text sizes
		ImVec2 name_size = ImGui::CalcTextSize(menu_name);
		ImVec2 version_size = ImGui::CalcTextSize(version);
		ImVec2 fps_size = ImGui::CalcTextSize("9999 FPS"); // worst-case for layout stability

		// Manually get max width
		float max_width = name_size.x;
		if (version_size.x > max_width) max_width = version_size.x;
		if (fps_size.x > max_width) max_width = fps_size.x;

		// Sizes and layout
		float spacing = 2.0f;
		float line_height = name_size.y + version_size.y + fps_size.y + spacing * 2;
		ImVec2 padding = ImVec2(12, 8);
		ImVec2 fixed_size = ImVec2(max_width + padding.x * 2, line_height + padding.y * 2);

		// Set window
		ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Always);
		ImGui::SetNextWindowSize(fixed_size);
		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(20 / 255.f, 20 / 255.f, 28 / 255.f, 200 / 255.f)); // frosted dark
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 8.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, padding);

		ImGui::Begin("watermark", nullptr,
			ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoScrollbar |
			ImGuiWindowFlags_NoInputs |
			ImGuiWindowFlags_NoSavedSettings |
			ImGuiWindowFlags_NoFocusOnAppearing |
			ImGuiWindowFlags_NoNav);

		// Accent gradient bar (static fade)
		ImDrawList* draw_list = ImGui::GetWindowDrawList();
		ImVec2 pos = ImGui::GetWindowPos();
		ImVec2 size = ImGui::GetWindowSize();
		ImU32 col1 = ImGui::ColorConvertFloat4ToU32(c::main_color);
		ImU32 col2 = ImGui::GetColorU32(ImVec4(c::main_color.x, c::main_color.y, c::main_color.z, 0.1f));
		draw_list->AddRectFilledMultiColor(
			ImVec2(pos.x, pos.y),
			ImVec2(pos.x + 4, pos.y + size.y),
			col1, col1, col2, col2
		);

		// Render text
		ImGui::PushFont(m_pFont);

		float center_x = (fixed_size.x - max_width) * 0.5f;
		float cursor_y = padding.y;

		// menu name (bright white)
		ImGui::SetCursorPos(ImVec2(center_x, cursor_y));
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 1, 1)); // full white
		ImGui::TextUnformatted(menu_name);
		ImGui::PopStyleColor();
		cursor_y += name_size.y + spacing;

		// version (dimmed white)
		ImGui::SetCursorPos(ImVec2(center_x, cursor_y));
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.7f, 0.7f, 0.8f, 0.85f));
		ImGui::TextUnformatted(version);
		ImGui::PopStyleColor();
		cursor_y += version_size.y + spacing;

		// fps (your accent color)
		ImGui::SetCursorPos(ImVec2(center_x, cursor_y));
		ImGui::PushStyleColor(ImGuiCol_Text, c::main_color);
		ImGui::TextUnformatted(fps_text);
		ImGui::PopStyleColor();

		ImGui::PopFont();
		ImGui::End();
		ImGui::PopStyleVar(2);
		ImGui::PopStyleColor(); // window bg
	}


	if (visuals.radar && visuals.enable)
	{
		float radarX = radar_position_x;
		float radarY = radar_position_y;
		float radarSize = radar_size;
		bool isRect = rect_radar;
		fortnite_radar(radarX, radarY, radarSize, isRect);
	}
	
	if (aimbot.drawfov)
	{
		bool isTargeting = Kernel->read<bool>((uintptr_t)WorldCache.AcknownledgedPawn + 0x1e34);
		aimbot.fov_drawdistance = isTargeting ? aimbot.ADS_Fov_Size : aimbot.fovsize;

		if (misc_rgb) {
			draw_spinning_gradient_circle(globals.ScreenWidth / 2, globals.ScreenHeight / 2, aimbot.fovsize, 250, 2.0f, 1.0f);
		}
		else {
			draw_fov_circle(globals.ScreenWidth / 2, globals.ScreenHeight / 2, aimbot.fovsize, ImColor(255, 255, 255), 250, 2.0f);
		
		}

	}

	if (visuals.crosshair && visuals.enable)
	{
		ImDrawList* draw_list = ImGui::GetBackgroundDrawList();

		ImVec2 center(globals.ScreenWidth / 2, globals.ScreenHeight / 2);

		ImU32 crosshair_color_u32 = ImGui::ColorConvertFloat4ToU32(ImColor(255, 255, 255));

		draw_list->AddLine(ImVec2(center.x - 2.0f, center.y), ImVec2(center.x - 9.0f, center.y), crosshair_color_u32, 0.5);
		draw_list->AddLine(ImVec2(center.x + 2.0f, center.y), ImVec2(center.x + 9.0f, center.y), crosshair_color_u32, 0.5);
		draw_list->AddLine(ImVec2(center.x, center.y - 2.0f), ImVec2(center.x, center.y - 9.0f), crosshair_color_u32, 0.5);
		draw_list->AddLine(ImVec2(center.x, center.y + 2.0f), ImVec2(center.x, center.y + 9.0f), crosshair_color_u32, 0.5);

		draw_list->AddLine(ImVec2(center.x - 2.0f - 1, center.y), ImVec2(center.x - 9.0f + 1, center.y), crosshair_color_u32, 0.5 - 1);
		draw_list->AddLine(ImVec2(center.x + 2.0f + 1, center.y), ImVec2(center.x + 9.0f - 1, center.y), crosshair_color_u32, 0.5 - 1);
		draw_list->AddLine(ImVec2(center.x, center.y - 2.0f - 1), ImVec2(center.x, center.y - 9.0f + 1), crosshair_color_u32, 0.5 - 1);
		draw_list->AddLine(ImVec2(center.x, center.y + 2.0f + 1), ImVec2(center.x, center.y + 9.0f - 1), crosshair_color_u32, 0.5 - 1);
	}
}

void line(const ImVec2& from, const ImVec2& to, ImColor color, float thickness) {
	ImGui::GetBackgroundDrawList()->AddLine(from, to, color, thickness);
}

static auto draw_crosshair() -> void
{
	ImDrawList* draw_list = ImGui::GetBackgroundDrawList();

	draw_list->AddLine(ImVec2(globals.ScreenWidth / 2 - 3, globals.ScreenHeight / 2), ImVec2(globals.ScreenWidth / 2 - 8, globals.ScreenHeight / 2), ImColor(255, 255, 255), 1.5);
	draw_list->AddLine(ImVec2(globals.ScreenWidth / 2 + 3, globals.ScreenHeight / 2), ImVec2(globals.ScreenWidth / 2 + 8, globals.ScreenHeight / 2), ImColor(255, 255, 255), 1.5);
	draw_list->AddLine(ImVec2(globals.ScreenWidth / 2, globals.ScreenHeight / 2 - 3), ImVec2(globals.ScreenWidth / 2, globals.ScreenHeight / 2 - 8), ImColor(255, 255, 255), 1.5);
	draw_list->AddLine(ImVec2(globals.ScreenWidth / 2, globals.ScreenHeight / 2 + 3), ImVec2(globals.ScreenWidth / 2, globals.ScreenHeight / 2 + 8), ImColor(255, 255, 255), 1.5);

}


std::string get_rank_name(int tier)
{
	switch (tier)
	{
	case 0: return "Bronze 1";
	case 1: return "Bronze 2";
	case 2: return "Bronze 3";
	case 3: return "Silver 1";
	case 4: return "Silver 2";
	case 5: return "Silver 3";
	case 6: return "Gold 1";
	case 7: return "Gold 2";
	case 8: return "Gold 3";
	case 9: return "Platinum 1";
	case 10: return "Platinum 2";
	case 11: return "Platinum 3";
	case 12: return "Diamond 1";
	case 13: return "Diamond 2";
	case 14: return "Diamond 3";
	case 15: return "Elite";
	case 16: return "Champion";
	case 17: return "Unreal";
	default: return "Unranked";
	}
}

ImVec4 get_rank_color(int tier)
{
	switch (tier)
	{
	case 0: return ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
	case 1:
	case 2: return ImVec4(0.902f, 0.580f, 0.227f, 1.0f);
	case 3:
	case 4:
	case 5: return ImVec4(0.843f, 0.843f, 0.843f, 1.0f);
	case 6:
	case 7:
	case 8: return ImVec4(1.0f, 0.871f, 0.0f, 1.0f);
	case 9:
	case 10:
	case 11: return ImVec4(0.0f, 0.7f, 0.7f, 1.0f);
	case 12:
	case 13:
	case 14: return ImVec4(0.1686f, 0.3294f, 0.8235f, 1.0f);
	case 15: return ImVec4(0.7f, 0.7f, 0.7f, 1.0f);
	case 16: return ImVec4(1.0f, 0.6f, 0.0f, 1.0f);
	case 17: return ImVec4(0.6f, 0.0f, 0.6f, 1.0f);
	default: return ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
	}
}
enum bone : int
{
	HumanBase = 0,
	HumanPelvis = 2,
	HumanLThigh1 = 71,
	HumanLThigh2 = 77,
	HumanLThigh3 = 72,
	HumanLCalf = 74,
	HumanLFoot2 = 122,
	HumanLFoot = 122,
	HumanLToe = 76,
	HumanRThigh1 = 78,
	HumanRThigh2 = 84,
	HumanRThigh3 = 79,
	HumanRCalf = 81,
	HumanRFoot2 = 82,
	HumanRFoot = 87,
	HumanRToe = 83,
	HumanSpine1 = 7,
	HumanSpine2 = 5,
	HumanSpine3 = 2,
	HumanLCollarbone = 9,
	HumanLUpperarm = 35,
	HumanLForearm1 = 36,
	HumanLForearm23 = 10,
	HumanLForearm2 = 34,
	HumanLForearm3 = 33,
	HumanLPalm = 32,
	HumanLHand = 11,
	HumanRCollarbone = 98,
	HumanRUpperarm = 64,
	HumanRForearm1 = 65,
	HumanRForearm23 = 39,
	HumanRForearm2 = 63,
	HumanRForearm3 = 62,
	HumanRHand = 40,
	HumanRPalm = 58,
	HumanNeck = 66,
	HumanHead = 110,
	HumanLowerHead = 106,

	HumanChest = 65
};





namespace aimb
{
	bool prediction_pistol = false;
	bool trigb_pistol = false;
	int smoothing_pistol = 1;
	int fov_size_pistol = 100;

	bool prediction_pump = false;
	bool trigb_pump = false;
	int smoothing_pump = 1;
	int fov_size_pump = 100;

	bool prediction_rifle = false;
	bool trigb_rifle = false;
	int smoothing_rifle = 1;
	int fov_size_rifle = 100;

	bool prediction_sniper = false;
	bool trigb_sniper = false;
	int smoothing_sniper = 1;
	int fov_size_sniper = 100;

	bool prediction_shotgun = false;
	bool trigb_shotgun = false;
	int smoothing_shotgun = 1;
	int fov_size_shotgun = 100;

	bool prediction_other = false;
	bool trigb_other = false;
	int smoothing_other = 1;
	int fov_size_other = 100;

	bool prediction_smg = false;
	bool trigb_smg = false;
	int smoothing_smg = 1;
	int fov_size_smg = 100;
}

void weaponconfig(std::string weapon) { 

	if (weapon.find("Pistol") != std::string::npos || weapon.find("Hand Cannon") != std::string::npos) {
		aimbot.smoothsize = aimb::smoothing_pistol;
		aimbot.fovsize = aimb::fov_size_pistol;
		triggerbot.triggerbot_enable = aimb::trigb_pistol;
		aimbot.prediction = aimb::prediction_pistol;

	}
	else if (weapon.find("Assault Rifle") != std::string::npos || weapon.find("AR") != std::string::npos || weapon.find("Rifle") != std::string::npos) {
		aimbot.smoothsize = aimb::smoothing_rifle;
		aimbot.fovsize = aimb::fov_size_rifle;
		triggerbot.triggerbot_enable = aimb::trigb_rifle;
		aimbot.prediction = aimb::prediction_rifle;
	}
	else if (weapon.find("SMG") != std::string::npos || weapon.find("Submachine Gun") != std::string::npos) {
		aimbot.smoothsize = aimb::smoothing_smg;
		aimbot.fovsize = aimb::fov_size_smg;
		triggerbot.triggerbot_enable = aimb::trigb_smg;
		aimbot.prediction = aimb::prediction_smg;
	}
	else if (weapon.find("Sniper") != std::string::npos || weapon.find("DMR") != std::string::npos) {
		aimbot.smoothsize = aimb::smoothing_sniper;
		aimbot.fovsize = aimb::fov_size_sniper;
		triggerbot.triggerbot_enable = aimb::trigb_sniper;
		aimbot.prediction = aimb::prediction_sniper;
	}
	else if (weapon.find("Pump Shotgun") != std::string::npos || weapon.find("Thunder Shotgun") != std::string::npos || weapon.find("Tactical Shotgun") != std::string::npos) {
		aimbot.smoothsize = aimb::smoothing_sniper;
		aimbot.fovsize = aimb::fov_size_sniper;
		triggerbot.triggerbot_enable = aimb::trigb_sniper;
		aimbot.prediction = aimb::prediction_sniper;
	}
	else {

		aimbot.smoothsize = aimb::smoothing_other;
		aimbot.fovsize = aimb::fov_size_other;
		triggerbot.triggerbot_enable = aimb::trigb_other;
		aimbot.prediction = aimb::prediction_other;
	}
}

void radar_range(float* x, float* y, float range)
{
	if (fabs((*x)) > range || fabs((*y)) > range)
	{
		if ((*y) > (*x))
		{
			if ((*y) > -(*x))
			{
				(*x) = range * (*x) / (*y);
				(*y) = range;
			}
			else
			{
				(*y) = -range * (*y) / (*x);
				(*x) = -range;
			}
		}
		else
		{
			if ((*y) > -(*x))
			{
				(*y) = range * (*y) / (*x);
				(*x) = range;
			}
			else
			{
				(*x) = -range * (*x) / (*y);
				(*y) = -range;
			}
		}
	}
}



#define _
float GetProjectileSpeed(std::string DisplayName)
{
	return 0;
}

float GetGravityScale(std::string DisplayName)
{


	return 0;
}

bool carFly = false;
void project_world_to_radar(fvector vOrigin, int& screenx, int& screeny)
{
	fvector vAngle = camera_postion.rotation;
	auto fYaw = vAngle.y * M_PI / 180.0f;
	float dx = vOrigin.x - camera_postion.location.x;
	float dy = vOrigin.y - camera_postion.location.y;
	float fsin_yaw = sinf(fYaw);
	float fminus_cos_yaw = -cosf(fYaw);
	float x = dy * fminus_cos_yaw + dx * fsin_yaw;
	x = -x;
	float y = dx * fminus_cos_yaw - dy * fsin_yaw;
	//float range = (float)(250.0f / 5) * 1000.f;
	float range = (float)(50.0f / 5) * 1000.f;
	radar_range(&x, &y, range);

	ImVec2 DrawPos = ImVec2(10.0f, 60.0f);
	ImVec2 DrawSize = ImVec2(250.0f, 250.0f);
	float radarRadius = 250.0f / 2;
	float distance = sqrt(pow(screenx - DrawPos.x, 2) + pow(screeny - DrawPos.y, 2));

	if (distance > radarRadius)
	{
		float angle = atan2(screeny - DrawPos.y, screenx - DrawPos.x);
		screenx = static_cast<int>(DrawPos.x + radarRadius * cos(angle));
		screeny = static_cast<int>(DrawPos.y + radarRadius * sin(angle));
	}

	int rad_x = (int)DrawPos.x;
	int rad_y = (int)DrawPos.y;
	float r_siz_x = DrawSize.x;
	float r_siz_y = DrawSize.y;
	int x_max = (int)r_siz_x + rad_x - 5;
	int y_max = (int)r_siz_y + rad_y - 5;

	screenx = rad_x + ((int)r_siz_x / 2 + int(x / range * r_siz_x));
	screeny = rad_y + ((int)r_siz_y / 2 + int(y / range * r_siz_y));

	if (screenx > x_max)
		screenx = x_max;

	if (screenx < rad_x)
		screenx = rad_x;

	if (screeny > y_max)
		screeny = y_max;

	if (screeny < rad_y)
		screeny = rad_y;
}

void RadarRange(float* x, float* y, float range)
{
	if (fabs((*x)) > range || fabs((*y)) > range)
	{
		if ((*y) > (*x))
		{
			if ((*y) > -(*x))
			{
				(*x) = range * (*x) / (*y);
				(*y) = range;
			}
			else
			{
				(*y) = -range * (*y) / (*x);
				(*x) = -range;
			}
		}
		else
		{
			if ((*y) > -(*x))
			{
				(*y) = range * (*y) / (*x);
				(*x) = range;
			}
			else
			{
				(*x) = -range * (*x) / (*y);
				(*y) = -range;
			}
		}
	}
}

void CalcRadarPoint(fvector vOrigin, int& screenx, int& screeny)
{
	game_helper.get_camera();
	fvector vAngle = camera_postion.rotation;
	auto fYaw = vAngle.y * M_PI / 180.0f;
	float dx = vOrigin.x - camera_postion.location.x;
	float dy = vOrigin.y - camera_postion.location.y;

	float fsin_yaw = sinf(fYaw);
	float fminus_cos_yaw = -cosf(fYaw);

	float x = dy * fminus_cos_yaw + dx * fsin_yaw;
	x = -x;
	float y = dx * fminus_cos_yaw - dy * fsin_yaw;

	float range = (float)RadarDistance * 1000.f;

	RadarRange(&x, &y, range);

	ImVec2 DrawPos = ImVec2(radar_position_x, radar_position_y);
	ImVec2 DrawSize = ImVec2(radar_size, radar_size);


	int rad_x = (int)DrawPos.x;
	int rad_y = (int)DrawPos.y;

	float r_siz_x = DrawSize.x;
	float r_siz_y = DrawSize.y;

	int x_max = (int)r_siz_x + rad_x - 5;
	int y_max = (int)r_siz_y + rad_y - 5;

	screenx = rad_x + ((int)r_siz_x / 2 + int(x / range * r_siz_x));
	screeny = rad_y + ((int)r_siz_y / 2 + int(y / range * r_siz_y));

	if (screenx > x_max)
		screenx = x_max;

	if (screenx < rad_x)
		screenx = rad_x;

	if (screeny > y_max)
		screeny = y_max;

	if (screeny < rad_y)
		screeny = rad_y;
}

void add_players_radar(fvector WorldLocation)
{
	if (!ShowRadar) return;

	ImGui::Begin("##radar", nullptr, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDecoration);

	int ScreenX, ScreenY;
	ImDrawList* drawList = ImGui::GetWindowDrawList();
	CalcRadarPoint(WorldLocation, ScreenX, ScreenY);

	ImVec2 center = ImVec2(radar_position_x + radar_size / 2, radar_position_y + radar_size / 2);
	float radius = radar_size / 2.0f;

	// Offset from radar center
	float dx = (float)ScreenX - center.x;
	float dy = (float)ScreenY - center.y;
	float dist = sqrtf(dx * dx + dy * dy);

	// Clamp to radar circle
	if (dist > radius - 4.0f) {
		float scale = (radius - 4.0f) / dist;
		dx *= scale;
		dy *= scale;
	}
	ImVec2 clampedPos = ImVec2(center.x + dx, center.y + dy);


	ImVec4 colorVec = aimbot.vischeck ?
		ImVec4(vscolors.box_visible[0], vscolors.box_visible[1], vscolors.box_visible[2], 1.0f) :
		ImVec4(vscolors.box_invisible[0], vscolors.box_invisible[1], vscolors.box_invisible[2], 1.0f);

	drawList->AddCircleFilled(clampedPos, 3.0f, ImGui::GetColorU32(colorVec), 12);


	ImGui::End();
}

void draw_line(fvector2d start, fvector2d end, ImU32 color, float thickness)
{
	ImGui::GetBackgroundDrawList()->AddLine(ImVec2(start.x, start.y), ImVec2(end.x, end.y), color, thickness);
}

void draw_skeleton(uintptr_t mesh)
{
	std::vector<fvector> bones = {
		game_helper.getsocketlocation(mesh, 67),  // neck
		game_helper.getsocketlocation(mesh, 9),   // left shoulder
		game_helper.getsocketlocation(mesh, 10),  // left elbow
		game_helper.getsocketlocation(mesh, 11),  // left hand
		game_helper.getsocketlocation(mesh, 38),  // right shoulder
		game_helper.getsocketlocation(mesh, 39),  // right elbow
		game_helper.getsocketlocation(mesh, 40),  // right hand
		game_helper.getsocketlocation(mesh, 2),   // pelvis
		game_helper.getsocketlocation(mesh, 71),  // left hip
		game_helper.getsocketlocation(mesh, 72),  // left knee
		game_helper.getsocketlocation(mesh, 78),  // right hip
		game_helper.getsocketlocation(mesh, 79),  // right knee
		game_helper.getsocketlocation(mesh, 75),  // left foot
		game_helper.getsocketlocation(mesh, 82),  // right foot
		game_helper.getsocketlocation(mesh, 11)  // head
	};

	std::vector<fvector2d> screenPositions(bones.size());
	for (size_t i = 0; i < bones.size(); ++i) {
		screenPositions[i] = game_helper.ProjectWorldToScreen(bones[i]);
	}

  if (visuals.skel_outline)
  {
    // outline
    draw_line(screenPositions[1], screenPositions[4], ImColor(0, 0, 0), 2);   // left shoulder to right shoulder
    draw_line(screenPositions[1], screenPositions[2], ImColor(0, 0, 0), 2);   // left shoulder to left elbow
    draw_line(screenPositions[2], screenPositions[3], ImColor(0, 0, 0), 2);   // left elbow to left hand
    draw_line(screenPositions[4], screenPositions[5], ImColor(0, 0, 0), 2);   // right shoulder to right elbow
    draw_line(screenPositions[5], screenPositions[6], ImColor(0, 0, 0), 2);   // right elbow to right hand
    draw_line(screenPositions[0], screenPositions[7], ImColor(0, 0, 0), 2);   // neck to pelvis
    draw_line(screenPositions[7], screenPositions[8], ImColor(0, 0, 0), 2);   // pelvis to left hip
    draw_line(screenPositions[8], screenPositions[9], ImColor(0, 0, 0), 2);   // left hip to left knee
    draw_line(screenPositions[9], screenPositions[12], ImColor(0, 0, 0), 2);  // left knee to left foot
    draw_line(screenPositions[7], screenPositions[10], ImColor(0, 0, 0), 2);  // pelvis to right hip
    draw_line(screenPositions[10], screenPositions[11], ImColor(0, 0, 0), 2); // right hip to right knee
    draw_line(screenPositions[11], screenPositions[13], ImColor(0, 0, 0), 2); // right knee to right foot
  }
	// skeleton
	draw_line(screenPositions[1], screenPositions[4], box_color, visuals.skeleton_thickness);   // left shoulder to right shoulder
	draw_line(screenPositions[1], screenPositions[2], box_color, visuals.skeleton_thickness);   // left shoulder to left elbow
	draw_line(screenPositions[2], screenPositions[3], box_color, visuals.skeleton_thickness);   // left elbow to left hand
	draw_line(screenPositions[4], screenPositions[5], box_color, visuals.skeleton_thickness);   // right shoulder to right elbow
	draw_line(screenPositions[5], screenPositions[6], box_color, visuals.skeleton_thickness);   // right elbow to right hand
	draw_line(screenPositions[0], screenPositions[7], box_color, visuals.skeleton_thickness);   // neck to pelvis
	draw_line(screenPositions[7], screenPositions[8], box_color, visuals.skeleton_thickness);   // pelvis to left hip
	draw_line(screenPositions[8], screenPositions[9], box_color, visuals.skeleton_thickness);   // left hip to left knee
	draw_line(screenPositions[9], screenPositions[12], box_color, visuals.skeleton_thickness);  // left knee to left foot
	draw_line(screenPositions[7], screenPositions[10], box_color, visuals.skeleton_thickness);  // pelvis to right hip
	draw_line(screenPositions[10], screenPositions[11], box_color, visuals.skeleton_thickness); // right hip to right knee
	draw_line(screenPositions[11], screenPositions[13], box_color, visuals.skeleton_thickness); // right knee to right foot
}
std::string platform(uint64_t PlayerState)
{
	DWORD_PTR test_platform = Kernel->read<DWORD_PTR>(PlayerState + 0x430); //0x438 FortniteGame.FortPlayerState.Platform -> 0x408
	if (!test_platform) return std::string("AI");
	wchar_t platform[64];
	// Kernel->ReadMemory((PVOID)test_platform, (size_t*)platform, sizeof(platform));
	std::wstring platform_wstr(platform);
	std::string platform_str(platform_wstr.begin(), platform_wstr.end());
	return platform_str;
}

void draw_head(ImColor color, fvector head_location)
{
	if (!WorldCache.AcknownledgedPawn)
	{
		fvector2d head_2d = game_helper.ProjectWorldToScreen(fvector(head_location.x, head_location.y, head_location.z + 20));
		fvector delta = head_location - camera_postion.location;

		float distance = delta.length();
		const float constant_circle_size = 10;
		float circle_radius = constant_circle_size * (globals.ScreenHeight / (2.0f * distance * tanf(camera_postion.fov * (float)M_PI / 360.f)));
		float y_offset = +85.0f;
		head_2d.y += y_offset;
		int segments = 50;

		//	if (visuals.skel_outline) ImGui::GetBackgroundDrawList()->AddCircle(ImVec2(head_2d.x, head_2d.y), 7.0f, ImColor(0, 0, 0, 255), segments, visuals.skeleton_thickness + 2.0f);
		//	ImGui::GetBackgroundDrawList()->AddCircle(ImVec2(head_2d.x, head_2d.y), 7.0f, color, segments, visuals.skeleton_thickness);
		if (visuals.skel_outline) ImGui::GetBackgroundDrawList()->AddCircle(ImVec2(head_2d.x, head_2d.y), circle_radius, ImColor(0, 0, 0, 255), segments, visuals.skeleton_thickness + 2.0f);
		ImGui::GetBackgroundDrawList()->AddCircle(ImVec2(head_2d.x, head_2d.y), circle_radius, color, segments, visuals.skeleton_thickness);
	}
	else
	{
		fvector2d head_2d = game_helper.ProjectWorldToScreen(fvector(head_location.x, head_location.y, head_location.z));
		fvector delta = head_location - camera_postion.location;

		float distance = delta.length();
		const float constant_circle_size = 10;
		float circle_radius = constant_circle_size * (globals.ScreenHeight / (2.0f * distance * tanf(camera_postion.fov * (float)M_PI / 360.f)));
		float y_offset = +12.0f;
		int segments = 50;
		if (visuals.skel_outline) ImGui::GetBackgroundDrawList()->AddCircle(ImVec2(head_2d.x, head_2d.y), circle_radius, ImColor(0, 0, 0, 255), segments, visuals.skeleton_thickness + 2.0f);
		ImGui::GetBackgroundDrawList()->AddCircle(ImVec2(head_2d.x, head_2d.y), circle_radius, color, segments, visuals.skeleton_thickness);
		//if (visuals.skel_outline) ImGui::GetBackgroundDrawList()->AddCircle(ImVec2(head_2d.x, head_2d.y), 7.0f, ImColor(0, 0, 0, 255), segments, visuals.skeleton_thickness + 2.0f);
		//ImGui::GetBackgroundDrawList()->AddCircle(ImVec2(head_2d.x, head_2d.y), 7.0f, color, segments, visuals.skeleton_thickness);
	}
}
bool is_dead(uintptr_t pawn_private)
{
	return (Kernel->read<char>(pawn_private + offsets::b_is_dying) >> 5) & 1;
}
class UFortWeaponItemDefinition
{
};

class AFortWeapon
{
public:
	float GetProjectileGravity()
	{
		return *(float*)(this + 0x1A24);
	}

	float GetProjectileSpeed()
	{
		return *(float*)(this + 0x1DB4);
	}

	UFortWeaponItemDefinition* WeaponData()
	{
		return *(UFortWeaponItemDefinition**)(this + offsets::weapon_data);
	}
};
AFortWeapon* weapon;

void DrawRoundedRect(int x, int y, int w, int h, ImU32 color, int thickness)
{
	ImGui::GetBackgroundDrawList()->AddRect(ImVec2(x, y), ImVec2(w, h), ImGui::ColorConvertFloat4ToU32(ImVec4(1 / 255.0, 1 / 255.0, 1 / 255.0, 255 / 255.0)), 0, 0, 3);
	ImGui::GetBackgroundDrawList()->AddRect(ImVec2(x, y), ImVec2(w, h), ImGui::GetColorU32(color), 0, 0, thickness);
}
struct PlayerBounds {
	float left, right, top, bottom;
	uintptr_t pawn_priv;
};
struct PlayerBones {
	fvector2d screen;
	int index;
	bool on_screen;
	bool head_bone = false;
};
#include <array>
static std::array<PlayerBones, 26> BonesArray = {
PlayerBones{ fvector2d{}, bone::HumanHead, false, true }, PlayerBones{ fvector2d{}, bone::HumanNeck, false },
PlayerBones{ fvector2d{}, bone::HumanSpine3, false },  PlayerBones{ fvector2d{}, bone::HumanSpine2, false },
PlayerBones{ fvector2d{}, bone::HumanSpine1, false },  PlayerBones{ fvector2d{}, bone::HumanPelvis, false },
PlayerBones{ fvector2d{}, bone::HumanLToe, false },  PlayerBones{ fvector2d{}, bone::HumanLFoot, false },
PlayerBones{ fvector2d{}, bone::HumanLCalf, false },  PlayerBones{ fvector2d{}, bone::HumanLThigh2, false },
PlayerBones{ fvector2d{}, bone::HumanLThigh1, false },  PlayerBones{ fvector2d{}, bone::HumanPelvis, false },
PlayerBones{ fvector2d{}, bone::HumanRThigh1, false },  PlayerBones{ fvector2d{}, bone::HumanRThigh2, false },
PlayerBones{ fvector2d{}, bone::HumanRCalf, false },  PlayerBones{ fvector2d{}, bone::HumanRFoot, false },
PlayerBones{ fvector2d{}, bone::HumanRToe, false },  PlayerBones{ fvector2d{}, bone::HumanLPalm, false },
PlayerBones{ fvector2d{}, bone::HumanLForearm1, false },  PlayerBones{ fvector2d{}, bone::HumanLUpperarm, false },
PlayerBones{ fvector2d{}, bone::HumanRUpperarm, false }, PlayerBones{ fvector2d{}, bone::HumanLForearm23, false },
PlayerBones{ fvector2d{}, bone::HumanLForearm2, false },
PlayerBones{ fvector2d{}, bone::HumanLForearm3, false },
PlayerBones{ fvector2d{}, bone::HumanRForearm2, false },
PlayerBones{ fvector2d{}, bone::HumanRForearm23, false }
};
#define bones bone
bool GetPlayerBoundings(uintptr_t Mesh, PlayerBounds* Boundings) {
	const auto get_bounds = [&](PlayerBounds& out, float expand = 0.f) -> bool {
		PlayerBounds Bounds = { FLT_MAX, FLT_MIN, FLT_MAX, FLT_MIN };

		for (auto& bone : BonesArray) {
			fvector bone_pos = game_helper.getsocketlocation(Mesh, bone.index);
			bone.screen = fvector2d{  };

			if (bone.index == bone::HumanHead)
				bone_pos.z += 15;

			if (bone.index == bones::HumanLCalf ||
				bone.index == bones::HumanRCalf) {
				bone_pos.z -= 8.5;
			}

			if (bone.index == bones::HumanRPalm ||
				bone.index == bones::HumanRForearm23) {
				bone_pos.x -= 10.5;
			}

			if (bone.index == bones::HumanLPalm ||
				bone.index == bones::HumanLForearm23) {
				bone_pos.x += 11;
			}

			if (game_helper.WorldToScreen(bone_pos, &bone.screen)) {
				if (bone.screen.x < Bounds.left)
					Bounds.left = bone.screen.x;
				else if (bone.screen.x > Bounds.right)
					Bounds.right = bone.screen.x;
				if (bone.screen.y < Bounds.top)
					Bounds.top = bone.screen.y;
				else if (bone.screen.y > Bounds.bottom)
					Bounds.bottom = bone.screen.y;

				bone.on_screen = true;
			}
		}

		if (Bounds.left == FLT_MAX)
			return false;
		if (Bounds.right == FLT_MIN)
			return false;
		if (Bounds.top == FLT_MAX)
			return false;
		if (Bounds.bottom == FLT_MIN)
			return false;

		Bounds.left -= expand;
		Bounds.right += expand;
		Bounds.top -= expand;
		Bounds.bottom += expand;

		out = Bounds;

		return true;
		};

	PlayerBounds Bounds;

	if (get_bounds(Bounds, 4.f)) {
		*Boundings = Bounds;
		return true;
	}

	return false;

}
void normal_box(const ImVec2& position, const float width, const float height, const uint32_t color, float thickness) {
	ImGui::GetBackgroundDrawList()->AddRect(position, ImVec2(position.x + width, position.y + height), color, 0, 0, thickness);
}
void draw_cornered_box(int x, int y, int w, int h, ImColor color, float thickness) {
	ImGui::GetBackgroundDrawList()->AddLine(ImVec2(x, y), ImVec2(x, y + (h / 3)), color, thickness);
	ImGui::GetBackgroundDrawList()->AddLine(ImVec2(x, y), ImVec2(x + (w / 3), y), color, thickness);
	ImGui::GetBackgroundDrawList()->AddLine(ImVec2(x + w - (w / 3), y), ImVec2(x + w, y), color, thickness);
	ImGui::GetBackgroundDrawList()->AddLine(ImVec2(x + w, y), ImVec2(x + w, y + (h / 3)), color, thickness);
	ImGui::GetBackgroundDrawList()->AddLine(ImVec2(x, y + h - (h / 3)), ImVec2(x, y + h), color, thickness);
	ImGui::GetBackgroundDrawList()->AddLine(ImVec2(x, y + h), ImVec2(x + (w / 3), y + h), color, thickness);
	ImGui::GetBackgroundDrawList()->AddLine(ImVec2(x + w - (w / 3), y + h), ImVec2(x + w, y + h), color, thickness);
	ImGui::GetBackgroundDrawList()->AddLine(ImVec2(x + w, y + h - (h / 3)), ImVec2(x + w, y + h), color, thickness);
}
void filled_box_image(const ImVec2& position, float width, float height, ID3D11ShaderResourceView* texture)
{
	if (!texture) return;

	ImDrawList* draw = ImGui::GetBackgroundDrawList();

	draw->AddImage(
		texture,
		position,
		ImVec2(position.x + width, position.y + height),
		ImVec2(0, 1),
		ImVec2(1, 0) 
	);
}


void filled_box(const ImVec2& position, const float width, const float height, const uint32_t color) {
	ImGui::GetBackgroundDrawList()->AddRectFilled(position, ImVec2(position.x + width, position.y + height), color, 0, 0);
}
void outlined_box(const ImVec2& position, const float width, const float height, const uint32_t color, float thickness, float outline_thickness) {
	const ImVec2 top_left = position;
	const ImVec2 bottom_right = ImVec2(position.x + width, position.y + height);

	// Outline
	ImGui::GetBackgroundDrawList()->AddRect(top_left, bottom_right, ImColor(0, 0, 0), 0, 0, thickness + outline_thickness);

	// Inline
	ImGui::GetBackgroundDrawList()->AddRect(top_left, bottom_right, color, 0, 0, thickness);
}
static auto DrawHealthbars(float x, float y, float height, int thickness, int health, bool HealBool) -> void
{
	if (HealBool)
	{
		DrawFilledRect(x - (thickness + 5), y, thickness, height, ImColor(0, 255, 255));//health %left
		DrawFilledRect(x - (thickness + 5), y, thickness, height - ((height / 100) * health), ImColor(0, 0, 0)); //health bar
	}
}

enum class EFortRarity : uint8_t
{
	EFortRarity__Common = 0,
	EFortRarity__Uncommon = 1,
	EFortRarity__Rare = 2,
	EFortRarity__Epic = 3,
	EFortRarity__Legendary = 4,
	EFortRarity__Mythic = 5,
	EFortRarity__Transcendent = 6,
	EFortRarity__Unattainable = 7,
	EFortRarity__NumRarityValues = 8,
};

typedef struct items
{
	uintptr_t actor;
	std::string name;
	bool is_pickup;
	float distance;
} items;
std::vector<items> inline items_list;
std::vector<items> items_temp_list;

bool is_in_screen(fvector2d screen_location)
{
	if (screen_location.x > 0 && screen_location.x < globals.ScreenWidth && screen_location.y > 0 && screen_location.y < globals.ScreenHeight)
	{
		return true;
	}
	else
	{
		return false;
	}
}
ImColor get_loot_color(EFortRarity tier)
{
	if (tier == EFortRarity::EFortRarity__Common)
	{
		return ImColor(123, 123, 123, 255);
	}
	else if (tier == EFortRarity::EFortRarity__Uncommon)
	{
		return ImColor(58, 121, 19, 255);
	}
	else if (tier == EFortRarity::EFortRarity__Rare)
	{
		return ImColor(18, 88, 162, 255);
	}
	else if (tier == EFortRarity::EFortRarity__Epic)
	{
		return ImColor(189, 63, 250, 255);
	}
	else if (tier == EFortRarity::EFortRarity__Legendary)
	{
		return ImColor(255, 118, 5, 255);
	}
	else if (tier == EFortRarity::EFortRarity__Mythic)
	{
		return ImColor(220, 160, 30, 255);
	}
	else if (tier == EFortRarity::EFortRarity__Transcendent)
	{
		return ImColor(0, 225, 252, 255);
	}

	return ImColor(123, 123, 123, 255);
}

class FName
{
public:
#define read Kernel->read
	int32_t ComparisonIndex;

	std::string ToString()
	{
		return ToString(ComparisonIndex);
	}

	static std::string ToString(int32_t index)
	{

		int32_t DecryptedIndex = DecryptIndex(index);
		uint64_t NamePoolChunk = read<uint64_t>(Kernel->base + 0x16B71B40 + 8 * (HIWORD(DecryptedIndex) + 2)) + 2 * (uint16_t)DecryptedIndex;
		uint16_t Pool = read<uint16_t>(NamePoolChunk);

		if (((Pool ^ 0x820) & 0x7FE0) <= 0)
		{
			DecryptedIndex = DecryptIndex(read<int32_t>(NamePoolChunk + 2));
			NamePoolChunk = read<uint64_t>(Kernel->base + 0x16B71B40 + 8 * (HIWORD(DecryptedIndex) + 2)) + 2 * (uint16_t)DecryptedIndex;
			Pool = read<uint16_t>(NamePoolChunk);
		}

		int32_t Length = (((Pool ^ 0x820u) >> 5) & 0x3FF) * (((Pool & 0x8000u) != 0) ? 2 : 1);

		std::string NameBuffer(Length, '\0');
		Kernel->ReadMemory(NamePoolChunk + 2, &NameBuffer[0], Length);
		DecryptFName(&NameBuffer[0], Length);
		return NameBuffer;
	}

	static int32_t DecryptIndex(int32_t index)
	{
		if (index)
		{
			int32_t DecryptedIndex = -(0x1EC7C94 ^ (index - 1));
			return DecryptedIndex ? DecryptedIndex : 0x1EC7C95;
		}
		return 0;
	}

	static void DecryptFName(char* buffer, int length)
	{
		if (length)
		{
			int naan = 0x132ECB1 + 8077 * length;

			for (int indra = 0; indra < length; ++indra)
			{
				buffer[indra] = naan + 30 - buffer[indra];
				naan = 0x132ECB1 + 8077 * naan;
			}
		}

		buffer[length] = '\0';
	}
};
int bD1st4nce, bH1tb0x = 0, bE5pD1st4nce = 280, bA1mD1st4nce = 280, bB0xS1ze = 2.0f, bAut0L0ckD1st = 1, bLootRendering = 40;
void DrawString10(float fontSize, int x, int y, ImColor color, bool bCenter, bool stroke, const char* pText, ...)
{
	va_list va_alist;
	char buf[1024] = { 0 };
	va_start(va_alist, pText);
	_vsnprintf_s(buf, sizeof(buf), pText, va_alist);
	va_end(va_alist);
	std::string text = WStringToUTF8(MBytesToWString(buf).c_str());
	if (bCenter)
	{
		ImVec2 textSize = ImGui::CalcTextSize(text.c_str());
		x = x - textSize.x / 4;
		y = y - textSize.y;
	}
	
	if (stroke)
	{
		ImGui::GetBackgroundDrawList()->AddText(ImGui::GetFont(), fontSize, ImVec2(x + 1, y + 1), color, text.c_str());
		ImGui::GetBackgroundDrawList()->AddText(ImGui::GetFont(), fontSize, ImVec2(x - 1, y - 1), color, text.c_str());
		ImGui::GetBackgroundDrawList()->AddText(ImGui::GetFont(), fontSize, ImVec2(x + 1, y - 1), color, text.c_str());
		ImGui::GetBackgroundDrawList()->AddText(ImGui::GetFont(), fontSize, ImVec2(x - 1, y + 1), color, text.c_str());
	}
	ImGui::GetBackgroundDrawList()->AddText(ImGui::GetFont(), fontSize, ImVec2(x, y), color, text.c_str());
}
typedef struct _LootEntity {
	std::string GNames;
	uintptr_t ACurrentItem;
}LootEntity;
static std::vector<LootEntity> LootentityList;
static void CacheLevels()
{
	while (true)
	{
		if (WorldCache.AcknownledgedPawn)
		{
			std::vector<LootEntity> tmpList;
			uintptr_t ItemLevels = read<uintptr_t>(WorldCache.UWorld + 0x1B8);

			for (int i = 0; i < read<DWORD>(WorldCache.UWorld + (0x1B8 + sizeof(PVOID))); ++i) {

				uintptr_t ItemLevel = read<uintptr_t>(ItemLevels + (i * sizeof(uintptr_t)));

				for (int i = 0; i < read<DWORD>(ItemLevel + (0x208 + sizeof(PVOID))); ++i) {
					uintptr_t ItemsPawns = read<uintptr_t>(ItemLevel + 0x208);


					uintptr_t CurrentItemPawn = read<uintptr_t>(ItemsPawns + (i * sizeof(uintptr_t)));


					uintptr_t ItemRootComponent = read<uintptr_t>(CurrentItemPawn + 0x1B0);
					fvector ItemPosition = read<fvector>(ItemRootComponent + 0x138);
					float ItemDist = camera_postion.location.distance(ItemPosition) / 100.f;

					if (ItemDist < bLootRendering) {

						auto test = read<FName>(CurrentItemPawn + 0x18);

						auto CurrentItemPawnName = test.ToString();
						if ((true && strstr(CurrentItemPawnName.c_str(), "FortPickupAthena")) ||
							strstr(CurrentItemPawnName.c_str(), "Tiered_Chest") ||
							(true && (strstr(CurrentItemPawnName.c_str(), "Vehicl") ||
								strstr(CurrentItemPawnName.c_str(), "Valet_Taxi") ||
								strstr(CurrentItemPawnName.c_str(), "Tiered_Ammo"))) ||
							strstr(CurrentItemPawnName.c_str(), "Creative_SupplyDrop") ||
							strstr(CurrentItemPawnName.c_str(), "Creative_Device"))
						{
							LootEntity LevelObjects{ };
							LevelObjects.ACurrentItem = CurrentItemPawn;
							LevelObjects.GNames = CurrentItemPawnName;
							tmpList.push_back(LevelObjects);
						}
					}
				}
			}
			LootentityList.clear();
			LootentityList = tmpList;
			std::this_thread::sleep_for(std::chrono::milliseconds(300));
		}

	}
}typedef struct
{
	DWORD R;
	DWORD G;
	DWORD B;
	DWORD A;
} RGBA;
class Color
{
public:
	RGBA red = { 255,0,0,255 };
	RGBA Magenta = { 255,0,255,255 };
	RGBA yellow = { 255,255,0,255 };
	RGBA grayblue = { 128,128,255,255 };
	RGBA green = { 128,224,0,255 };
	RGBA darkgreen = { 0,224,128,255 };
	RGBA brown = { 192,96,0,255 };
	RGBA pink = { 255,168,255,255 };
	RGBA DarkYellow = { 216,216,0,255 };
	RGBA SilverWhite = { 236,236,236,255 };
	RGBA purple = { 144,0,255,255 };
	RGBA Navy = { 88,48,224,255 };
	RGBA skyblue = { 0,136,255,255 };
	RGBA graygreen = { 128,160,128,255 };
	RGBA blue = { 0,96,192,255 };
	RGBA orange = { 255,128,0,255 };
	RGBA peachred = { 255,80,128,255 };
	RGBA reds = { 255,128,192,255 };
	RGBA darkgray = { 96,96,96,255 };
	RGBA Navys = { 0,0,128,255 };
	RGBA darkgreens = { 0,128,0,255 };
	RGBA darkblue = { 0,128,128,255 };
	RGBA redbrown = { 128,0,0,255 };
	RGBA purplered = { 128,0,128,255 };
	RGBA greens = { 0,255,0,255 };
	RGBA envy = { 0,255,255,255 };
	RGBA black = { 0,0,0,255 };
	RGBA gray = { 128,128,128,255 };
	RGBA white = { 255,255,255,255 };
	RGBA blues = { 30,144,255,255 };
	RGBA lightblue = { 135,206,250,160 };
	RGBA Scarlet = { 220, 20, 60, 160 };
	RGBA white_ = { 255,255,255,200 };
	RGBA gray_ = { 128,128,128,200 };
	RGBA black_ = { 0,0,0,200 };
	RGBA red_ = { 255,0,0,200 };
	RGBA Magenta_ = { 255,0,255,200 };
	RGBA yellow_ = { 255,255,0,200 };
	RGBA grayblue_ = { 128,128,255,200 };
	RGBA green_ = { 128,224,0,200 };
	RGBA darkgreen_ = { 0,224,128,200 };
	RGBA brown_ = { 192,96,0,200 };
	RGBA pink_ = { 255,168,255,200 };
	RGBA darkyellow_ = { 216,216,0,200 };
	RGBA silverwhite_ = { 236,236,236,200 };
	RGBA purple_ = { 144,0,255,200 };
	RGBA Blue_ = { 88,48,224,200 };
	RGBA skyblue_ = { 0,136,255,200 };
	RGBA graygreen_ = { 128,160,128,200 };
	RGBA blue_ = { 0,96,192,200 };
	RGBA orange_ = { 255,128,0,200 };
	RGBA pinks_ = { 255,80,128,200 };
	RGBA Fuhong_ = { 255,128,192,200 };
	RGBA darkgray_ = { 96,96,96,200 };
	RGBA Navy_ = { 0,0,128,200 };
	RGBA darkgreens_ = { 0,128,0,200 };
	RGBA darkblue_ = { 0,128,128,200 };
	RGBA redbrown_ = { 128,0,0,200 };
	RGBA purplered_ = { 128,0,128,200 };
	RGBA greens_ = { 0,255,0,200 };
	RGBA envy_ = { 0,255,255,200 };

	RGBA glassblack = { 0, 0, 0, 160 };
	RGBA GlassBlue = { 65,105,225,80 };
	RGBA glassyellow = { 255,255,0,160 };
	RGBA glass = { 200,200,200,60 };

	RGBA filled = { 0, 0, 0, 150 };

	RGBA Plum = { 221,160,221,160 };

};
Color Col;

void actorloop()
{
	
	temporary_entity_list.clear();
	std::lock_guard<std::mutex> lock(base2);

	uintptr_t encryptedUWorld = read<uintptr_t>(Kernel->base + offsets::UWorld);
	uintptr_t decryptedUWorld = encryptedUWorld ^ offsets::UWorldDecryption;

	WorldCache.UWorld = decryptedUWorld;
	WorldCache.GameInstance = read<__int64>(WorldCache.UWorld + offsets::OwningGameInstance);
	WorldCache.LocalPlayer = read<__int64>(read<__int64>(WorldCache.GameInstance + offsets::LocalPlayers));
	WorldCache.PlayerController = read<__int64>(WorldCache.LocalPlayer + offsets::PlayerController);
	WorldCache.AcknownledgedPawn = read<__int64>(WorldCache.PlayerController + offsets::AcknowledgedPawn);
	WorldCache.Mesh = read<__int64>(WorldCache.AcknownledgedPawn + offsets::Mesh);
	WorldCache.PlayerState = read<__int64>(WorldCache.AcknownledgedPawn + offsets::PlayerState);
	WorldCache.RootComponent = read<__int64>(WorldCache.AcknownledgedPawn + offsets::RootComponent);
	WorldCache.GameState = read<__int64>(WorldCache.UWorld + offsets::GameState);
	WorldCache.PlayerArray = read<__int64>(WorldCache.GameState + offsets::PlayerArray);
	WorldCache.PlayerArraySize = read<int>(WorldCache.GameState + (offsets::PlayerArray + sizeof(uintptr_t)));
	WorldCache.LocalWeapon = read<__int64>(WorldCache.AcknownledgedPawn + offsets::current_weapon);
	WorldCache.LocalVehicle = read<__int64>(WorldCache.AcknownledgedPawn + offsets::current_vehicle);
	WorldCache.TeamIndex = read<char>(WorldCache.PlayerState + offsets::TeamIndex);

	if (WorldCache.AcknownledgedPawn != 0)
	{
		WorldCache.RootComponent = read<uintptr_t>(WorldCache.AcknownledgedPawn + offsets::RootComponent);
		WorldCache.PlayerState = read<uintptr_t>(WorldCache.AcknownledgedPawn + offsets::PlayerState);
		WorldCache.TeamIndex = read<int>(WorldCache.PlayerState + offsets::TeamIndex);
	}

	TargetDistance = FLT_MAX;
	TargetEntity = NULL;
	DesyncTargetEntity = NULL;

	render_elements();
	uintptr_t game_state = read<uintptr_t>(WorldCache.UWorld + offsets::GameState);
	uintptr_t player_array = read<uintptr_t>(game_state + offsets::PlayerArray);
	int player_count = read<int>(game_state + (offsets::PlayerArray + sizeof(uintptr_t)));
	for (int i = 0; i < player_count; i++)
	{
		uintptr_t PlayerState = read<uintptr_t>(WorldCache.PlayerArray + (i * sizeof(uintptr_t)));
		uintptr_t CurrentActor = read<uintptr_t>(PlayerState + offsets::PawnPrivate);
		uintptr_t pawn_private = read<uintptr_t>(PlayerState + offsets::PawnPrivate);
		if (pawn_private == WorldCache.AcknownledgedPawn) continue;

		if (CurrentActor == WorldCache.AcknownledgedPawn) continue;

		uintptr_t skeletalmesh = read<uintptr_t>(CurrentActor + offsets::Mesh);
		if (!skeletalmesh) continue;

		if (aimbot.ignore_team)
		{
			char team_id = read<char>(PlayerState + offsets::TeamIndex);
			if (WorldCache.AcknownledgedPawn)
			{
				if (team_id == WorldCache.TeamIndex) continue;
			}
		}

		fvector base_bone = game_helper.getsocketlocation(skeletalmesh, 0);
		if (base_bone.x == 0 && base_bone.y == 0 && base_bone.z == 0) continue;

		if (is_dead(pawn_private)) continue;

		uintptr_t PlayerArray = read<uintptr_t>(WorldCache.PlayerArray + i * 0x8);
		uintptr_t SkeletalMesh = read<uintptr_t>(CurrentActor + offsets::Mesh);

		fvector root_bone = game_helper.getsocketlocation(SkeletalMesh, 0);
		fvector2d root_box1 = game_helper.ProjectWorldToScreen(fvector(root_bone.x, root_bone.y, root_bone.z - 15));

		fvector head_bone = game_helper.getsocketlocation(SkeletalMesh, 110);
		fvector2d head_box = game_helper.ProjectWorldToScreen(fvector(head_bone.x, head_bone.y, head_bone.z + 15));
		fvector head3d = game_helper.getsocketlocation(SkeletalMesh, 110);
		fvector2d head = game_helper.ProjectWorldToScreen(head_bone);
		fvector2d root = game_helper.ProjectWorldToScreen(root_bone);

		float box_height = abs(head.y - root_box1.y);
		float box_width = box_height * 0.30f;
		float distance = camera_postion.location.distance(root_bone) / 100;

		float CornerHeight = abs(head_box.y - root.y);
		float CornerWidth = CornerHeight * visuals.corner_width;

		int bottom_offset = 0;
		int top_offset = 0;



		if (distance > visuals.renderDistance && WorldCache.AcknownledgedPawn) continue;

		int visible_meshes = 0;
		int not_visible_meshes = 0;
		static ImColor box_filled;

		if (game_helper.IsEnemyVisible(SkeletalMesh))
		{
			box_color = ImGui::GetColorU32({ vscolors.box_visible[0], vscolors.box_visible[1], vscolors.box_visible[2],  1.0f });
			box_filled = ImGui::GetColorU32({ vscolors.box_filled_visible[0], vscolors.box_filled_visible[1], vscolors.box_filled_visible[2], vscolors.box_filled_visible[3] });
			visible_meshes++;
		}
		else
		{
			box_color = ImGui::GetColorU32({ vscolors.box_invisible[0], vscolors.box_invisible[1], vscolors.box_invisible[2],  1.0f });
			box_filled = ImGui::GetColorU32({ vscolors.box_filled_invisible[0], vscolors.box_filled_invisible[1], vscolors.box_filled_invisible[2],  vscolors.box_filled_invisible[3] });
			not_visible_meshes++;
		}
		ImColor textcol = {};
		textcol = ImGui::GetColorU32({ vscolors.text_color[0], vscolors.text_color[1], vscolors.text_color[2],  1.0f });
		if (aimbot.enable)
		{
			double dx = head.x - (globals.ScreenWidth / 2);
			double dy = head.y - (globals.ScreenHeight / 2);
			float dist = sqrtf(dx * dx + dy * dy);

			if (aimbot.vischeck)
			{
				if (game_helper.IsEnemyVisible(SkeletalMesh))
				{
					if (dist < aimbot.fovsize && dist < TargetDistance)
					{
						TargetDistance = dist;
						TargetEntity = CurrentActor;
					}
				}
			}
			else
			{
				if (dist < aimbot.fovsize && dist < TargetDistance)
				{
					TargetDistance = dist;
					TargetEntity = CurrentActor;
				}
			}
		}
		PlayerBounds o{ };
		if (GetPlayerBoundings(SkeletalMesh, &o))
		{
		if (visuals.box && visuals.enable)
		{
			
			if (config.target_bone == 0)
			{
				const float halfWidth = box_width / 2.0f;
				const ImVec2 topLeft(head_box.x - halfWidth, head_box.y);
				const ImVec2 topRight(root.x + halfWidth, head_box.y);
				const ImVec2 bottomLeft(head_box.x - halfWidth, root.y);
				const ImVec2 bottomRight(root.x + halfWidth, root.y);

				const int boxThickness = visuals.box_thick;

				const ImColor outlineColor(0, 0, 0, 255);
				const int outlineThickness = visuals.box_thick;

				if (visuals.filled)
				{
					int style = config.filledStyles;

					if (!userinformations::IsBooster && style != 0)
						style = 0;

					if (style == 0)
					{
						filled_box(ImVec2(o.left, o.bottom), o.right - o.left, o.top - o.bottom, box_filled);
					}
					else if (style == 1)
					{
						filled_box_image(ImVec2(o.left, o.bottom), o.right - o.left, o.top - o.bottom, ms::ImgFilled);
					}
					

				}

				if (visuals.box_outline)
				{
					outlined_box(ImVec2(o.left, o.bottom), o.right - o.left, o.top - o.bottom, box_color, outlineThickness, outlineThickness + 1.5);
				}
				float LeftX = head3d.x - (box_width / 1);
				float LeftY = bottomRight.y;;
				normal_box(ImVec2(o.left, o.bottom), o.right - o.left, o.top - o.bottom, box_color, outlineThickness);
			}
			else if (config.target_bone == 1)
			{
				const float width = o.right - o.left;
				const float height = o.top - o.bottom;
				const int x = static_cast<int>(o.left);
				const int y = static_cast<int>(o.bottom);

				const float boxThickness = visuals.box_thick;
				const float outlineThickness = 1.5f;

				if (visuals.filled)
				{
					if (config.filledStyles == 0)
					{
						filled_box(ImVec2(o.left, o.bottom), o.right - o.left, o.top - o.bottom, box_filled);
					}
					else
					{
						filled_box_image(ImVec2(x, y), width, height, ms::ImgFilled);

					}
				}

				if (visuals.box_outline)
				{
					draw_cornered_box(x, y, static_cast<int>(width), static_cast<int>(height), ImColor(0, 0, 0, 255), boxThickness + outlineThickness);
				}
				draw_cornered_box(x, y, static_cast<int>(width), static_cast<int>(height), box_color, boxThickness);
			}

		}

		
		


		float yOffsetBottom = 10.0f;
		
		if (visuals.distance && visuals.enable)
		{

			fvector bottom3d = game_helper.getsocketlocation(SkeletalMesh, 0);
			fvector2d bottom2d = game_helper.ProjectWorldToScreen(bottom3d);
			char dist[64];
			sprintf_s(dist, "%.fm", distance);

			ImVec2 text_size = ImGui::CalcTextSize(dist);
			ImVec2 pos = ImVec2(bottom2d.x - text_size.x / 2, bottom2d.y + yOffsetBottom - text_size.y / 2);
			ImVec2 background_size = ImVec2(text_size.x + 10.0f, text_size.y);
			ImVec2 background_pos = ImVec2(pos.x - 5.0f, pos.y);

			ImGui::PushFont(menus.MenuFont);
			text(to_string(static_cast<int>(distance)) + _("m"), ImVec2(o.left + (o.right - o.left) / 2.0f, o.bottom + 7.0f), (float)(visuals.textsize), textcol, true);
			ImGui::PopFont();
			yOffsetBottom += 15.0f;
		}

			float yOffsetTop = 10.0f;

			if (visuals.username && visuals.platform && visuals.enable)
			{


				std::string username_str = decryption.GetPlayerName(PlayerState);
				std::string platform_str = platform(PlayerState);



				std::string combinedText = username_str + " | " + platform_str + "";
				int offset;
				if (visuals.username && visuals.platform)
					offset = 20;
				else if (visuals.weapon)
					offset = 20;
				else
					offset = 20;
				if (visuals.username && visuals.platform && visuals.weapon)
					offset = 20;

				//ImVec2 textPosition(bottom.x, bottom.y + offset);
				ImVec2 textPosition(o.left + (o.right - o.left) / 2.0f, o.top - offset);
				if (aimbot.vischeck)
				{
					text(combinedText.c_str(), textPosition, (float)visuals.textsize, textcol, true);
				}
				else {
					//draw_outlined_text(textPosition, normal, combinedText.c_str());
					text(combinedText.c_str(), textPosition, (float)visuals.textsize, textcol, true);

				}
			}
			else if (visuals.username && !visuals.platform && visuals.enable)
			{
				std::string playerName = decryption.GetPlayerName(PlayerState);
				text(playerName.c_str(), ImVec2(o.left + (o.right - o.left) / 2.0f, o.top - 20.0f), (float)visuals.textsize, textcol, true);

			}
			else if (visuals.platform && !visuals.username && visuals.enable)
			{
				std::string platform_str = platform(PlayerState);
				ImVec2 text_size = ImGui::CalcTextSize(platform_str.c_str());
				int offset;
				if (visuals.username)
					offset = 20;
				else if (visuals.weapon)
					offset = 20;
				else
					offset = 20;
				if (visuals.username && visuals.weapon)
					offset = 20;
				if (aimbot.vischeck)
				{
					text(platform_str.c_str(), ImVec2(o.left + (o.right - o.left) / 2.0f, o.top - offset), (float)visuals.textsize, textcol, true);
				}
				else {
					text(platform_str.c_str(), ImVec2(o.left + (o.right - o.left) / 2.0f, o.top - offset), (float)visuals.textsize, textcol, true);
				}
			}
		
			if (visuals.weapon && visuals.enable)
			{
				/*ImVec2 text_position(o.left + (o.right - o.left) / 2.0f, o.bottom + 21.0f);
				uint8_t Tier = {};
				Tier = memory.Read<uint8_t>(Tier + 0xa2);
				int32_t AmmoCount = {};
				AmmoCount =	memory.Read<int32_t>(AmmoCount + 0x10bc);
				std::string weaponname = decryption.GetWeaponName(pawn_private);
				if (Tier == 0) {
					string weapon_text = weaponname;
					if (strstr(weaponname.c_str(), _("Pickaxe")) != nullptr) {
						weapon_text = weaponname;
					}
					else {
						weapon_text = _("No Item");
					}
					text(weapon_text, text_position, 13.0f, textcol, true);
				}
				else {
					if (visuals.ammo)
					{
						ImVec2 text_position(o.left + (o.right - o.left) / 2.0f + 20.0f, o.bottom + 21.0f);
						text(weaponname + ("[ ") + to_string(AmmoCount) + (" ]"), text_position, 13.0f, textcol, true);
					}
					else
					{
						text(weaponname, text_position, 13.0f, textcol, true);
					}

				}*/
				fvector bottom3d = game_helper.getsocketlocation(SkeletalMesh, 0);
				fvector2d bottom2d = game_helper.ProjectWorldToScreen(bottom3d);
				std::string weapon;
				std::string weaponname = decryption.GetWeaponName(pawn_private);
				if (weaponname.empty())
				{
					weapon = "No Item";
				}
				else
				{
					weapon = weaponname;
				}

				ImVec2 text_size = ImGui::CalcTextSize(weapon.c_str());
				ImVec2 pos(o.left + (o.right - o.left) / 2.0f, o.bottom + 21.0f);
				ImVec2 background_size = ImVec2(text_size.x + 10.0f, text_size.y);
				ImVec2 background_pos = ImVec2(pos.x - 5.0f, pos.y);

				ImGui::PushFont(menus.MenuFont);
				
				text( weapon.c_str() , ImVec2(o.left + (o.right - o.left) / 2.0f, o.bottom + 21.0f), (float)(visuals.textsize), textcol, true);
				ImGui::PopFont();

				yOffsetBottom += 15.0f;

			}
			
			o.pawn_priv = pawn_private;
			if (visuals.radar && visuals.enable)
			{
				add_players_radar(root_bone);
			}

			if (visuals.rank && visuals.enable)
			{
				fvector bottom3d = game_helper.getsocketlocation(SkeletalMesh, 0);
				fvector2d bottom2d = game_helper.ProjectWorldToScreen(bottom3d);
				uintptr_t habenaro = read<uintptr_t>(PlayerState + offsets::habanero_component);
				int32_t ranked_progress = read<int32_t>(habenaro + 0xd0 + 0x10);
				std::string rank_name = get_rank_name(ranked_progress);
				ImVec4 rank_color = get_rank_color(ranked_progress);

				ImVec2 text_size = ImGui::CalcTextSize(rank_name.c_str());
				ImVec2 pos = ImVec2(bottom2d.x - text_size.x / 2, bottom2d.y + yOffsetBottom - text_size.y / 2);
				ImVec2 background_size = ImVec2(text_size.x + 10.0f, text_size.y);
				ImVec2 background_pos = ImVec2(pos.x - 5.0f, pos.y);

				ImGui::PushFont(menus.MenuFont);
				text(rank_name.c_str(), ImVec2(o.left + (o.right - o.left) / 2.0f, o.bottom + 34.0f), (float)(visuals.textsize), rank_color, true);
				ImGui::PopFont();
				yOffsetBottom += 15.0f;
			}

			if (aimbot.weaponcfg)
			{
				weaponconfig(decryption.GetWeaponName((uintptr_t)WorldCache.AcknownledgedPawn));
			}
			int side_offset = 0;


			fvector2d boxHead = game_helper.ProjectWorldToScreen(fvector(head3d.x, head3d.y, head3d.z + 20.0f));
			if (visuals.platform1 && visuals.enable)
			{
				uintptr_t platform_ptr = read<uintptr_t>(PlayerState + offsets::Platform);
				wchar_t platform_char[64] = { 0 };

				Kernel->ReadMemory(platform_ptr, reinterpret_cast<uint8_t*>(platform_char), sizeof(platform_char));

				std::wstring platform_wide_str(platform_char);
				std::string platform_str(platform_wide_str.begin(), platform_wide_str.end());

				ImVec2 text_size = ImGui::CalcTextSize(platform_str.c_str());
				ImVec2 pos = ImVec2(boxHead.x - text_size.x / 2, boxHead.y - 10.0f - text_size.y / 2);
				ImVec2 background_size = ImVec2(text_size.x + 10.0f, text_size.y);
				ImVec2 background_pos = ImVec2(pos.x - 5.0f, pos.y);

				float imageWidth = 20.0f;
				float imageX = boxHead.x - (imageWidth / 2);
				float imageY = boxHead.y - (yOffsetTop + 5) - (imageWidth / 2);

				if (platform_str == xorstr_("WIN"))
				{
					ImGui::GetBackgroundDrawList()->AddImage(ranks::windows, ImVec2(imageX, imageY), ImVec2(imageX + imageWidth, imageY + imageWidth));
				}
				else if (platform_str == xorstr_("PSN") || platform_str == xorstr_("PS5"))
				{
					ImGui::GetBackgroundDrawList()->AddImage(ranks::playstation, ImVec2(imageX, imageY), ImVec2(imageX + imageWidth, imageY + imageWidth));
				}
				else if (platform_str == xorstr_("XBL") || platform_str == xorstr_("XSX") || platform_str == xorstr_("XSS"))
				{
					ImGui::GetBackgroundDrawList()->AddImage(ranks::xbox, ImVec2(imageX, imageY), ImVec2(imageX + imageWidth, imageY + imageWidth));
				}
				else if (platform_str == xorstr_("SWT"))
				{
					ImGui::GetBackgroundDrawList()->AddImage(ranks::switchs, ImVec2(imageX, imageY), ImVec2(imageX + imageWidth, imageY + imageWidth));
				}
				else if (platform_str == xorstr_("AND") || platform_str == xorstr_("IOS"))
				{
					ImGui::GetBackgroundDrawList()->AddImage(ranks::ios, ImVec2(imageX, imageY), ImVec2(imageX + imageWidth, imageY + imageWidth));
				}
			}

			if (visuals.killscore && visuals.enable)
			{
				
			}

			if (visuals.level_score && visuals.enable)
			{
				
			}

		}

		if (visuals.skeleton && visuals.enable)
		{
			int selected = config.selected_config;

			// If user is not a booster, override to 0
			if (!userinformations::IsBooster && selected != 0)
				selected = 0;

			if (selected == 0)
			{
				// === non-wavy bones ===
				#define mesh SkeletalMesh
				std::vector<fvector> bones = {
					game_helper.getsocketlocation(mesh, 67),  // neck
					game_helper.getsocketlocation(mesh, 9),   // left shoulder
					game_helper.getsocketlocation(mesh, 10),  // left elbow
					game_helper.getsocketlocation(mesh, 11),  // left hand
					game_helper.getsocketlocation(mesh, 38),  // right shoulder
					game_helper.getsocketlocation(mesh, 39),  // right elbow
					game_helper.getsocketlocation(mesh, 40),  // right hand
					game_helper.getsocketlocation(mesh, 2),   // pelvis
					game_helper.getsocketlocation(mesh, 71),  // left hip
					game_helper.getsocketlocation(mesh, 72),  // left knee
					game_helper.getsocketlocation(mesh, 78),  // right hip
					game_helper.getsocketlocation(mesh, 79),  // right knee
					game_helper.getsocketlocation(mesh, 75),  // left foot
					game_helper.getsocketlocation(mesh, 82),  // right foot
					game_helper.getsocketlocation(mesh, 11)   // head
				};

				std::vector<fvector2d> screenPositions(bones.size());
				for (size_t i = 0; i < bones.size(); ++i) {
					screenPositions[i] = game_helper.ProjectWorldToScreen(bones[i]);
				}

				if (visuals.skel_outline)
				{
					draw_line(screenPositions[1], screenPositions[4], ImColor(0, 0, 0), 2);   // left shoulder to right shoulder
					draw_line(screenPositions[1], screenPositions[2], ImColor(0, 0, 0), 2);   // left shoulder to left elbow
					draw_line(screenPositions[2], screenPositions[3], ImColor(0, 0, 0), 2);   // left elbow to left hand
					draw_line(screenPositions[4], screenPositions[5], ImColor(0, 0, 0), 2);   // right shoulder to right elbow
					draw_line(screenPositions[5], screenPositions[6], ImColor(0, 0, 0), 2);   // right elbow to right hand
					draw_line(screenPositions[0], screenPositions[7], ImColor(0, 0, 0), 2);   // neck to pelvis
					draw_line(screenPositions[7], screenPositions[8], ImColor(0, 0, 0), 2);   // pelvis to left hip
					draw_line(screenPositions[8], screenPositions[9], ImColor(0, 0, 0), 2);   // left hip to left knee
					draw_line(screenPositions[9], screenPositions[12], ImColor(0, 0, 0), 2);  // left knee to left foot
					draw_line(screenPositions[7], screenPositions[10], ImColor(0, 0, 0), 2);  // pelvis to right hip
					draw_line(screenPositions[10], screenPositions[11], ImColor(0, 0, 0), 2); // right hip to right knee
					draw_line(screenPositions[11], screenPositions[13], ImColor(0, 0, 0), 2); // right knee to right foot
				}

				draw_line(screenPositions[1], screenPositions[4], box_color, visuals.skeleton_thickness);
				draw_line(screenPositions[1], screenPositions[2], box_color, visuals.skeleton_thickness);
				draw_line(screenPositions[2], screenPositions[3], box_color, visuals.skeleton_thickness);
				draw_line(screenPositions[4], screenPositions[5], box_color, visuals.skeleton_thickness);
				draw_line(screenPositions[5], screenPositions[6], box_color, visuals.skeleton_thickness);
				draw_line(screenPositions[0], screenPositions[7], box_color, visuals.skeleton_thickness);
				draw_line(screenPositions[7], screenPositions[8], box_color, visuals.skeleton_thickness);
				draw_line(screenPositions[8], screenPositions[9], box_color, visuals.skeleton_thickness);
				draw_line(screenPositions[9], screenPositions[12], box_color, visuals.skeleton_thickness);
				draw_line(screenPositions[7], screenPositions[10], box_color, visuals.skeleton_thickness);
				draw_line(screenPositions[10], screenPositions[11], box_color, visuals.skeleton_thickness);
				draw_line(screenPositions[11], screenPositions[13], box_color, visuals.skeleton_thickness);
			}
			else if (selected == 1)
			{
				// === wavy bones ===
				#define mesh SkeletalMesh
				std::vector<fvector> bones = {
					game_helper.getsocketlocation(mesh, 67),  // neck
					game_helper.getsocketlocation(mesh, 9),   // left shoulder
					game_helper.getsocketlocation(mesh, 10),  // left elbow
					game_helper.getsocketlocation(mesh, 11),  // left hand
					game_helper.getsocketlocation(mesh, 38),  // right shoulder
					game_helper.getsocketlocation(mesh, 39),  // right elbow
					game_helper.getsocketlocation(mesh, 40),  // right hand
					game_helper.getsocketlocation(mesh, 2),   // pelvis
					game_helper.getsocketlocation(mesh, 71),  // left hip
					game_helper.getsocketlocation(mesh, 72),  // left knee
					game_helper.getsocketlocation(mesh, 78),  // right hip
					game_helper.getsocketlocation(mesh, 79),  // right knee
					game_helper.getsocketlocation(mesh, 75),  // left foot
					game_helper.getsocketlocation(mesh, 82),  // right foot
					game_helper.getsocketlocation(mesh, 11)   // head
				};

				float time = ImGui::GetTime();
				std::vector<fvector2d> screenPositions(bones.size());
				for (size_t i = 0; i < bones.size(); ++i) {
					screenPositions[i] = game_helper.ProjectWorldToScreen(bones[i]);
					float wave_phase = static_cast<float>(i);
					screenPositions[i].x += std::sin(wave_phase + screenPositions[i].y * 0.05f + time * 5.0f) * 5.0f;
					screenPositions[i].y += std::cos(wave_phase + screenPositions[i].x * 0.05f + time * 5.0f) * 5.0f;
				}

				auto draw_wavy_line = [](fvector2d from, fvector2d to, ImColor color, float thickness = 1.0f, float amplitude = 4.0f, int segments = 12)
					{
						for (int i = 0; i < segments; ++i)
						{
							float t1 = (float)i / segments;
							float t2 = (float)(i + 1) / segments;

							fvector2d p1 = {
								from.x + (to.x - from.x) * t1 + std::sin(t1 * 6.283f) * amplitude,
								from.y + (to.y - from.y) * t1 + std::cos(t1 * 6.283f) * amplitude
							};
							fvector2d p2 = {
								from.x + (to.x - from.x) * t2 + std::sin(t2 * 6.283f) * amplitude,
								from.y + (to.y - from.y) * t2 + std::cos(t2 * 6.283f) * amplitude
							};

							draw_line(p1, p2, color, thickness);
						}
					};

				std::vector<std::pair<int, int>> bone_pairs = {
					{1, 4}, {1, 2}, {2, 3}, {4, 5}, {5, 6},
					{0, 7}, {7, 8}, {8, 9}, {9, 12}, {7, 10},
					{10, 11}, {11, 13}
				};

				if (visuals.skel_outline)
				{
					for (auto& [start, end] : bone_pairs)
					{
						draw_wavy_line(screenPositions[start], screenPositions[end], ImColor(0, 0, 0), visuals.skeleton_thickness + 2.0f);
					}
				}

				for (auto& [start, end] : bone_pairs)
				{
					draw_wavy_line(screenPositions[start], screenPositions[end], box_color, visuals.skeleton_thickness);
				}
			}



		}
		if (visuals.head_esp && visuals.enable)
		{
			draw_head(box_color, head3d);
		}
		if (visuals.player_count) {
			char rendercount[256];
			sprintf(rendercount, "Render Conut: %d", player_count);
			ImVec2 text_size = ImGui::CalcTextSize(rendercount);
			float x = (globals.ScreenWidth - text_size.x) / 2.0f;
			float y = 80.0f;
			ImGui::GetBackgroundDrawList()->AddText(ImGui::GetFont(), 15.0f, ImVec2(x, y), ImColor(255, 255, 255, 255), rendercount);
		}

		if (triggerbot.triggerbot_enable && (GetAsyncKeyState(triggerbot.triggerbot_key) < 0))
		{
			std::string weapon_name = decryption.GetWeaponName(WorldCache.AcknownledgedPawn);
			std::transform(weapon_name.begin(), weapon_name.end(), weapon_name.begin(), ::tolower);
			if (aimbot.shotgunonly)
			{
				if (weapon_name.find("shotgun") != std::string::npos)
				{
					if ((read<uintptr_t>(WorldCache.PlayerController + offsets::TargetedFortPawn)))
					{
						if (distance <= triggerbot.triggerbot_distance)
						{
							Sleep(triggerbot.delay);
							utils.left_click();
						}
					}
				}
			}
			else {
				if ((read<uintptr_t>(WorldCache.PlayerController + offsets::TargetedFortPawn)))
				{
					if (distance <= triggerbot.triggerbot_distance)
					{
						Sleep(triggerbot.delay);
						utils.left_click();
					}
				}
			}
		}

		if (aimbot.fov_arrows)
		{
			fvector root_bone = game_helper.getsocketlocation(SkeletalMesh, 0); //skeletalmesh will be SkeletonMesh for you

			decryption.AddPlayerToFovCircle(root_bone, 187, game_helper.IsEnemyVisible(SkeletalMesh), ImColor(255, 0, 0)); // change coloring there ofc
		}

		entity cached_actors{ };
		cached_actors.entity = CurrentActor;
		cached_actors.skeletal_mesh = skeletalmesh;
		cached_actors.player_state = PlayerState;
		cached_actors.pawn_private = pawn_private;

		temporary_entity_list.push_back(cached_actors);
	}

	if (TargetEntity && aimbot.enable)
	{
		auto ClosestMesh = read<uint64_t>(TargetEntity + offsets::Mesh);

		game_helper.get_camera();
		fvector Hitbox;

		if (aimbot.Hitbox == 0)
			Hitbox = game_helper.getsocketlocation(ClosestMesh, 110);
		else if (aimbot.Hitbox == 1)
			Hitbox = game_helper.getsocketlocation(ClosestMesh, 66);
		else if (aimbot.Hitbox == 2)
			Hitbox = game_helper.getsocketlocation(ClosestMesh, 7);
		else if (aimbot.Hitbox == 3)
			Hitbox = game_helper.getsocketlocation(ClosestMesh, 2);
		else if (aimbot.Hitbox == 4) {
			std::mt19937 rng(static_cast<unsigned int>(std::time(nullptr)));
			std::uniform_int_distribution<int> distribution(0, 3);
			switch (distribution(rng)) {
			case 0: Hitbox = game_helper.getsocketlocation(ClosestMesh, 110); break;
			case 1: Hitbox = game_helper.getsocketlocation(ClosestMesh, 66); break;
			case 2: Hitbox = game_helper.getsocketlocation(ClosestMesh, 7); break;
			case 3: Hitbox = game_helper.getsocketlocation(ClosestMesh, 2); break;
			}
		}

		fvector HitboxHead = game_helper.getsocketlocation(ClosestMesh, 110);
		fvector2d HitboxHeadScreen = game_helper.ProjectWorldToScreen(fvector(HitboxHead.x, HitboxHead.y, HitboxHead.z +50));
		fvector2d HitboxScreen = game_helper.ProjectWorldToScreen(Hitbox);
		ImVec2 screen_center(globals.ScreenWidth / 2, globals.ScreenHeight / 2);
		ImVec2 target;

		if (aimbot.target_line)
		{
			ImGui::GetBackgroundDrawList()->AddLine(ImVec2(globals.ScreenWidth / 2, 0), ImVec2(HitboxHeadScreen.x, HitboxHeadScreen.y), ImColor(255, 0, 0), 1.f);
		}

		if (HitboxScreen.x != 0 || HitboxScreen.y != 0 && (get_cross_distance(HitboxScreen.x, HitboxScreen.y, globals.ScreenWidth / 2, globals.ScreenHeight / 2) <= aimbot.fovsize))
		{
			if (aimbot.vischeck ? game_helper.IsEnemyVisible(ClosestMesh) : true) {

				if (GetAsyncKeyState(aimbot.aimkey) && aimbot.mouse)
				{

					if (aimbot.prediction)
					{
						entity cached_actors{ };
						PlayerBounds o{ };
						std::string weaponname = decryption.GetWeaponName(o.pawn_priv);
						float projectileSpeed;
						float projectileGravityScale;

						projectileSpeed = read<float>(WorldCache.LocalWeapon + 0x1E20);
						projectileGravityScale = read<float>(WorldCache.LocalWeapon + 0x1FAC);
						
						
						auto root = read<uintptr_t>(TargetEntity + offsets::RootComponent);
						auto distance = camera_postion.location.distance(Hitbox);
						fvector velocity = read<fvector>(root + 0x180);

						if (projectileSpeed > 1000)
						{
							Hitbox = game_helper.PredictLocation(Hitbox, velocity, projectileSpeed, projectileGravityScale, distance);
						}

						HitboxScreen = game_helper.ProjectWorldToScreen(Hitbox);

						move(HitboxScreen);
					}
					else {
						
						move(HitboxScreen);
					}


				}
			}
		}
	}
	else
	{
		TargetDistance = FLT_MAX;
		TargetEntity = NULL;
	}
	entity_list.clear();
	entity_list = temporary_entity_list;
}