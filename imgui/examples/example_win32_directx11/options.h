#pragma once
#pragma once
#include <string>
#include <variant>
#include <vector>

struct options_t
{
    bool enable_ragebot{ false };
    bool silent_aimbot{ false };
    int filed_of_view{ 0 };

    int body_aimbot{ 0 };
    int safe_points{ 0 };

    bool enable_recoil{ false };
    int recoil_smoothness{ 0 };

    int hitchanceL{ 1 };
    int hitchanceR{ 50 };
    int min_damageL{ 1 };
    int min_damageR{ 50 };
    int max_misses{ 0 };
    bool static_point_scale{ false };
    bool head_safety_if_letal{ false };

    float pitch{ 0.f };
    float yaw{ 0.f };
    bool jitter{ false };

    ////

    bool enable_esp{ false };
    bool through_walls{ false };
    int dynamic_tracers{ 0 };
    bool dynamic_boxes{ false };
    float boxes_color[4]{ 1.f, 1.f, 1.f, 1.f };

    bool enable_glow{ false };
    int glow_power{ 0 };

    bool attachments{ false };
    float attachments_color[4]{ 1.f, 1.f, 1.f, 1.f };

    bool enable_chams{ false };
    float chams_color[4]{ 1.f, 1.f, 1.f, 1.f };
    bool backtrack{ false };
    float backtrack_color[4]{ 1.f, 1.f, 1.f, 1.f };

    bool skeleton{ false };
    int snaplines{ 0 };
    bool weapon{ false };
    float weapon_color[4]{ 1.f, 1.f, 1.f, 1.f };

    ////

    char text_field[128]{};
    char desc_text_field[128]{};
};

struct names_t
{
    std::string enable_ragebot{ "Enable ragebot" };
    std::string silent_aimbot{ "Silent aimbot" };
    std::string filed_of_view{ "Field of view" };

    std::string body_aimbot{ "Body aimbot" };
    std::string safe_points{ "Safe points" };

    std::string enable_recoil{ "Enable recoil" };
    std::string recoil_smoothness{ "Smoothness" };

    std::string hitchance{ "Hit chance" };
    std::string min_damage{ "Min. damage" };
    std::string max_misses{ "Max misses" };
    std::string static_point_scale{ "Static point scale" };
    std::string head_safety_if_letal{ "Head safety if lethal" };

    std::string pitch{ "Pitch" };
    std::string yaw{ "Yaw" };
    std::string jitter{ "Jitter" };

    ////

    std::string enable_esp{ "Enable ESP" };
    std::string through_walls{ "Through walls" };
    std::string dynamic_tracers{ "Dynamic tracer" };
    std::string dynamic_boxes{ "Dynamic boxes" };
    std::string boxes_color{ "Dynamic color" };

    std::string enable_glow{ "Enable glow" };
    std::string glow_power{ "Power" };

    std::string attachments{ "Attachments" };
    std::string attachments_color{ "Attachments color" };

    std::string enable_chams{ "Enable chams" };
    std::string chams_color{ "Chams color" };
    std::string backtrack{ "Backtrack" };
    std::string backtrack_color{ "Backtrack color" };

    std::string skeleton{ "Skeleton" };
    std::string snaplines{ "Snaplines" };
    std::string weapon{ "Weapon" };
    std::string weapon_color{ "Weapon color" };

    ////

    std::string text_field{ "Text field" };
    std::string desc_text_field{ "Another text field" };
};

struct desc_t
{
    std::string enable_ragebot{ "Automatic aiming and firing" };
    std::string silent_aimbot{ "Discreet automatic targeting" };
    std::string min_damage{ "Minimum damage" };
    std::string body_aimbot{ "Shooting to the body" };
    std::string safe_points{ "Safe aiming points" };
    std::string recoil_smoothness{ "Recoil control" };
    std::string hitchance{ "Probability of hitting" };
    std::string max_misses{ "Maximum misses to the head" };

    std::string jitter{ "Jittering your model" };

    ////

    std::string dynamic_tracers{ "Dynamic tracer type" };
    std::string boxes_color{ "Affects the color of the tracers" };

    std::string glow_power{ "Glow intensity" };
    std::string attachments_color{ "Weapon attachments color" };
    std::string backtrack{ "Highlight the model on the last tick" };

    std::string skeleton{ "Shows the player's skeleton" };
    std::string weapon{ "Shows the player's skeleton weapon" };

    ////

    std::string desc_text_field{ "Description of text field" };

};

struct items_t
{
    const char* subtabs[2] = { "First","Second" };
    const char* subopt[2] = { "First","Second" };
    const char* combo_items[2] = { "Option #1","Option #2" };
};

struct option_s
{
    options_t param;
    names_t name;
    desc_t desc;
    items_t item;
};

inline option_s* option = new option_s();

struct find_elements
{
    std::string name;
    std::string desc;
};

struct find_elements_bool : find_elements
{
    bool* value;
};

struct find_elements_int : find_elements
{
    int* value;
    int min;
    int max;
};

struct find_elements_int_d : find_elements
{
    int* value;
    const char** items;
    int elements;
};

struct find_elements_int_r : find_elements
{
    int* valueL;
    int* valueR;
    int min;
    int max;
};

struct find_elements_col : find_elements
{
    float* col;
};

typedef void (*func)(int sub_opt);

struct find_elements_bool_opt : find_elements_bool
{
    const char** opts;
    int numb_opts;
    func option;
};

struct find_elements_int_opt : find_elements_int
{
    const char** opts;
    int numb_opts;
    func option;
};

struct find_elements_int_r_opt : find_elements_int_r
{
    const char** opts;
    int numb_opts;
    func option;
};

struct find_elements_int_d_opt : find_elements_int_d
{
    const char** opts;
    int numb_opts;
    func option;
};

struct find_elements_col_opt : find_elements_col
{
    const char** opts;
    int numb_opts;
    func option;
};

class c_search
{
public:
    bool text_field_hovered{ false };
    float window_alpha{ 0.f };
    bool active_searcing{ false };

    std::string search_element{ };
    char search_buf[128]{ };

    using all_elements = std::variant<find_elements_col_opt, find_elements_int_d_opt, find_elements_int_r_opt, find_elements_int_opt, find_elements_bool, find_elements_int, find_elements_int_d, find_elements_col, find_elements_int_r, find_elements_bool_opt>;

    std::vector<all_elements> search_elements;

    void init_options();

private:
    bool initialized{ false };
};

inline c_search* search = new c_search();
