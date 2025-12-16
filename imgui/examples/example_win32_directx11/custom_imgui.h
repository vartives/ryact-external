#pragma once
#include "../../imgui.h"
#include <d3d11.h>
#include <unordered_map>
#include <string>
#include "../../imgui_internal.h"
#include <map>
#include <variant>

struct sub_tab_s
{
    sub_tab_s() {};
    float anim{ 0.0f };
    //add your custom anims
};
struct slider_s
{
    slider_s() {};
    float grab_anim{ 1.0f };
    float range_anim[2]{ 1.0f, 1.0f };
};
struct combo_s
{
    combo_s() {};
    float anim{ -1.0f };
    std::map<ImGuiID, float> anim_element_v;
};
struct cpicker_s
{
    cpicker_s() {};
    float anim{ 0.0f };
    float blob_anim[3]{ 1.0f, 1.0f, 1.0f };
};
struct tab_s
{
    tab_s(int cur_tab) { tab = cur_tab; };
    int tab{};
    bool active{};
    int sub_tab{};
    float anim{ 1.0f };

    int old_sub_tab{};
    float child_anim{ 0.0f };

    std::map<ImGuiID, sub_tab_s> anim_sub_v;
};
struct opt_s
{
    opt_s() {};
    int sub_opt{};
    bool open_popup{};
    bool open_sub_popup{};
    float anim{ 0.0f };
    std::map<ImGuiID, float> anim_spopup_v;
    //add your custom options
};
namespace cgui
{
    extern bool Selectable_tab(const char* name, bool selected, float anim = 1.f, float anim_sel = 1.f);

    //helpers for helpers )))
    template <typename T>
    extern void clip(T& n, const T& lower, const T& upper);
    template <typename T>
    extern T clipr(T n, const T& lower, const T& upper);
    extern float CalcMaxPopupHeightFromItemCount(int items_count);
    extern const char* Items_ArrayGetter(void* data, int idx);
    extern void ColorEditRestoreHS(const float* col, float* H, float* S, float* V);
    extern void ColorEditRestoreH(const float* col, float* H);
    extern float bz4(float x);
    extern float clipbz4(float x);

    //data for tabs
    extern std::unordered_map<std::string, tab_s> tabs_v;
    extern int cur_active;
    extern int cur_tab;

    //data for options
    extern std::unordered_map<std::string, opt_s> opts_v;

    //data for widget anims
    extern std::map<ImGuiID, float> anim_checkbox_v;
    extern std::map<ImGuiID, slider_s> anim_slider_v;
    extern std::map<ImGuiID, combo_s> anim_combo_v;
    extern std::map<ImGuiID, cpicker_s> anim_cpicker_v;


}

struct cgui_t
{
    ImFont* logo{ };
    ImFont* tab{ };
    ID3D11ShaderResourceView* logo_t = nullptr;
    ID3D11ShaderResourceView* tab_t[5] = { nullptr, nullptr, nullptr, nullptr, nullptr };
    ID3D11ShaderResourceView* empty_t = nullptr;
    ID3D11ShaderResourceView* search_t = nullptr;
    ID3D11ShaderResourceView* gear_t = nullptr;
    ID3D11ShaderResourceView* colpicker_t = nullptr;
    ID3D11ShaderResourceView* user_t = nullptr;
    ID3D11ShaderResourceView* pipe_t = nullptr;
    std::string user_name = "MonixLITE";
    std::string till_data = "13.01.2025";
    //fuck this
    bool opened_by_search{};
};

//data of textures & fonts & etc...
extern cgui_t g_cgui;
