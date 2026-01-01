#pragma once

﻿#include <iostream>
#include <string>
#include "../../imgui/examples/example_win32_directx11/imgui_settings.h"

struct Renranding {
    LPCSTR name;
    const char* bigtext;
    const char* menuText;
    bool enableFeature;
    int consoleColor;
    LPCSTR shopLink;
    
};

const Renranding config2 = {
    "Ryact",
    R"()",
    "Ryact",
    false,
    13,
    "https://ryact.ltd"
};

int configMode = 2;  
const Renranding* activeConfig = nullptr;

void loadConfig() {
    activeConfig = &config2;

    c::rect = theme2::rect;
    c::main_color = theme2::main_color;
    c::main_color_1 = theme2::main_color_1;
    c::line_in_active = theme2::line_in_active;
    c::background = theme2::background;
    c::text_active = theme2::text_active;
    c::text_active_in_1 = theme2::text_active_in_1;
    c::text_active_in_active = theme2::text_active_in_active;
    c::text_red = theme2::text_red;
    c::text_green = theme2::text_green;
    c::text_active_in = theme2::text_active_in;
    c::text_active_in_alpha = theme2::text_active_in_alpha;
    c::text_group_tab = theme2::text_group_tab;
    c::line = theme2::line;
    c::rect_window = theme2::rect_window;
    c::icon_active = theme2::icon_active;
    c::icon_inactive = theme2::icon_inactive;
    c::background_active = theme2::background_active;
    c::background_inactive = theme2::background_inactive;
    c::select_tab_active = theme2::select_tab_active;
    c::select_tab_inactive = theme2::select_tab_inactive;
    c::background_icon = theme2::background_icon;
    c::background_icon_in = theme2::background_icon_in;
    c::icon_exit = theme2::icon_exit;
    c::icon_exit_in = theme2::icon_exit_in;
    c::child = theme2::child;
    c::child_rect = theme2::child_rect;
    c::window_bg = theme2::window_bg;
    c::checkbox_bg = theme2::checkbox_bg;
    c::checkbox_bg_in = theme2::checkbox_bg_in;
    c::rect_checkbox_bg = theme2::rect_checkbox_bg;
    c::rect_checkbox_bg_in = theme2::rect_checkbox_bg_in;
    c::separator = theme2::separator;
    c::background_slider = theme2::background_slider;
    c::text_slider_value = theme2::text_slider_value;
    c::settings_checkbox = theme2::settings_checkbox;
    c::rect_input = theme2::rect_input;
    c::bg_input = theme2::bg_input;
    c::bg_selectable = theme2::bg_selectable;
    c::bg_selectable_in = theme2::bg_selectable_in;
    c::bg_notif = theme2::bg_notif;
    c::input_scale = theme2::input_scale;
    c::keybind_bg = theme2::keybind_bg;
    c::selected_bg = theme2::selected_bg;
    c::selected_rect = theme2::selected_rect;
    c::selected_inactiv = theme2::selected_inactiv;
    c::main_color_tabs = theme2::main_color_tabs;
    c::icon_in_activ = theme2::icon_in_activ;
    c::bg_scrollbar = theme2::bg_scrollbar;
    c::bg_element = theme2::bg_element;
    c::bg_element_1 = theme2::bg_element_1;
    c::esp_bg = theme2::esp_bg;
    c::input = theme2::input;
    c::black = theme2::black;
    c::columns = theme2::columns;
}

