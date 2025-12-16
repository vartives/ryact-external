#define IMGUI_DEFINE_MATH_OPERATORS
#include "custom_imgui.h"
#include "../../imgui_internal.h"
#include <algorithm>
#include "options.h"
#include "imgui_settings.h"
extern ImFont* Inter_SemmiBold_1;
bool cgui::Selectable_tab(const char* name, bool selected, float anim, float anim_sel)
{
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems || anim <= 0.f)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(name);
    const ImVec2 label_size = ImGui::CalcTextSize(name, NULL, true);

    const float square_sz = ImGui::GetFrameHeight();
    const ImVec2 pos = window->DC.CursorPos;
    const ImVec2 size = ImVec2(234.f, 40.f * anim);
    const ImRect total_bb(pos, pos + ImVec2(size.x, size.y + 8.f));
    ImGui::ItemSize(total_bb, style.FramePadding.y);
    const bool is_visible = ImGui::ItemAdd(total_bb, id);

    if (!is_visible)
        return false;

    bool hovered, held;
    bool pressed = ImGui::ButtonBehavior(total_bb, id, &hovered, &held);
    const ImRect right_bb(pos + ImVec2(40.f, 0.f), pos + size);
    ImVec2 text_pos = ImVec2(pos.x + 40.f + 11.f, pos.y + 10.f);

    window->DrawList->AddLine(ImVec2(pos.x + 20.f, pos.y), ImVec2(pos.x + 20.f, pos.y + size.y), ImGui::GetColorU32(ImVec4(g.Style.Colors[ImGuiCol_TextDisabled].x, g.Style.Colors[ImGuiCol_TextDisabled].y, g.Style.Colors[ImGuiCol_TextDisabled].z, 0.1f * clipr(anim, 0.f, 1.f))), 1.5f);

    if ((anim * anim_sel) > 0.01f)
    {
        window->DrawList->AddRectFilled(right_bb.Min, right_bb.Max, ImGui::GetColorU32(c::main_color_tabs, clipr(anim, 0.f, 1.f) * clipr(anim_sel, 0.f, 1.f)), 8.f);
    }
    ImVec4 col_text = ImLerp(g.Style.Colors[ImGuiCol_TextDisabled], g.Style.Colors[ImGuiCol_Text], clipr(anim_sel, 0.f, 1.f));
    col_text.w *= clipr(anim, 0.f, 1.f);
    if (anim > 0.2f)
        window->DrawList->AddText(Inter_SemmiBold_1, 17.f, text_pos, ImGui::GetColorU32(col_text), name);

    return pressed;
}

//helpers for helpers )))
template <typename T>
void cgui::clip(T& n, const T& lower, const T& upper) {
    n = max(lower, min(n, upper));
}
template<typename T>
T cgui::clipr(T n, const T& lower, const T& upper)
{
    n = max(lower, min(n, upper));
    return n;
}
static float cgui::CalcMaxPopupHeightFromItemCount(int items_count)
{
    ImGuiContext& g = *GImGui;
    if (items_count <= 0)
        return FLT_MAX;
    return (g.FontSize + g.Style.ItemSpacing.y) * items_count - g.Style.ItemSpacing.y + (g.Style.WindowPadding.y * 2);
}
static const char* cgui::Items_ArrayGetter(void* data, int idx)
{
    const char* const* items = (const char* const*)data;
    return items[idx];
}
static void cgui::ColorEditRestoreHS(const float* col, float* H, float* S, float* V)
{
    ImGuiContext& g = *GImGui;
    IM_ASSERT(g.ColorEditCurrentID != 0);
    if (g.ColorEditSavedID != g.ColorEditCurrentID || g.ColorEditSavedColor != ImGui::ColorConvertFloat4ToU32(ImVec4(col[0], col[1], col[2], 0)))
        return;

    // When S == 0, H is undefined.
    // When H == 1 it wraps around to 0.
    if (*S == 0.0f || (*H == 0.0f && g.ColorEditSavedHue == 1))
        *H = g.ColorEditSavedHue;

    // When V == 0, S is undefined.
    if (*V == 0.0f)
        *S = g.ColorEditSavedSat;
}
static void cgui::ColorEditRestoreH(const float* col, float* H)
{
    ImGuiContext& g = *GImGui;
    IM_ASSERT(g.ColorEditCurrentID != 0);
    if (g.ColorEditSavedID != g.ColorEditCurrentID || g.ColorEditSavedColor != ImGui::ColorConvertFloat4ToU32(ImVec4(col[0], col[1], col[2], 0)))
        return;
    *H = g.ColorEditSavedHue;
}
float cgui::bz4(float x)
{
    return x * x * (3.0f - 2.0f * x);
}
float cgui::clipbz4(float x)
{
    return clipr(bz4(clipr(x, 0.f, 1.f)), 0.f, 1.f);
}

//data for tabs
std::unordered_map<std::string, tab_s> cgui::tabs_v{};


//data for options
std::unordered_map<std::string, opt_s> cgui::opts_v{};

// data for widget anims
std::map<ImGuiID, float> cgui::anim_checkbox_v{};
std::map<ImGuiID, slider_s> cgui::anim_slider_v{};
std::map<ImGuiID, combo_s> cgui::anim_combo_v{};
std::map<ImGuiID, cpicker_s> cgui::anim_cpicker_v{};

//data of textures & fonts & etc...
cgui_t g_cgui{};
